#include "rhi/graphics_device.hpp"

#if defined(RHI_GRAPHICS_API_D3D12)
    #include "rhi/d3d12/d3d12_graphics_device.hpp"
#elif defined(RHI_GRAPHICS_API_VULKAN)
    // TODO: implement
#else
    #error No graphics api is set.
#endif

namespace rhi
{
std::unique_ptr<Graphics_Device> Graphics_Device::create(const Graphics_Device_Create_Info& create_info) noexcept
{
#if defined(RHI_GRAPHICS_API_D3D12)
    return std::make_unique<d3d12::D3D12_Graphics_Device>(create_info);
#elif defined(RHI_GRAPHICS_API_VULKAN)
    // TODO: implement
#else
    #error No graphics api is set.
#endif
}
}
