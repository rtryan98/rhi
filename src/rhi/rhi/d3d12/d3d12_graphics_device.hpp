#pragma once

#include "rhi/graphics_device.hpp"

namespace rhi::d3d12
{
class D3D12_Graphics_Device final : public Graphics_Device
{
public:
    D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~D3D12_Graphics_Device() noexcept override = default;
};
}
