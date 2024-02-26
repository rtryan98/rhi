#ifdef RHI_GRAPHICS_API_D3D12
#include "d3d12_graphics_device.hpp"

namespace rhi::d3d12
{
D3D12_Graphics_Device::D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept
    : Graphics_Device()
    , m_context{}
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
}

#endif // RHI_GRAPHICS_API_D3D12
