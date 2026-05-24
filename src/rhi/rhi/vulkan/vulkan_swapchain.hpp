#pragma once

#include "rhi/swapchain.hpp"

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
#include <VkBootstrap.h>

namespace rhi::vulkan
{
class Vulkan_Graphics_Device;

constexpr static auto MAX_SWAPCHAIN_IMAGES = 8u;

struct Vulkan_Image;

class Vulkan_Swapchain : public Swapchain
{
public:
    Vulkan_Swapchain(
        Vulkan_Graphics_Device* graphics_device,
        const Swapchain_Win32_Create_Info& create_info) noexcept;
    virtual ~Vulkan_Swapchain() noexcept;

    virtual void acquire_next_image() noexcept override;
    virtual void present() noexcept override;
    virtual void change_format(Image_Format format) noexcept override;

    virtual [[nodiscard]] Swapchain_Resize_Info query_resize() noexcept override;
    virtual [[nodiscard]] Image_Format get_image_format() noexcept override;
    virtual [[nodiscard]] Image_View* get_current_image_view() noexcept override;
    virtual [[nodiscard]] uint32_t get_width() const noexcept override;
    virtual [[nodiscard]] uint32_t get_height() const noexcept override;

    VkSemaphore get_current_acquire_semaphore() const noexcept;
    VkSemaphore get_current_present_semaphore() const noexcept;

private:
    void recreate_resources() noexcept;
    Swapchain_Resize_Info query_resize_internal(Image_Format format) noexcept;

    std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> create_semaphores();

private:
    Vulkan_Graphics_Device* m_device;
    VkSurfaceKHR m_surface;
    vkb::Swapchain m_swapchain;
    void* m_hwnd;
    std::array<Vulkan_Image*, MAX_SWAPCHAIN_IMAGES> m_images;
    std::vector<VkImageView> m_image_views;
    uint32_t m_current_image_index;

    uint64_t m_current_acquire_semaphore;
    std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> m_acquire_semaphores;
    std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> m_present_semaphores;
};
}
