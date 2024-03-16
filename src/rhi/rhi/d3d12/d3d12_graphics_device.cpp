#include "d3d12_graphics_device.hpp"
#include "d3d12_command_list.hpp"

#include <core/d3d12/d3d12_pso.hpp>
#include <core/d3d12/d3d12_descriptor_util.hpp>
#include <D3D12MemAlloc.h>

namespace rhi::d3d12
{
constexpr static uint32_t MAX_RTV_DSV_DESCRIPTORS = 2048;

Result result_from_hresult(HRESULT hresult) noexcept
{
    switch (hresult)
    {
    case E_INVALIDARG:
        return Result::Error_Invalid_Parameters;
    case E_OUTOFMEMORY:
        return Result::Error_Out_Of_Memory;
    case DXGI_ERROR_DEVICE_REMOVED:
        [[fallthrough]];
    case DXGI_ERROR_DEVICE_HUNG:
        return Result::Error_Device_Lost;
    case E_FAIL:
        return Result::Error_Unknown;
    default:
        return Result::Success;
    }
}

D3D12_HEAP_TYPE translate_heap_type(Memory_Heap_Type heap_type) noexcept
{
    switch (heap_type)
    {
    case Memory_Heap_Type::GPU:
        return D3D12_HEAP_TYPE_DEFAULT;
    case Memory_Heap_Type::CPU_Upload:
        return D3D12_HEAP_TYPE_UPLOAD;
    case Memory_Heap_Type::CPU_Readback:
        return D3D12_HEAP_TYPE_READBACK;
    default:
        return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_Graphics_Device::D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept
    : Graphics_Device()
    , m_context{}
    , m_allocator()
    , m_descriptor_increment_sizes{}
    , m_indirect_signatures{}
    , m_use_mutex(create_info.enable_locking)
    , m_resource_mutex()
    , m_direct_queue_mutex()
    , m_compute_queue_mutex()
    , m_copy_queue_mutex()
    , m_fences()
    , m_buffers()
    , m_buffer_views()
    , m_images()
    , m_image_views()
    , m_samplers()
    , m_shader_blobs()
    , m_pipelines()
{
    core::d3d12::D3D12_Context_Create_Info context_create_info = {
        .enable_validation = create_info.enable_validation,
        .enable_gpu_validation = create_info.enable_gpu_validation,
        .disable_tdr = false,
        .feature_level = D3D_FEATURE_LEVEL_12_2, // TODO: customizable feature level?
        .resource_descriptor_heap_size = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
        .sampler_descriptor_heap_size = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE,
        .rtv_descriptor_heap_size = MAX_RTV_DSV_DESCRIPTORS,
        .dsv_descriptor_heap_size = MAX_RTV_DSV_DESCRIPTORS,
        .push_constant_size = 64,
        .static_samplers = {}
    };
    core::d3d12::create_d3d12_context(context_create_info, &m_context);
    D3D12MA::ALLOCATOR_DESC allocator_desc = {
        .Flags = D3D12MA::ALLOCATOR_FLAG_SINGLETHREADED,
        .pDevice = m_context.device,
        .PreferredBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pAdapter = m_context.adapter
    };
    D3D12MA::CreateAllocator(&allocator_desc, &m_allocator);

    m_descriptor_increment_sizes = acquire_descriptor_increment_sizes();
    m_indirect_signatures = create_execute_indirect_signatures();
}

D3D12_Graphics_Device::~D3D12_Graphics_Device() noexcept
{
    core::d3d12::await_context(&m_context);

    // Release everything that was not released by the user
    // TODO: Should this be done or should a leak be mentioned by the validation layer instead?
    for (auto& buffer : m_buffers)
    {
        buffer.resource->Release();
        buffer.allocation->Release();
    }
    for (auto& image : m_images)
    {
        image.resource->Release();
        image.allocation->Release();
    }
    for (auto& pipeline : m_pipelines)
    {
        if (pipeline.type == Pipeline_Type::Ray_Tracing)
        {
            pipeline.rtpso->Release();
        }
        else
        {
            pipeline.pso->Release();
        }
    }

    m_indirect_signatures.draw_indirect->Release();
    m_indirect_signatures.draw_indexed_indirect->Release();
    m_indirect_signatures.draw_mesh_tasks_indirect->Release();
    m_indirect_signatures.dispatch_indirect->Release();
    m_allocator->Release();
    core::d3d12::destroy_d3d12_context(&m_context);
}

Graphics_API D3D12_Graphics_Device::get_graphics_api() const noexcept
{
    return Graphics_API::D3D12;
}

std::expected<Fence*, Result> D3D12_Graphics_Device::create_fence(uint64_t initial_value) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    ID3D12Fence1* d3d12_fence = nullptr;
    auto result = result_from_hresult(
        m_context.device->CreateFence(
            initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12_fence)));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    D3D12_Fence* fence = m_fences.acquire();
    fence->fence = d3d12_fence;
    return fence;
}

void D3D12_Graphics_Device::destroy_fence(Fence* fence) noexcept
{
    if (!fence) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto d3d12_fence = static_cast<D3D12_Fence*>(fence);
    d3d12_fence->fence->Release();
    m_fences.release(d3d12_fence);
}

std::expected<Buffer*, Result> D3D12_Graphics_Device::create_buffer(const Buffer_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    D3D12_RESOURCE_DESC1 resource_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0,
        .Width = create_info.size,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = { .Count = 1, .Quality = 0 },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        .SamplerFeedbackMipRegion = {}
    };
    D3D12MA::ALLOCATION_DESC allocation_desc = {
        .Flags = D3D12MA::ALLOCATION_FLAG_NONE,
        .HeapType = translate_heap_type(create_info.heap),
        .ExtraHeapFlags = D3D12_HEAP_FLAG_NONE,
        .CustomPool = nullptr,
        .pPrivateData = nullptr
    };
    D3D12MA::Allocation* allocation = nullptr;
    ID3D12Resource2* resource = nullptr;
    auto result = result_from_hresult(m_allocator->CreateResource3(
        &allocation_desc, &resource_desc, D3D12_BARRIER_LAYOUT_UNDEFINED,
        nullptr, 0, nullptr, &allocation, IID_PPV_ARGS(&resource)));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    void* mapped_data = nullptr;
    if (create_info.heap == Memory_Heap_Type::CPU_Upload)
    {
        resource->Map(0, nullptr, &mapped_data);
    }

    auto buffer = m_buffers.acquire();
    buffer->size = create_info.size;
    buffer->heap_type = create_info.heap;
    buffer->buffer_view = m_buffer_views.acquire();
    buffer->buffer_view->bindless_index = create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    buffer->buffer_view->size = buffer->size;
    buffer->buffer_view->offset = 0;
    buffer->buffer_view->buffer = buffer;
    static_cast<D3D12_Buffer_View*>(buffer->buffer_view)->next_buffer_view = nullptr;
    buffer->data = mapped_data;
    buffer->resource = resource;
    buffer->allocation = allocation;
    buffer->buffer_view_linked_list_head = nullptr;

    create_initial_buffer_descriptors(buffer);

    return buffer;
}

std::expected<Buffer_View*, Result> D3D12_Graphics_Device::create_buffer_view(
    Buffer* buffer, const Buffer_View_Create_Info& create_info) noexcept
{
    if (!buffer) return std::unexpected(Result::Error_Invalid_Parameters);

    if (create_info.size + create_info.offset > buffer->size)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto buffer_view = m_buffer_views.acquire();
    buffer_view->bindless_index = create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    buffer_view->size = create_info.size;
    buffer_view->offset = create_info.offset;
    buffer_view->buffer = buffer;
    buffer_view->next_buffer_view = static_cast<D3D12_Buffer*>(buffer)->buffer_view_linked_list_head;
    static_cast<D3D12_Buffer*>(buffer)->buffer_view_linked_list_head = buffer_view;

    auto srv_uav_word_offset = buffer_view->offset >> 2; // counting number of 4 byte elements.
    auto srv_desc = core::d3d12::make_raw_buffer_srv(buffer_view->size);
    srv_desc.Buffer.FirstElement = srv_uav_word_offset;
    auto uav_desc = core::d3d12::make_raw_buffer_uav(buffer_view->size);
    uav_desc.Buffer.FirstElement = srv_uav_word_offset;
    create_srv_and_uav(static_cast<D3D12_Buffer*>(buffer_view->buffer)->resource,
        buffer_view->bindless_index, &srv_desc, &uav_desc);

    return buffer_view;
}

void D3D12_Graphics_Device::destroy_buffer(Buffer* buffer) noexcept
{
    if (!buffer) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    d3d12_buffer->resource->Release();
    d3d12_buffer->allocation->Release();

    release_bindless_index(buffer->buffer_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    auto next_buffer_view = d3d12_buffer->buffer_view_linked_list_head;
    while (next_buffer_view != nullptr)
    {
        auto current_buffer_view = next_buffer_view;
        release_bindless_index(current_buffer_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        next_buffer_view = current_buffer_view->next_buffer_view;
        m_buffer_views.release(current_buffer_view);
    }

    m_buffers.release(d3d12_buffer);
}

std::expected<Image*, Result> D3D12_Graphics_Device::create_image(const Image_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
    if (uint32_t(create_info.usage & Image_Usage::Color_Attachment) > 0)
    {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }
    if (uint32_t(create_info.usage & Image_Usage::Depth_Stencil_Attachment) > 0)
    {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }
    if (uint32_t(create_info.usage & Image_Usage::Unordered_Access) > 0)
    {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }
    // illegal to be both color and depth attachment.
    if ((create_info.usage & (Image_Usage::Color_Attachment | Image_Usage::Depth_Stencil_Attachment))
        == (Image_Usage::Color_Attachment | Image_Usage::Depth_Stencil_Attachment))
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    bool is_array_type = false;
    switch (create_info.primary_view_type)
    {
    case Image_View_Type::Texture_1D_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube_Array:
        is_array_type = true;
        break;
    default:
        break;
    }

    bool is_rtv = false;
    bool is_dsv = false;

    D3D12_RESOURCE_DESC1 resource_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0,
        .Width = create_info.width,
        .Height = create_info.height,
        .DepthOrArraySize = is_array_type ? uint16_t(create_info.array_size) : uint16_t(create_info.depth),
        .MipLevels = create_info.mip_levels,
        .Format = translate_format(create_info.format),
        .SampleDesc = {.Count = 1, .Quality = 0 }, // TODO: implement MS textures for D3D12 Backend.
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = flags,
        .SamplerFeedbackMipRegion = {}
    };
    D3D12MA::ALLOCATION_DESC allocation_desc = {
        .Flags = D3D12MA::ALLOCATION_FLAG_NONE,
        .HeapType = D3D12_HEAP_TYPE_DEFAULT,
        .ExtraHeapFlags = D3D12_HEAP_FLAG_NONE,
        .CustomPool = nullptr,
        .pPrivateData = nullptr
    };
    D3D12MA::Allocation* allocation = nullptr;
    ID3D12Resource2* resource = nullptr;
    auto result = result_from_hresult(m_allocator->CreateResource3(
        &allocation_desc, &resource_desc, D3D12_BARRIER_LAYOUT_UNDEFINED,
        nullptr, 0, nullptr, &allocation, IID_PPV_ARGS(&resource)));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    auto image = m_images.acquire();
    image->format = create_info.format;
    image->width = create_info.width;
    image->height = create_info.height;
    image->depth = create_info.depth;
    image->array_size = create_info.array_size;
    image->mip_levels = create_info.mip_levels;
    image->usage = create_info.usage;
    image->primary_view_type = create_info.primary_view_type;
    image->image_view = m_image_views.acquire();
    image->image_view->bindless_index = create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    image->image_view->image = image;
    static_cast<D3D12_Image_View*>(image->image_view)->rtv_dsv_index = is_rtv
        ? create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
        : is_dsv
            ? create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
            : 0;
    static_cast<D3D12_Image_View*>(image->image_view)->next_image_view = nullptr;
    image->resource = resource;
    image->allocation = allocation;
    image->image_view_linked_list_head = nullptr;

    return image;
}

std::expected<Image_View*, Result> D3D12_Graphics_Device::create_image_view(
    Image* image, const Image_View_Create_Info& create_info) noexcept
{
    if (!image) return std::unexpected(Result::Error_Invalid_Parameters);

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto d3d12_image = static_cast<D3D12_Image*>(image);
    auto image_view = m_image_views.acquire();

    image_view->next_image_view = d3d12_image->image_view_linked_list_head;
    d3d12_image->image_view_linked_list_head = image_view;

    return image_view;
}

void D3D12_Graphics_Device::destroy_image(Image* image) noexcept
{
    if (!image) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto d3d12_image = static_cast<D3D12_Image*>(image);
    d3d12_image->resource->Release();
    d3d12_image->allocation->Release();

    release_bindless_index(d3d12_image->image_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    if (bool(image->usage & Image_Usage::Color_Attachment))
    {
        release_bindless_index(static_cast<D3D12_Image_View*>(d3d12_image->image_view)->rtv_dsv_index,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    if (bool(image->usage & Image_Usage::Depth_Stencil_Attachment))
    {
        release_bindless_index(static_cast<D3D12_Image_View*>(d3d12_image->image_view)->rtv_dsv_index,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    auto next_image_view = d3d12_image->image_view_linked_list_head;
    while (next_image_view != nullptr)
    {
        auto current_image_view = next_image_view;
        release_bindless_index(current_image_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        next_image_view = current_image_view->next_image_view;
        m_image_views.release(current_image_view);
    }

    m_images.release(d3d12_image);
}

std::expected<Shader_Blob*, Result> D3D12_Graphics_Device::create_shader_blob(void* data, uint64_t size) noexcept
{
    static_assert(
        sizeof(decltype(Shader_Blob::data)::value_type) == sizeof(uint8_t),
        "Size of blob changed.");

    if (data == nullptr || size == 0)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto blob = m_shader_blobs.acquire();
    blob->data.reserve(size);
    memcpy(blob->data.data(), data, size);
    return blob;
}

void D3D12_Graphics_Device::destroy_shader_blob(Shader_Blob* shader_blob) noexcept
{
    if (shader_blob == nullptr) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_shader_blobs.release(shader_blob);
}

D3D12_SHADER_BYTECODE translate_shader_blob_safe(Shader_Blob* blob)
{
    D3D12_SHADER_BYTECODE bytecode = {};
    if (blob)
    {
        bytecode.BytecodeLength = blob->data.size();
        bytecode.pShaderBytecode = blob->data.data();
    }
    return bytecode;
}

D3D12_FILL_MODE translate_fill_mode(Fill_Mode fill_mode)
{
    return fill_mode == Fill_Mode::Solid
        ? D3D12_FILL_MODE_SOLID
        : D3D12_FILL_MODE_WIREFRAME;
}

D3D12_CULL_MODE translate_cull_mode(Cull_Mode cull_mode)
{
    switch (cull_mode)
    {
    case rhi::Cull_Mode::None:
        return D3D12_CULL_MODE_NONE;
    case rhi::Cull_Mode::Front:
        return D3D12_CULL_MODE_FRONT;
    case rhi::Cull_Mode::Back:
        return D3D12_CULL_MODE_BACK;
    default:
        return D3D12_CULL_MODE_NONE;
    }
    return D3D12_CULL_MODE_NONE;
}

D3D12_RASTERIZER_DESC1 translate_rasterizer_desc(const Pipeline_Rasterization_State_Info& raster_info)
{
    D3D12_RASTERIZER_DESC1 result = {
        .FillMode = translate_fill_mode(raster_info.fill_mode),
        .CullMode = translate_cull_mode(raster_info.cull_mode),
        .FrontCounterClockwise = raster_info.winding_order == Winding_Order::Front_Face_CCW ? true : false,
        .DepthBias = raster_info.depth_bias,
        .DepthBiasClamp = raster_info.depth_bias_clamp,
        .SlopeScaledDepthBias = raster_info.depth_bias_slope_scale,
        .DepthClipEnable = raster_info.depth_clip_enable,
        .MultisampleEnable = false,
        .AntialiasedLineEnable = false,
        .ForcedSampleCount = 0,
        .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };
    return result;
}

D3D12_BLEND translate_blend(Blend_Factor blend_factor)
{
    switch (blend_factor)
    {
    case Blend_Factor::Zero:
        return D3D12_BLEND_ZERO;
    case Blend_Factor::One:
        return D3D12_BLEND_ONE;
    case Blend_Factor::Src_Color:
        return D3D12_BLEND_SRC_COLOR;
    case Blend_Factor::One_Minus_Src_Color:
        return D3D12_BLEND_INV_SRC_COLOR;
    case Blend_Factor::Dst_Color:
        return D3D12_BLEND_DEST_COLOR;
    case Blend_Factor::One_Minus_Dst_Color:
        return D3D12_BLEND_INV_DEST_COLOR;
    case Blend_Factor::Src_Alpha:
        return D3D12_BLEND_SRC_ALPHA;
    case Blend_Factor::One_Minus_Src_Alpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case Blend_Factor::Dst_Alpha:
        return D3D12_BLEND_DEST_ALPHA;
    case Blend_Factor::One_Minus_Dst_Alpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
    case Blend_Factor::Constant_Color:
        return D3D12_BLEND_BLEND_FACTOR;
    case Blend_Factor::One_Minus_Constant_Color:
        return D3D12_BLEND_INV_BLEND_FACTOR;
    case Blend_Factor::Constant_Alpha:
        return D3D12_BLEND_ALPHA_FACTOR;
    case Blend_Factor::One_Minus_Constant_Alpha:
        return D3D12_BLEND_INV_ALPHA_FACTOR;
    case Blend_Factor::Src1_Color:
        return D3D12_BLEND_SRC1_COLOR;
    case Blend_Factor::One_Minus_Src1_Color:
        return D3D12_BLEND_INV_SRC1_COLOR;
    case Blend_Factor::Src1_Alpha:
        return D3D12_BLEND_SRC1_ALPHA;
    case Blend_Factor::One_Minus_Src1_Alpha:
        return D3D12_BLEND_INV_SRC1_ALPHA;
    default:
        return D3D12_BLEND_ZERO;
    }
}

D3D12_BLEND_OP translate_blend_op(Blend_Op blend_op)
{
    return std::bit_cast<D3D12_BLEND_OP>(blend_op);
}

D3D12_LOGIC_OP translate_logic_op(Logic_Op logic_op)
{
    return std::bit_cast<D3D12_LOGIC_OP>(logic_op);
}

uint8_t translate_renter_target_write_mask(Color_Component components)
{
    return static_cast<uint8_t>(components);
}

D3D12_RENDER_TARGET_BLEND_DESC translate_render_target_blend_desc(
    const Pipeline_Color_Attachment_Blend_Info& attachment_blend_info)
{
    D3D12_RENDER_TARGET_BLEND_DESC result = {
        .BlendEnable = attachment_blend_info.blend_enable,
        .LogicOpEnable = attachment_blend_info.logic_op_enable,
        .SrcBlend = translate_blend(attachment_blend_info.color_src_blend),
        .DestBlend = translate_blend(attachment_blend_info.color_dst_blend),
        .BlendOp = translate_blend_op(attachment_blend_info.color_blend_op),
        .SrcBlendAlpha = translate_blend(attachment_blend_info.alpha_src_blend),
        .DestBlendAlpha = translate_blend(attachment_blend_info.alpha_dst_blend),
        .BlendOpAlpha = translate_blend_op(attachment_blend_info.alpha_blend_op),
        .LogicOp = translate_logic_op(attachment_blend_info.logic_op),
        .RenderTargetWriteMask = translate_renter_target_write_mask(attachment_blend_info.color_write_mask)
    };
    return result;
}

D3D12_BLEND_DESC translate_blend_state_desc(const Pipeline_Blend_State_Info& blend_info)
{
    D3D12_BLEND_DESC result = {
        .AlphaToCoverageEnable = false,
        .IndependentBlendEnable = blend_info.independent_blend_enable,
        .RenderTarget = {}
    };
    for (auto i = 0; i < blend_info.color_attachments.size(); ++i)
    {
        result.RenderTarget[i] = translate_render_target_blend_desc(blend_info.color_attachments[i]);
    }
    return result;
}

D3D12_COMPARISON_FUNC translate_comparison_func(Comparison_Func comparison_func)
{
    return std::bit_cast<D3D12_COMPARISON_FUNC>(comparison_func);
}

D3D12_STENCIL_OP translate_stencil_op(Stencil_Op stencil_op)
{
    return std::bit_cast<D3D12_STENCIL_OP>(stencil_op);
}

D3D12_DEPTH_STENCILOP_DESC1 translate_depth_stencilop_desc(const Pipeline_Depth_Stencil_Op_Info& ds_op_info)
{
    D3D12_DEPTH_STENCILOP_DESC1 result = {
        .StencilFailOp = translate_stencil_op(ds_op_info.fail),
        .StencilDepthFailOp = translate_stencil_op(ds_op_info.depth_fail),
        .StencilPassOp = translate_stencil_op(ds_op_info.pass),
        .StencilFunc = translate_comparison_func(ds_op_info.comparison_func),
        .StencilReadMask = ds_op_info.stencil_read_mask,
        .StencilWriteMask = ds_op_info.stencil_write_mask
    };
    return result;
}

D3D12_DEPTH_STENCIL_DESC2 translate_depth_stencil_desc(const Pipeline_Depth_Stencil_State_Info& ds_info)
{
    D3D12_DEPTH_STENCIL_DESC2 result = {
        .DepthEnable = ds_info.depth_enable,
        .DepthWriteMask = ds_info.depth_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc = translate_comparison_func(ds_info.comparison_func),
        .StencilEnable = ds_info.stencil_enable,
        .FrontFace = translate_depth_stencilop_desc(ds_info.stencil_front_face),
        .BackFace = translate_depth_stencilop_desc(ds_info.stencil_back_face),
        .DepthBoundsTestEnable = ds_info.depth_bounds_test_mode == Depth_Bounds_Test_Mode::Disabled
            ? false
            : true
    };
    return result;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE translate_primitive_topology_type(Primitive_Topology_Type topology)
{
    return std::bit_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(topology);
}

std::expected<Pipeline*, Result> D3D12_Graphics_Device::create_pipeline(
    const Graphics_Pipeline_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    core::d3d12::Graphics_Pipeline_Desc graphics_pipeline_stream = {
        .root_signature = { .data = m_context.bindless_root_signature },
        .vs = { .data = translate_shader_blob_safe(create_info.vs) },
        .hs = { .data = translate_shader_blob_safe(create_info.hs) },
        .ds = { .data = translate_shader_blob_safe(create_info.ds) },
        .gs = { .data = translate_shader_blob_safe(create_info.gs) },
        .ps = { .data = translate_shader_blob_safe(create_info.ps) },
        .stream_output = { .data = {} },
        .blend_state = { .data = translate_blend_state_desc(create_info.blend_state_info)},
        .sample_mask = { .data = ~0u },
        .rasterizer_state = { .data = translate_rasterizer_desc(create_info.rasterizer_state_info)},
        .depth_stencil_state = { .data = translate_depth_stencil_desc(create_info.depth_stencil_info)},
        .input_layout_desc = { .data = {} },
        .primitive_topology_type = { .data = translate_primitive_topology_type(create_info.primitive_topology) },
        .render_target_formats = { .data = { .NumRenderTargets = create_info.color_attachment_count } },
        .depth_stencil_format = { .data = translate_format(create_info.depth_stencil_format)},
        .sample_desc = { .data = { .Count = 1, .Quality = 0 } },
        .node_mask = { .data = { 0 } },
        .cached_pso = { .data = {} },
        .flags = { .data = { D3D12_PIPELINE_STATE_FLAG_NONE } },
    };
    for (auto i = 0; i < PIPELINE_COLOR_ATTACHMENTS_MAX; ++i)
    {
        graphics_pipeline_stream.render_target_formats.data.RTFormats[i] = translate_format(
            create_info.color_attachment_formats[i]);
    }
    D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {
        .SizeInBytes = sizeof(graphics_pipeline_stream),
        .pPipelineStateSubobjectStream = &graphics_pipeline_stream
    };
    ID3D12PipelineState* pso = nullptr;
    auto result = result_from_hresult(m_context.device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pso)));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    auto pipeline = m_pipelines.acquire();
    pipeline->type = Pipeline_Type::Vertex_Shading;
    pipeline->vertex_shading_info = create_info;
    pipeline->pso = pso;
    pipeline->rtpso = nullptr;
    return pipeline;
}

std::expected<Pipeline*, Result> D3D12_Graphics_Device::create_pipeline(
    const Compute_Pipeline_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC compute_pipeline_desc = {
        .pRootSignature = m_context.bindless_root_signature,
        .CS = translate_shader_blob_safe(create_info.cs),
        .NodeMask = 0,
        .CachedPSO = {},
        .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };
    ID3D12PipelineState* pso;
    auto result = result_from_hresult(m_context.device->CreateComputePipelineState(
        &compute_pipeline_desc, IID_PPV_ARGS(&pso)));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    auto pipeline = m_pipelines.acquire();
    pipeline->type = Pipeline_Type::Compute;
    pipeline->compute_shading_info = create_info;
    pipeline->pso = pso;
    pipeline->rtpso = nullptr;
    return pipeline;
}

std::expected<Pipeline*, Result> D3D12_Graphics_Device::create_pipeline(
    const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    core::d3d12::Mesh_Shader_Pipeline_Desc mesh_pipeline_stream = {
        .root_signature = {.data = m_context.bindless_root_signature },
        .as = {.data = translate_shader_blob_safe(create_info.ts) },
        .ms = {.data = translate_shader_blob_safe(create_info.ms) },
        .blend_state = {.data = translate_blend_state_desc(create_info.blend_state_info)},
        .sample_mask = {.data = ~0u },
        .rasterizer_state = {.data = translate_rasterizer_desc(create_info.rasterizer_state_info)},
        .depth_stencil_state = {.data = translate_depth_stencil_desc(create_info.depth_stencil_info)},
        .input_layout_desc = {.data = {} },
        .primitive_topology_type = {.data = translate_primitive_topology_type(create_info.primitive_topology) },
        .render_target_formats = {.data = {.NumRenderTargets = create_info.color_attachment_count } },
        .depth_stencil_format = {.data = translate_format(create_info.depth_stencil_format)},
        .sample_desc = {.data = {.Count = 1, .Quality = 0 } },
        .node_mask = {.data = { 0 } },
        .cached_pso = {.data = {} },
        .flags = {.data = { D3D12_PIPELINE_STATE_FLAG_NONE } },
    };
    for (auto i = 0; i < PIPELINE_COLOR_ATTACHMENTS_MAX; ++i)
    {
        mesh_pipeline_stream.render_target_formats.data.RTFormats[i] = translate_format(
            create_info.color_attachment_formats[i]);
    }
    D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {
        .SizeInBytes = sizeof(mesh_pipeline_stream),
        .pPipelineStateSubobjectStream = &mesh_pipeline_stream
    };
    ID3D12PipelineState* pso = nullptr;
    auto result = result_from_hresult(m_context.device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pso)));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    auto pipeline = m_pipelines.acquire();
    pipeline->type = Pipeline_Type::Mesh_Shading;
    pipeline->mesh_shading_info = create_info;
    pipeline->pso = pso;
    pipeline->rtpso = nullptr;
    return pipeline;
}

void D3D12_Graphics_Device::destroy_pipeline(Pipeline* pipeline) noexcept
{
    if (pipeline == nullptr) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto d3d12_pipeline = static_cast<D3D12_Pipeline*>(pipeline);
    if (d3d12_pipeline->pso)
        d3d12_pipeline->pso->Release();
    if (d3d12_pipeline->rtpso)
        d3d12_pipeline->rtpso->Release();

    m_pipelines.release(d3d12_pipeline);
}

Result D3D12_Graphics_Device::submit(const Submit_Info& submit_info) noexcept
{
    std::mutex* queue_mutex = nullptr;
    ID3D12CommandQueue* command_queue = nullptr;
    switch (submit_info.queue_type)
    {
    case Queue_Type::Graphics:
        command_queue = m_context.direct_queue;
        queue_mutex = &m_direct_queue_mutex;
        break;
    case Queue_Type::Compute:
        command_queue = m_context.compute_queue;
        queue_mutex = &m_compute_queue_mutex;
        break;
    case Queue_Type::Copy:
        command_queue = m_context.copy_queue;
        queue_mutex = &m_copy_queue_mutex;
        break;
    case Queue_Type::Video_Decode:
        [[fallthrough]]; // TODO: implement video queues
    case Queue_Type::Video_Encode:
        [[fallthrough]];
    default:
        return Result::Error_Invalid_Parameters;
    }
    std::unique_lock<std::mutex> lock_guard(*queue_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    for (auto& wait_info : submit_info.wait_infos)
    {
        auto result = result_from_hresult(command_queue->Wait(
            static_cast<ID3D12Fence1*>(
                static_cast<D3D12_Fence*>(wait_info.fence)->fence),
            wait_info.value));
        if (result != Result::Success)
        {
            return result;
        }
    }
    uint32_t command_list_count = uint32_t(submit_info.command_lists.size());
    std::vector<ID3D12CommandList*> command_lists;
    command_lists.reserve(command_list_count);
    for (auto command_list : submit_info.command_lists)
    {
        auto d3d12_command_list = static_cast<D3D12_Command_List*>(command_list);
        command_lists.push_back(d3d12_command_list->get_internal_command_list());
    }
    command_queue->ExecuteCommandLists(command_list_count, command_lists.data());
    for (auto& signal_info : submit_info.signal_infos)
    {
        auto result = result_from_hresult(command_queue->Signal(
            static_cast<ID3D12Fence1*>(
                static_cast<D3D12_Fence*>(signal_info.fence)->fence),
            signal_info.value));
        if (result != Result::Success)
        {
            return result;
        }
    }
    return Result::Success;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12_Graphics_Device::get_cpu_descriptor_handle(
    uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
    uint64_t increment = 0;
    switch (type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        handle = m_context.resource_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.resource;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        handle = m_context.sampler_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.sampler;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        handle = m_context.rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.rtv;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        handle = m_context.dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.dsv;
        break;
    default:
        break;
    }

    handle.ptr += increment * index;

    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12_Graphics_Device::get_gpu_descriptor_handle(
    uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = {};
    uint64_t increment = 0;
    switch (type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        handle = m_context.resource_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.resource;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        handle = m_context.sampler_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.sampler;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        handle = m_context.rtv_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.rtv;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        handle = m_context.dsv_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        increment = m_descriptor_increment_sizes.dsv;
        break;
    default:
        break;
    }

    handle.ptr += increment * index;

    return handle;
}

uint32_t D3D12_Graphics_Device::get_uav_from_bindless_index(uint32_t bindless_index) const noexcept
{
    return bindless_index + 1;
}

const Indirect_Signatures& D3D12_Graphics_Device::get_indirect_signatures() const noexcept
{
    return m_indirect_signatures;
}

void D3D12_Graphics_Device::create_initial_buffer_descriptors(D3D12_Buffer* buffer) noexcept
{
    auto srv_desc = core::d3d12::make_raw_buffer_srv(buffer->size);
    auto uav_desc = core::d3d12::make_raw_buffer_uav(buffer->size);
    create_srv_and_uav(buffer->resource, buffer->buffer_view->bindless_index, &srv_desc, &uav_desc);
}

void D3D12_Graphics_Device::create_initial_image_descriptors(D3D12_Image* image) noexcept
{
    auto format = translate_format(image->format);
    uint32_t depth_or_array_size = image->depth > image->array_size ? image->depth : image->array_size;
    auto srv_desc = core::d3d12::make_full_texture_srv(
        format,
        translate_view_type_srv(image->primary_view_type),
        depth_or_array_size);
    auto uav_desc = core::d3d12::make_full_texture_uav(
        format,
        translate_view_type_uav(image->primary_view_type),
        depth_or_array_size,
        0, 0);
    auto rtv_desc = core::d3d12::make_full_texture_rtv(
        format,
        translate_view_type_rtv(image->primary_view_type),
        depth_or_array_size,
        0, 0);
    auto dsv_desc = core::d3d12::make_full_texture_dsv(
        format,
        translate_view_type_dsv(image->primary_view_type),
        depth_or_array_size,
        0);

    bool image_has_srv = bool(image->usage & Image_Usage::Sampled);
    bool image_has_uav = bool(image->usage & Image_Usage::Unordered_Access);
    bool image_has_rtv = bool(image->usage & Image_Usage::Color_Attachment);
    bool image_has_dsv = bool(image->usage & Image_Usage::Depth_Stencil_Attachment);

    create_srv_uav_rtv_dsv(image->resource, image->image_view->bindless_index,
        image_has_srv ? &srv_desc : nullptr,
        image_has_uav ? &uav_desc : nullptr,
        static_cast<D3D12_Image_View*>(image->image_view)->rtv_dsv_index,
        image_has_rtv ? &rtv_desc : nullptr,
        image_has_dsv ? &dsv_desc : nullptr);
}

void D3D12_Graphics_Device::create_srv_and_uav(
    ID3D12Resource* resource,
    uint32_t bindless_index,
    const D3D12_SHADER_RESOURCE_VIEW_DESC* srv_desc,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav_desc) noexcept
{
    if (srv_desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = m_context.resource_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        cpu_handle.ptr += (bindless_index) * m_descriptor_increment_sizes.resource;
        m_context.device->CreateShaderResourceView(resource, srv_desc, cpu_handle);
    }

    if (uav_desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = m_context.resource_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        cpu_handle.ptr += (bindless_index + 1) * m_descriptor_increment_sizes.resource;
        m_context.device->CreateUnorderedAccessView(resource, nullptr, uav_desc, cpu_handle);
    }
}

void D3D12_Graphics_Device::create_srv_uav_rtv_dsv(
    ID3D12Resource* resource,
    uint32_t bindless_index,
    const D3D12_SHADER_RESOURCE_VIEW_DESC* srv_desc,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav_desc,
    uint32_t rtv_dsv_index,
    const D3D12_RENDER_TARGET_VIEW_DESC* rtv_desc,
    const D3D12_DEPTH_STENCIL_VIEW_DESC* dsv_desc) noexcept
{
    create_srv_and_uav(resource, bindless_index, srv_desc, uav_desc);

    if (rtv_desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = m_context.rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        cpu_handle.ptr += (rtv_dsv_index) * m_descriptor_increment_sizes.rtv;
        m_context.device->CreateRenderTargetView(resource, rtv_desc, cpu_handle);
    }

    if (dsv_desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = m_context.dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        cpu_handle.ptr += (rtv_dsv_index)*m_descriptor_increment_sizes.dsv;
        m_context.device->CreateDepthStencilView(resource, dsv_desc, cpu_handle);
    }
}

Descriptor_Increment_Sizes D3D12_Graphics_Device::acquire_descriptor_increment_sizes() noexcept
{
    return Descriptor_Increment_Sizes {
        .resource = m_context.device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        .sampler = m_context.device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER),
        .rtv = m_context.device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
        .dsv = m_context.device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
    };
}

Indirect_Signatures D3D12_Graphics_Device::create_execute_indirect_signatures() noexcept
{
    Indirect_Signatures result = {};
    D3D12_INDIRECT_ARGUMENT_DESC argument_desc = {};
    D3D12_COMMAND_SIGNATURE_DESC command_signature_desc = {
        .ByteStride = INDIRECT_ARGUMENT_STRIDE,
        .NumArgumentDescs = 1,
        .pArgumentDescs = &argument_desc,
        .NodeMask = 0
    };

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DRAW_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        m_context.bindless_root_signature,
        IID_PPV_ARGS(&result.draw_indirect));

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DRAW_INDEXED_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        m_context.bindless_root_signature,
        IID_PPV_ARGS(&result.draw_indexed_indirect));

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DISPATCH_MESH_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        m_context.bindless_root_signature,
        IID_PPV_ARGS(&result.draw_mesh_tasks_indirect));

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DISPATCH_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        m_context.bindless_root_signature,
        IID_PPV_ARGS(&result.dispatch_indirect));

    return result;
}
}
