#include "rhi/d3d12/d3d12_graphics_device.hpp"
#include "rhi/d3d12/d3d12_command_list.hpp"
#include "rhi/d3d12/d3d12_swapchain.hpp"

#include <core/d3d12/d3d12_pso.hpp>
#include <core/d3d12/d3d12_descriptor_util.hpp>
#include <D3D12MemAlloc.h>

extern "C" __declspec(dllexport) extern const uint32_t D3D12SDKVersion = 613;
extern "C" __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";

namespace rhi::d3d12
{
constexpr static uint32_t MAX_RTV_DSV_DESCRIPTORS = 1024;

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

Result wait_result_from_dword(DWORD dword) noexcept
{
    switch (dword)
    {
    case WAIT_OBJECT_0:
        return Result::Success;
    case WAIT_TIMEOUT:
        return Result::Wait_Timeout;
    case WAIT_FAILED:
        return Result::Error_Wait_Failed;
    default:
        return Result::Error_Unknown;
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

Result D3D12_Fence::get_status(uint64_t value) noexcept
{
    return wait_result_from_dword(core::d3d12::await_fence(fence, value, 0));
}

Result D3D12_Fence::wait_for_value(uint64_t value) noexcept
{
    return wait_result_from_dword(core::d3d12::await_fence(fence, value, INFINITE));
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
    , m_resource_descriptor_indices()
    , m_sampler_descriptor_indices()
    , m_rtv_descriptor_indices()
    , m_dsv_descriptor_indices()
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
        .push_constant_size = PUSH_CONSTANT_MAX_SIZE,
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

    m_resource_descriptor_indices.reserve(D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2 / 2);
    for (auto i = (D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2 / 2) - 1; i >= 0; --i)
    {
        m_resource_descriptor_indices.push_back(uint32_t(i * 2));
    }
    m_sampler_descriptor_indices.reserve(D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE);
    for (auto i = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE - 1; i >= 0; --i)
    {
        m_sampler_descriptor_indices.push_back(uint32_t(i));
    }
    m_rtv_descriptor_indices.reserve(MAX_RTV_DSV_DESCRIPTORS);
    for (auto i = int32_t(MAX_RTV_DSV_DESCRIPTORS) - 1; i >= 0; --i)
    {
        m_rtv_descriptor_indices.push_back(uint32_t(i));
    }
    m_dsv_descriptor_indices.reserve(MAX_RTV_DSV_DESCRIPTORS);
    for (auto i = int32_t(MAX_RTV_DSV_DESCRIPTORS) - 1; i >= 0; --i)
    {
        m_dsv_descriptor_indices.push_back(uint32_t(i));
    }
}

D3D12_Graphics_Device::~D3D12_Graphics_Device() noexcept
{
    core::d3d12::await_context(&m_context);

    // Release everything that was not released by the user
    // TODO: Should this be done or should a leak be mentioned by the validation layer instead?
    for (auto& buffer : m_buffers)
    {
        if (buffer.resource && buffer.allocation)
        {
            buffer.resource->Release();
            buffer.allocation->Release();
        }
    }
    for (auto& image : m_images)
    {
        if (image.resource && image.allocation)
        {
            image.resource->Release();
            image.allocation->Release();
        }
    }
    for (auto& pipeline : m_pipelines)
    {
        if (pipeline.type == Pipeline_Type::Ray_Tracing)
        {
            if (pipeline.rtpso)
            {
                pipeline.rtpso->Release();
            }
        }
        else
        {
            if (pipeline.pso)
            {
                pipeline.pso->Release();
            }
        }
    }

    m_indirect_signatures.draw_indirect->Release();
    m_indirect_signatures.draw_indexed_indirect->Release();
    m_indirect_signatures.draw_mesh_tasks_indirect->Release();
    m_indirect_signatures.dispatch_indirect->Release();
    m_allocator->Release();
    core::d3d12::destroy_d3d12_context(&m_context);
}

Result D3D12_Graphics_Device::wait_idle() noexcept
{
    return wait_result_from_dword(core::d3d12::await_context(&m_context));
}

Result D3D12_Graphics_Device::queue_wait_idle(Queue_Type queue, uint64_t timeout) noexcept
{
    ID3D12CommandQueue* command_queue = nullptr;
    switch (queue)
    {
    case Queue_Type::Graphics:
        command_queue = m_context.direct_queue;
        break;
    case Queue_Type::Compute:
        command_queue = m_context.compute_queue;
        break;
    case Queue_Type::Copy:
        command_queue = m_context.copy_queue;
        break;
    default: // TODO: implement video queues
        command_queue = m_context.direct_queue;
        break;
    }
    return wait_result_from_dword(core::d3d12::await_queue(m_context.device, command_queue, timeout));
}

Graphics_API D3D12_Graphics_Device::get_graphics_api() const noexcept
{
    return Graphics_API::D3D12;
}

std::unique_ptr<Swapchain> D3D12_Graphics_Device::create_swapchain(
    const Swapchain_Win32_Create_Info& create_info) noexcept
{
    return std::make_unique<D3D12_Swapchain>(this, create_info);
}

std::unique_ptr<Command_Pool> D3D12_Graphics_Device::create_command_pool(
    const Command_Pool_Create_Info& create_info) noexcept
{
    return std::make_unique<D3D12_Command_Pool>(this, create_info);
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

    bool allow_uav = create_info.heap == Memory_Heap_Type::GPU;

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
        .Flags = allow_uav
            ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            : D3D12_RESOURCE_FLAG_NONE,
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
    buffer->buffer_view->bindless_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
    buffer_view->bindless_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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

    release_descriptor_index(buffer->buffer_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    auto next_buffer_view = d3d12_buffer->buffer_view_linked_list_head;
    while (next_buffer_view != nullptr)
    {
        auto current_buffer_view = next_buffer_view;
        release_descriptor_index(current_buffer_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
        if (create_info.depth <= 1)
        {
            is_array_type = true;
        }
        break;
    default:
        break;
    }

    auto dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    switch (create_info.primary_view_type)
    {
    case Image_View_Type::Texture_2D:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS_Array:
        dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        break;
    case Image_View_Type::Texture_3D:
        dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        break;
    default:
        break;
    }

    bool is_rtv = false;
    bool is_dsv = false;

    D3D12_RESOURCE_DESC1 resource_desc = {
        .Dimension = dimension,
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
    image->image_view->bindless_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    image->image_view->image = image;
    static_cast<D3D12_Image_View*>(image->image_view)->rtv_dsv_index = is_rtv
        ? create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
        : is_dsv
            ? create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
            : ~0u;
    // HACK: internal descriptor type should not be visible
    image->image_view->descriptor_type = static_cast<Descriptor_Type>(0);
    static_cast<D3D12_Image_View*>(image->image_view)->next_image_view = nullptr;
    image->resource = resource;
    image->allocation = allocation;
    image->image_view_linked_list_head = nullptr;

    auto srv_desc = core::d3d12::make_full_texture_srv(
        translate_format(image->format),
        translate_view_type_srv(image->primary_view_type),
        std::max(image->depth, uint32_t(image->array_size)));
    auto uav_desc = core::d3d12::make_full_texture_uav(
        translate_format(image->format),
        translate_view_type_uav(image->primary_view_type),
        std::max(image->depth, uint32_t(image->array_size)),
        0,
        0);
    auto rtv_desc = core::d3d12::make_full_texture_rtv(
        translate_format(image->format),
        translate_view_type_rtv(image->primary_view_type),
        std::max(image->depth, uint32_t(image->array_size)),
        0,
        0);
    auto dsv_desc = core::d3d12::make_full_texture_dsv(
        translate_format(image->format),
        translate_view_type_dsv(image->primary_view_type),
        std::max(image->depth, uint32_t(image->array_size)),
        0);

    auto srv_desc_ptr = bool(image->usage & Image_Usage::Sampled) ? &srv_desc : nullptr;
    auto uav_desc_ptr = bool(image->usage & Image_Usage::Unordered_Access) ? &uav_desc : nullptr;
    auto rtv_desc_ptr = bool(image->usage & Image_Usage::Color_Attachment) ? &rtv_desc : nullptr;
    auto dsv_desc_ptr = bool(image->usage & Image_Usage::Depth_Stencil_Attachment) ? &dsv_desc : nullptr;
    create_srv_uav_rtv_dsv(
        image->resource,
        image->image_view->bindless_index,
        srv_desc_ptr, uav_desc_ptr,
        static_cast<D3D12_Image_View*>(image->image_view)->rtv_dsv_index,
        rtv_desc_ptr, dsv_desc_ptr);

    return image;
}

uint32_t translate_shader_swizzle(Image_Component_Swizzle swizzle, uint32_t identity_component)
{
    switch (swizzle)
    {
    case rhi::Image_Component_Swizzle::Identity:
        // HACK: D3D12 does not have an identity mapping
        return D3D12_SHADER_COMPONENT_MAPPING(identity_component % 4);
    case rhi::Image_Component_Swizzle::Zero:
        return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0;
    case rhi::Image_Component_Swizzle::One:
        return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1;
    case rhi::Image_Component_Swizzle::R:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0;
    case rhi::Image_Component_Swizzle::G:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1;
    case rhi::Image_Component_Swizzle::B:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2;
    case rhi::Image_Component_Swizzle::A:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3;
    default:
        return D3D12_SHADER_COMPONENT_MAPPING(identity_component % 4);
    }
}

uint32_t translate_shader_4_component_mapping(Image_Component_Mapping mapping)
{
    uint32_t r = translate_shader_swizzle(mapping.r, 0);
    uint32_t g = translate_shader_swizzle(mapping.g, 1);
    uint32_t b = translate_shader_swizzle(mapping.b, 2);
    uint32_t a = translate_shader_swizzle(mapping.a, 3);
    return D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(r,g,b,a);
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
    image_view->image = d3d12_image;

    switch (create_info.descriptor_type)
    {
    case Descriptor_Type::Resource:
        image_view->bindless_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        auto srv_desc = core::d3d12::make_texture_srv(
            translate_format(d3d12_image->format),
            translate_view_type_srv(create_info.view_type),
            create_info.first_array_level,
            create_info.array_levels,
            create_info.first_mip_level,
            create_info.mip_levels,
            translate_shader_4_component_mapping(create_info.component_mapping));
        m_context.device->CreateShaderResourceView(
            d3d12_image->resource,
            &srv_desc,
            get_cpu_descriptor_handle(image_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        if (bool(d3d12_image->usage & Image_Usage::Unordered_Access))
        {
            auto uav_desc = core::d3d12::make_full_texture_uav(
                translate_format(d3d12_image->format),
                translate_view_type_uav(create_info.view_type),
                create_info.first_array_level,
                create_info.first_mip_level,
                0);
            m_context.device->CreateUnorderedAccessView(
                d3d12_image->resource,
                nullptr,
                &uav_desc,
                get_cpu_descriptor_handle(
                    get_uav_from_bindless_index(
                        image_view->bindless_index),
                        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        }
        break;
    case Descriptor_Type::Color_Attachment:
        image_view->rtv_dsv_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtv_desc = core::d3d12::make_full_texture_rtv(
            translate_format(d3d12_image->format),
            translate_view_type_rtv(create_info.view_type),
            create_info.array_levels,
            create_info.first_mip_level,
            0);
        m_context.device->CreateRenderTargetView(
            d3d12_image->resource,
            &rtv_desc,
            get_cpu_descriptor_handle(image_view->rtv_dsv_index, D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        break;
    case Descriptor_Type::Depth_Stencil_Attachment:
        image_view->rtv_dsv_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        auto dsv_desc = core::d3d12::make_full_texture_dsv(
            translate_format(d3d12_image->format),
            translate_view_type_dsv(create_info.view_type),
            create_info.array_levels,
            create_info.first_mip_level);
        m_context.device->CreateDepthStencilView(
            d3d12_image->resource,
            &dsv_desc,
            get_cpu_descriptor_handle(image_view->rtv_dsv_index, D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
        break;
    default:
        break;
    }

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

    release_descriptor_index(d3d12_image->image_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    if (bool(image->usage & Image_Usage::Color_Attachment))
    {
        release_descriptor_index(static_cast<D3D12_Image_View*>(d3d12_image->image_view)->rtv_dsv_index,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    else if (bool(image->usage & Image_Usage::Depth_Stencil_Attachment))
    {
        release_descriptor_index(static_cast<D3D12_Image_View*>(d3d12_image->image_view)->rtv_dsv_index,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    auto next_image_view = d3d12_image->image_view_linked_list_head;
    while (next_image_view != nullptr)
    {
        auto current_image_view = next_image_view;
        switch (current_image_view->descriptor_type)
        {
        case Descriptor_Type::Resource:
            release_descriptor_index(current_image_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            break;
        case Descriptor_Type::Color_Attachment:
            release_descriptor_index(current_image_view->rtv_dsv_index, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            break;
        case Descriptor_Type::Depth_Stencil_Attachment:
            release_descriptor_index(current_image_view->rtv_dsv_index, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            break;
        default:
            break;
        }

        release_descriptor_index(current_image_view->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        next_image_view = current_image_view->next_image_view;
        m_image_views.release(current_image_view);
    }

    m_images.release(d3d12_image);
}

D3D12_TEXTURE_ADDRESS_MODE translate_texture_address_mode(Image_Sample_Address_Mode address_mode) noexcept
{
    switch (address_mode)
    {
    case Image_Sample_Address_Mode::Wrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case Image_Sample_Address_Mode::Mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case Image_Sample_Address_Mode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case Image_Sample_Address_Mode::Border:
        return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    default:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}

std::expected<Sampler*, Result> D3D12_Graphics_Device::create_sampler(const Sampler_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto sampler = m_samplers.acquire();
    D3D12_SAMPLER_DESC sampler_desc = {
        .Filter = translate_filter(
            create_info.filter_min,
            create_info.filter_mag,
            create_info.filter_mip,
            create_info.reduction,
            create_info.anisotropy_enable),
        .AddressU = translate_texture_address_mode(create_info.address_mode_u),
        .AddressV = translate_texture_address_mode(create_info.address_mode_v),
        .AddressW = translate_texture_address_mode(create_info.address_mode_w),
        .MipLODBias = create_info.mip_lod_bias,
        .MaxAnisotropy = create_info.max_anisotropy,
        .ComparisonFunc = translate_comparison_func(create_info.comparison_func),
        .BorderColor = {
            create_info.border_color[0],
            create_info.border_color[1],
            create_info.border_color[2],
            create_info.border_color[3]
        },
        .MinLOD = create_info.min_lod,
        .MaxLOD = create_info.max_lod
    };
    auto descriptor_index = create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    auto dest_descriptor = get_cpu_descriptor_handle(descriptor_index, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    m_context.device->CreateSampler(&sampler_desc, dest_descriptor);
    sampler->bindless_index = descriptor_index;
    return sampler;
}

void D3D12_Graphics_Device::destroy_sampler(Sampler* sampler) noexcept
{
    if (!sampler) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto d3d12_sampler = static_cast<D3D12_Sampler*>(sampler);
    release_descriptor_index(d3d12_sampler->bindless_index, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    m_samplers.release(d3d12_sampler);
}

std::expected<Shader_Blob*, Result> D3D12_Graphics_Device::create_shader_blob(
    const Shader_Blob_Create_Info& create_info) noexcept
{
    static_assert(
        sizeof(decltype(Shader_Blob::data)::value_type) == sizeof(uint8_t),
        "Size of blob changed.");

    if (create_info.data == nullptr || create_info.data_size == 0)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto blob = m_shader_blobs.acquire();
    blob->data.resize(create_info.data_size);
    blob->groups_x = create_info.groups_x;
    blob->groups_y = create_info.groups_y;
    blob->groups_z = create_info.groups_z;
    memcpy(blob->data.data(), create_info.data, create_info.data_size);
    return blob;
}

Result D3D12_Graphics_Device::recreate_shader_blob(Shader_Blob* shader_blob, const Shader_Blob_Create_Info& create_info) noexcept
{
    if (!shader_blob) return Result::Error_Invalid_Parameters;

    shader_blob->data.clear();
    shader_blob->data.reserve(create_info.data_size);
    shader_blob->groups_x = create_info.groups_x;
    shader_blob->groups_y = create_info.groups_y;
    shader_blob->groups_z = create_info.groups_z;
    memcpy(shader_blob->data.data(), create_info.data, create_info.data_size);

    return Result::Success;
}

Result D3D12_Graphics_Device::recreate_shader_blob_deserialize_memory(Shader_Blob* shader_blob, void* memory) noexcept
{
    if (!shader_blob || !memory) return Result::Error_Invalid_Parameters;

    uint8_t* blob_ptr = static_cast<uint8_t*>(memory);
    memcpy(&shader_blob->groups_x, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    memcpy(&shader_blob->groups_y, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    memcpy(&shader_blob->groups_z, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    uint32_t dxil_blob_size = 0;
    memcpy(&dxil_blob_size, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    // spir-v blob size
    blob_ptr += sizeof(uint32_t);

    shader_blob->data.clear();
    shader_blob->data.reserve(dxil_blob_size);
    memcpy(shader_blob->data.data(), blob_ptr, dxil_blob_size);

    return Result::Success;
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
        auto d3d12_command_list = static_cast<D3D12_Command_List*>(command_list)->get_internal_command_list();
        d3d12_command_list->Close();
        command_lists.push_back(d3d12_command_list);
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

void D3D12_Graphics_Device::name_resource(Buffer* buffer, const char* name) noexcept
{
    if (!buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    auto str = std::string(name);
    d3d12_buffer->resource->SetName(std::wstring(str.begin(), str.end()).c_str());
}

void D3D12_Graphics_Device::name_resource(Image* image, const char* name) noexcept
{
    if (!image) return;

    auto d3d12_image = static_cast<D3D12_Image*>(image);
    auto str = std::string(name);
    d3d12_image->resource->SetName(std::wstring(str.begin(), str.end()).c_str());
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
    create_srv_and_uav(
        buffer->resource,
        buffer->buffer_view->bindless_index,
        &srv_desc,
        buffer->heap_type == Memory_Heap_Type::GPU ? &uav_desc : nullptr);
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

uint32_t D3D12_Graphics_Device::create_descriptor_index_blocking(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    return create_descriptor_index(type);
}

void D3D12_Graphics_Device::release_descriptor_index_blocking(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    release_descriptor_index(index, type);
}

D3D12_Image* D3D12_Graphics_Device::acquire_custom_allocated_image() noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto image = m_images.acquire();
    image->image_view = m_image_views.acquire();

    return image;
}

void D3D12_Graphics_Device::release_custom_allocated_image(D3D12_Image* image) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_image_views.release(static_cast<D3D12_Image_View*>(image->image_view));
    m_images.release(image);
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

uint32_t D3D12_Graphics_Device::create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
    uint32_t index = ~0u;
    switch (type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        index = m_resource_descriptor_indices.back();
        m_resource_descriptor_indices.pop_back();
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        index = m_sampler_descriptor_indices.back();
        m_sampler_descriptor_indices.pop_back();
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        index = m_rtv_descriptor_indices.back();
        m_rtv_descriptor_indices.pop_back();
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        index = m_dsv_descriptor_indices.back();
        m_dsv_descriptor_indices.pop_back();
        break;
    default:
        break;
    }
    return index;
}

void D3D12_Graphics_Device::release_descriptor_index(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
    switch (type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        m_resource_descriptor_indices.push_back(index);
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        m_sampler_descriptor_indices.push_back(index);
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        m_rtv_descriptor_indices.push_back(index);
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        m_dsv_descriptor_indices.push_back(index);
        break;
    default:
        break;
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
        nullptr,
        IID_PPV_ARGS(&result.draw_indirect));

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DRAW_INDEXED_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        nullptr,
        IID_PPV_ARGS(&result.draw_indexed_indirect));

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DISPATCH_MESH_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        nullptr,
        IID_PPV_ARGS(&result.draw_mesh_tasks_indirect));

    static_assert(INDIRECT_ARGUMENT_STRIDE >= sizeof(D3D12_DISPATCH_ARGUMENTS) + 1);
    argument_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
    m_context.device->CreateCommandSignature(
        &command_signature_desc,
        nullptr,
        IID_PPV_ARGS(&result.dispatch_indirect));

    return result;
}

core::d3d12::D3D12_Context* D3D12_Graphics_Device::get_context() noexcept
{
    return &m_context;
}
}
