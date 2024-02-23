#pragma once

#include <memory>

namespace rhi
{
enum class Graphics_API
{
    D3D12,
    Vulkan
};

struct Graphics_Device_Create_Info
{
    bool enable_validation;
};

class Graphics_Device
{
public:
    static [[nodiscard]] std::unique_ptr<Graphics_Device> create(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~Graphics_Device() noexcept = default;
    Graphics_Device(const Graphics_Device& other) = delete;
    Graphics_Device(Graphics_Device&& other) = delete;
    Graphics_Device& operator=(const Graphics_Device& other) = delete;
    Graphics_Device& operator=(Graphics_Device&& other) = delete;

    constexpr static [[nodiscard]] Graphics_API get_active_graphics_api() noexcept;

protected:
    Graphics_Device() noexcept = default;

private:

};
}
