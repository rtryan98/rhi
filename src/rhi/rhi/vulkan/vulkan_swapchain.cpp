#include "rhi/vulkan/vulkan_swapchain.hpp"
#include "rhi/vulkan/vulkan_graphics_device.hpp"

#include "rhi/common/win32_forward.hpp"
#include "rhi/vulkan/vulkan_cast.hpp"
#include "rhi/vulkan/vulkan_format.hpp"

namespace rhi::vulkan
{
VkSurfaceFormatKHR find_swapchain_format(Image_Format image_format)
{
    switch (image_format)
    {
    case Image_Format::R8G8B8A8_UNORM:
        return {
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
    case Image_Format::R8G8B8A8_SRGB:
        return {
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
    case Image_Format::B8G8R8A8_UNORM:
        return {
            .format = VK_FORMAT_B8G8R8A8_UNORM,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
    case Image_Format::B8G8R8A8_SRGB:
        return {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
    case Image_Format::A2R10G10B10_UNORM_PACK32:
        return {
            .format = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
            .colorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT
        };
    case Image_Format::R16G16B16A16_SFLOAT:
        return {
            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
            .colorSpace = VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT
        };
    default:
        assert(false && "Invalid format");
        std::unreachable();
    }
}

VkSwapchainKHR create_swapchain(
    VkDevice device,
    VkSurfaceKHR surface,
    uint32_t image_count,
    const VkSurfaceFormatKHR& surface_format,
    const VkExtent2D& extent,
    VkSwapchainKHR old_swapchain)
{
    VkSwapchainPresentScalingCreateInfoKHR swapchain_present_scaling_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_SCALING_CREATE_INFO_KHR,
        .pNext = nullptr,
        .scalingBehavior = VK_PRESENT_SCALING_STRETCH_BIT_KHR,
        .presentGravityX = 0,
        .presentGravityY = 0
    };
    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = &swapchain_present_scaling_create_info,
        .flags = 0,
        .surface = surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1u,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
        .clipped = VK_FALSE,
        .oldSwapchain = old_swapchain
    };
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain);
    return swapchain;
}

Vulkan_Swapchain::Vulkan_Swapchain(Vulkan_Graphics_Device* graphics_device, const Swapchain_Win32_Create_Info& create_info) noexcept
    : m_device(graphics_device)
    , m_surface(VK_NULL_HANDLE)
    , m_swapchain()
    , m_hwnd(create_info.hwnd)
    , m_image_count(create_info.image_count)
    , m_extent()
    , m_format(vulkan_cast<VkFormat>(create_info.preferred_format))
    , m_images()
    , m_image_views()
    , m_current_image_index(0u)
    , m_current_acquire_semaphore_index(0ull)
    , m_current_acquire_semaphore(VK_NULL_HANDLE)
    , m_acquire_semaphores(create_semaphores())
    , m_present_semaphores(create_semaphores())
{
    VkWin32SurfaceCreateInfoKHR surface_create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = GetModuleHandle(NULL),
        .hwnd = reinterpret_cast<HWND>(create_info.hwnd)
    };
    vkCreateWin32SurfaceKHR(*m_device, &surface_create_info, nullptr, &m_surface);

    auto surface_format = find_swapchain_format(create_info.preferred_format);

    RECT rect;
    bool client_rect_result = GetClientRect(reinterpret_cast<HWND>(m_hwnd), &rect);
    uint32_t client_width = rect.right - rect.left;
    uint32_t client_height = rect.bottom - rect.top;
    m_extent = {
        .width = client_width,
        .height = client_height
    };

    m_swapchain = create_swapchain(
        *m_device,
        m_surface,
        m_image_count,
        surface_format,
        { client_width, client_height },
        VK_NULL_HANDLE);

    recreate_resources();
}

Vulkan_Swapchain::~Vulkan_Swapchain() noexcept
{
    for (auto semaphore : m_acquire_semaphores)
    {
        vkDestroySemaphore(*m_device, semaphore, nullptr);
    }
    for (auto semaphore : m_present_semaphores)
    {
        vkDestroySemaphore(*m_device, semaphore, nullptr);
    }
    if (m_swapchain != VK_NULL_HANDLE)
    {
        for (auto image_view : m_image_views)
        {
            vkDestroyImageView(*m_device, image_view, nullptr);
        }
    }
    vkDestroySwapchainKHR(*m_device, m_swapchain, nullptr);
    vkDestroySurfaceKHR(*m_device, m_surface, nullptr);
}

void Vulkan_Swapchain::acquire_next_image() noexcept
{
    m_current_acquire_semaphore = m_acquire_semaphores[(m_current_acquire_semaphore_index++) % m_acquire_semaphores.size()];
    vkAcquireNextImageKHR(
        *m_device,
        m_swapchain,
        0ull,
        m_current_acquire_semaphore,
        VK_NULL_HANDLE,
        &m_current_image_index);
}

void Vulkan_Swapchain::present() noexcept
{
    VkSwapchainKHR swapchain = m_swapchain;
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &m_present_semaphores[m_current_image_index],
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &m_current_image_index,
        .pResults = nullptr
    };
    vkQueuePresentKHR(m_device->get_queue(Queue_Type::Graphics), &present_info);
}

void Vulkan_Swapchain::change_format(Image_Format format) noexcept
{
    query_resize_internal(format);
}

Swapchain_Resize_Info Vulkan_Swapchain::query_resize() noexcept
{
    return query_resize_internal(Image_Format::Undefined);
}

Image_Format Vulkan_Swapchain::get_image_format() noexcept
{
    return translate_vkformat_to_image_format(m_format);
}

Image_View* Vulkan_Swapchain::get_current_image_view() noexcept
{
    return m_images.at(m_current_image_index)->image_view;
}

uint32_t Vulkan_Swapchain::get_width() const noexcept
{
    return m_extent.width;
}

uint32_t Vulkan_Swapchain::get_height() const noexcept
{
    return m_extent.height;
}

VkSemaphore Vulkan_Swapchain::get_current_acquire_semaphore() const noexcept
{
    return m_current_acquire_semaphore;
}

VkSemaphore Vulkan_Swapchain::get_current_present_semaphore() const noexcept
{
    return m_present_semaphores.at(m_current_image_index);
}

void Vulkan_Swapchain::recreate_resources() noexcept
{
    if (m_images[0] == nullptr) // No images have been created yet.
    {
        for (auto i = 0; i < m_image_count; ++i)
        {
            m_images[i] = m_device->create_proxy_image();
        }
    }
    if (m_extent.width > 0 && m_extent.height > 0)
    {
        std::array<VkImage, MAX_SWAPCHAIN_IMAGES> images = {};
        vkGetSwapchainImagesKHR(*m_device, m_swapchain, &m_image_count, images.data());

        for (auto i = 0; i < m_image_count; ++i)
        {
            VkImageViewCreateInfo image_view_create_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = m_format,
                .components = {}, // identity
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = VK_REMAINING_MIP_LEVELS,
                    .baseArrayLayer = 0,
                    .layerCount = VK_REMAINING_ARRAY_LAYERS
                }
            };
            vkCreateImageView(*m_device, &image_view_create_info, nullptr, &m_image_views[i]);

            auto* image = m_images[i];
            image->image = images[i];
            image->allocation = nullptr;
            image->image_view_linked_list_head = nullptr;
            image->format = translate_vkformat_to_image_format(m_format);
            image->width = m_extent.width;
            image->height = m_extent.height;
            image->depth = 1;
            image->array_size = 1;
            image->mip_levels = 1;
            image->usage = Image_Usage::Color_Attachment;
            image->primary_view_type = Image_View_Type::Texture_2D;
            image->image_view->bindless_index = ~0u;
            image->image_view->image = image;
            image->image_view->descriptor_type = Descriptor_Type::Color_Attachment;
            static_cast<Vulkan_Image_View*>(image->image_view)->image_view = m_image_views[i];
            image->image_view->next_image_view = nullptr;
        }
    }
}

Swapchain_Resize_Info Vulkan_Swapchain::query_resize_internal(Image_Format format) noexcept
{
    Swapchain_Resize_Info result = {
        .is_size_changed = false,
        .width = 0,
        .height = 0
    };

    RECT rect;
    bool client_rect_result = GetClientRect(reinterpret_cast<HWND>(m_hwnd), &rect);
    uint32_t client_width = rect.right - rect.left;
    uint32_t client_height = rect.bottom - rect.top;

    bool has_area = false;
    has_area = (client_width > 0) && (client_height > 0);

    bool resize = false;
    resize = ((client_width != m_extent.width) || (client_height != m_extent.height)) && has_area;

    if ((resize && client_rect_result) || format != Image_Format::Undefined)
    {
        result.is_size_changed = true;
        result.width = client_width;
        result.height = client_height;

        m_device->wait_idle();

        for (auto image_view : m_image_views)
        {
            vkDestroyImageView(*m_device, image_view, nullptr);
        }

        VkSurfaceFormatKHR surface_format = find_swapchain_format(translate_vkformat_to_image_format(m_format));

        // Check if format changed
        if (format != Image_Format::Undefined &&
            vulkan_cast<VkFormat>(format) != m_format)
        {
            surface_format = find_swapchain_format(format);
            m_format = surface_format.format;
        }

        m_extent = {
            .width = client_width,
            .height = client_height
        };

        auto new_swapchain = create_swapchain(
            *m_device,
            m_surface,
            m_image_count,
            surface_format,
            m_extent,
            m_swapchain);
        vkDestroySwapchainKHR(*m_device, m_swapchain, nullptr);
        m_swapchain = new_swapchain;

        recreate_resources();
    }

    return result;
}

std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> Vulkan_Swapchain::create_semaphores()
{
    std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> semaphores;
    VkSemaphoreCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };
    for (auto& semaphore : semaphores)
    {
        vkCreateSemaphore(*m_device, &create_info, nullptr, &semaphore);
    }
    return semaphores;
}
}
