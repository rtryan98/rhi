#ifdef RHI_GRAPHICS_API_D3D12
#include "d3d12_graphics_device.hpp"

namespace rhi::d3d12
{
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
    m_shader_blobs.release(shader_blob);
}
}

#endif // RHI_GRAPHICS_API_D3D12
