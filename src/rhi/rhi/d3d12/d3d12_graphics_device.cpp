#ifdef RHI_GRAPHICS_API_D3D12
#include "d3d12_graphics_device.hpp"

#include <core/d3d12/d3d12_pso.hpp>

namespace rhi::d3d12
{
Result result_from_hresult(HRESULT hresult)
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

D3D12_Graphics_Device::D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept
    : Graphics_Device()
    , m_context{}
    , m_buffers()
    , m_images()
    , m_samplers()
    , m_shader_blobs()
    , m_pipelines()
{
    core::d3d12::D3D12_Context_Create_Info context_create_info = {

    };
    core::d3d12::create_d3d12_context(context_create_info, &m_context);
}

D3D12_Graphics_Device::~D3D12_Graphics_Device() noexcept
{
    core::d3d12::await_context(&m_context);
    core::d3d12::destroy_d3d12_context(&m_context);
}

Result D3D12_Graphics_Device::submit(const Submit_Info& submit_info) noexcept
{
    return Result::Success;
}

std::expected<Shader_Blob*, Result> D3D12_Graphics_Device::create_shader_blob(void* data, uint64_t size) noexcept
{
    static_assert(sizeof(decltype(Shader_Blob::data)::value_type) == sizeof(uint8_t), "Size of blob changed.");
    if (data == nullptr)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }
    auto blob = m_shader_blobs.acquire();
    blob->data.reserve(size);
    memcpy(blob->data.data(), data, size);
    return blob;
}

void D3D12_Graphics_Device::destroy_shader_blob(Shader_Blob* shader_blob) noexcept
{
    if (shader_blob == nullptr) return;

    m_shader_blobs.release(shader_blob);
}

std::expected<Pipeline*, Result> D3D12_Graphics_Device::create_pipeline(const Graphics_Pipeline_Create_Info& create_info) noexcept
{
    core::d3d12::Graphics_Pipeline_Desc graphics_pipeline_stream = {
        .root_signature = { .data = m_context.bindless_root_signature },
        .vs = { .data = {} },
        .hs = { .data = {} },
        .ds = { .data = {} },
        .gs = { .data = {} },
        .ps = { .data = {} },
        .stream_output = { .data = {} },
        .blend_state = { .data = {} },
        .sample_mask = { .data = {} },
        .rasterizer_state = { .data = {} },
        .depth_stencil_state = { .data = {} },
        .input_layout_desc = { .data = {} },
        .primitive_topology_type = { .data = {} },
        .render_target_formats = { .data = {} },
        .depth_stencil_format = { .data = {} },
        .sample_desc = { .data = {} },
        .node_mask = { .data = { 0 } },
        .cached_pso = { .data = {} },
        .flags = { .data = { D3D12_PIPELINE_STATE_FLAG_NONE } },
    };
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
    pipeline->pso = pso;
    pipeline->rtpso = nullptr;
    return pipeline;
}

void D3D12_Graphics_Device::destroy_pipeline(Pipeline* pipeline) noexcept
{
    if (pipeline == nullptr) return;

    m_pipelines.release(static_cast<D3D12_Pipeline*>(pipeline));
}
}

#endif // RHI_GRAPHICS_API_D3D12
