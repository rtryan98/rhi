#include "rhi/vulkan/vulkan_swapchain.hpp"
#include "rhi/vulkan/vulkan_graphics_device.hpp"

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#define STRICT
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

#include <volk.h>

#include "rhi/vulkan/vulkan_cast.hpp"
#include "rhi/vulkan/vulkan_format.hpp"

namespace rhi::vulkan
{
void find_and_set_swapchain_format(vkb::SwapchainBuilder& swapchain_builder, Image_Format image_format)
{
    if (image_format == Image_Format::R8G8B8A8_UNORM ||
        image_format == Image_Format::B8G8R8A8_UNORM)
    {
        VkSurfaceFormatKHR surface_format = {
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
        swapchain_builder.set_desired_format(surface_format);
        surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
        swapchain_builder.add_fallback_format(surface_format);
    }
    else if (image_format == Image_Format::A2R10G10B10_UNORM_PACK32)
    {
        VkSurfaceFormatKHR surface_format = {
            .format = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
            .colorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT
        };
        swapchain_builder.set_desired_format(surface_format);
    }
    else
    {
        assert(false && "Invalid format");
    }
}

Vulkan_Swapchain::Vulkan_Swapchain(Vulkan_Graphics_Device* graphics_device, const Swapchain_Win32_Create_Info& create_info) noexcept
    : m_device(graphics_device)
    , m_surface(VK_NULL_HANDLE)
    , m_swapchain()
    , m_hwnd(create_info.hwnd)
    , m_images()
    , m_image_views()
    , m_current_image_index(0u)
    , m_current_acquire_semaphore(0ull)
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
    vkCreateWin32SurfaceKHR(m_device->get_instance(), &surface_create_info, nullptr, &m_surface);

    vkb::SwapchainBuilder swapchain_builder(graphics_device->get_device(), m_surface);
    swapchain_builder.set_desired_present_mode(create_info.present_mode == Present_Mode::Immediate
        ? VK_PRESENT_MODE_IMMEDIATE_KHR
        : VK_PRESENT_MODE_MAILBOX_KHR);
    find_and_set_swapchain_format(swapchain_builder, create_info.preferred_format);
    recreate_resources();
}

Vulkan_Swapchain::~Vulkan_Swapchain() noexcept
{
    for (auto semaphore : m_acquire_semaphores)
    {
        vkDestroySemaphore(m_device->get_device(), semaphore, nullptr);
    }
    for (auto semaphore : m_present_semaphores)
    {
        vkDestroySemaphore(m_device->get_device(), semaphore, nullptr);
    }
    if (m_swapchain != VK_NULL_HANDLE)
    {
        m_swapchain.destroy_image_views(m_image_views);
    }
    vkb::destroy_swapchain(m_swapchain);
    vkDestroySurfaceKHR(m_device->get_instance(), m_surface, nullptr);
}

void Vulkan_Swapchain::acquire_next_image() noexcept
{
    auto acquire_semaphore = m_acquire_semaphores[(m_current_acquire_semaphore++) % m_acquire_semaphores.size()];
    vkAcquireNextImageKHR(
        m_device->get_device(),
        m_swapchain,
        0ull,
        acquire_semaphore,
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
    vkQueuePresentKHR(m_device->get_graphics_queue(), &present_info);
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
    return translate_vkformat_to_image_format(m_swapchain.image_format);
}

Image_View* Vulkan_Swapchain::get_current_image_view() noexcept
{
    return m_images.at(m_current_image_index)->image_view;
}

uint32_t Vulkan_Swapchain::get_width() const noexcept
{
    return m_swapchain.extent.width;
}

uint32_t Vulkan_Swapchain::get_height() const noexcept
{
    return m_swapchain.extent.height;
}

VkSemaphore Vulkan_Swapchain::get_current_acquire_semaphore() const noexcept
{
    return m_acquire_semaphores.at(m_current_acquire_semaphore);
}

VkSemaphore Vulkan_Swapchain::get_current_present_semaphore() const noexcept
{
    return m_present_semaphores.at(m_current_image_index);
}

void Vulkan_Swapchain::recreate_resources() noexcept
{
    if (m_images[0] == nullptr) // No images have been created yet.
    {
        for (auto i = 0; i < m_swapchain.image_count; ++i)
        {
            m_images[i] = m_device->create_proxy_image();
        }
    }
    if (m_swapchain.extent.width > 0 && m_swapchain.extent.height > 0)
    {
        auto images = m_swapchain.get_images().value();
        m_image_views = m_swapchain.get_image_views().value();

        for (auto i = 0; i < m_swapchain.image_count; ++i)
        {
            auto* image = m_images[i];
            image->image = images.at(i);
            image->allocation = nullptr;
            image->image_view_linked_list_head = nullptr;
            image->format = translate_vkformat_to_image_format(m_swapchain.image_format);
            image->width = m_swapchain.extent.width;
            image->height = m_swapchain.extent.height;
            image->depth = 1;
            image->array_size = 1;
            image->mip_levels = 1;
            image->usage = Image_Usage::Color_Attachment;
            image->primary_view_type = Image_View_Type::Texture_2D;
            image->image_view->bindless_index = ~0u;
            image->image_view->image = image;
            image->image_view->descriptor_type = Descriptor_Type::Color_Attachment;
            static_cast<Vulkan_Image_View*>(image->image_view)->image_view = m_image_views.at(i);
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
    resize = ((client_width != m_swapchain.extent.width) || (client_height != m_swapchain.extent.height)) && has_area;

    if ((resize && client_rect_result) || format != Image_Format::Undefined)
    {
        result.is_size_changed = true;
        result.width = client_width;
        result.height = client_height;

        m_device->wait_idle();

        m_swapchain.destroy_image_views(m_image_views);

        vkb::SwapchainBuilder swapchain_builder(m_device->get_device(), m_surface);
        swapchain_builder.set_old_swapchain(m_swapchain);

        // Check if format changed
        if (format != Image_Format::Undefined &&
            vulkan_cast<VkFormat>(format) != m_swapchain.image_format)
        {
            find_and_set_swapchain_format(swapchain_builder, format);
        }

        auto new_swapchain = swapchain_builder.build();

        vkb::destroy_swapchain(m_swapchain);

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
        vkCreateSemaphore(m_device->get_device(), &create_info, nullptr, &semaphore);
    }
    return semaphores;
}
}
