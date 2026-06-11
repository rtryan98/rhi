#pragma once

#include "rhi/resource.hpp"

#include <volk.h>
#include <vk_mem_alloc.h>

namespace rhi::vulkan
{
struct Vulkan_Buffer : public Buffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct Vulkan_Buffer_View : public Buffer_View
{
    VkBufferView buffer_view;
};

struct Vulkan_Image : public Image
{
    VkImage image;
    VmaAllocation allocation;
};

struct Vulkan_Image_View : public Image_View
{
    VkImageView image_view;
    Vulkan_Image_View* image_view_linked_list_head;
};

struct Vulkan_Sampler : public Sampler
{
    VkSampler sampler;
};

struct Vulkan_Pipeline : public Pipeline
{
    VkPipeline pipeline;
};

struct Vulkan_Acceleration_Structure : public Acceleration_Structure
{
    VkAccelerationStructureKHR acceleration_structure;
};

inline VkFlags get_aspect_mask(Image* image)
{
    auto image_format_info = get_image_format_info(image->format);
    VkFlags aspect_mask = VK_IMAGE_ASPECT_NONE;

    if (!(image_format_info.is_depth || image_format_info.is_stencil))
    {
        aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else
    {
        if (image_format_info.is_depth)
        {
            aspect_mask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (image_format_info.is_stencil)
        {
            aspect_mask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }

    return aspect_mask;
}
}
