#pragma once

#include "rhi/resource.hpp"

namespace rhi
{
enum class Image_Format;

enum class Present_Mode
{
    Immediate,
    Vsync
};

struct Swapchain_Win32_Create_Info
{
    void* hwnd; // unsafe!
    Image_Format preferred_format;
    uint32_t image_count;
    Present_Mode present_mode;
};

struct Swapchain_Resize_Info
{
    bool is_size_changed;
    uint32_t width;
    uint32_t height;
};

class Swapchain
{
public:
    virtual ~Swapchain() noexcept = default;

    virtual void acquire_next_image() noexcept = 0;
    virtual void present() noexcept = 0;

    virtual [[nodiscard]] Swapchain_Resize_Info query_resize() noexcept = 0;
    virtual [[nodiscard]] Image_Format get_image_format() noexcept = 0;
    virtual [[nodiscard]] Image_View* get_current_image_view() noexcept = 0;

protected:
    Swapchain() = default;
};
}
