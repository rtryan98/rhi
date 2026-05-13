#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include "rhi/resource.hpp"

#include <VkBootstrap.h>
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

// No extra members required with VK_EXT_descriptor_heap
using Vulkan_Sampler = Sampler;

struct Vulkan_Pipeline : public Pipeline
{
    VkPipeline pipeline;
};

struct Vulkan_Acceleration_Structure : public Acceleration_Structure
{
    VkAccelerationStructureKHR acceleration_structure;
};
}
