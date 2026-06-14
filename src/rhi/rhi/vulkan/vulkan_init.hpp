#pragma once

#include <volk.h>

namespace rhi::vulkan
{
struct Queue_Infos
{
    uint32_t graphics_queue_index;
    uint32_t compute_queue_index;
    uint32_t copy_queue_index;
    uint32_t video_decode_queue_index;
    uint32_t video_encode_queue_index;
};

VkInstance create_instance(bool enable_validation_layers, bool enable_gpu_validation);
VkPhysicalDevice select_physical_device(VkInstance instance);
Queue_Infos get_queue_infos(VkPhysicalDevice physical_device);
VkDevice create_device(VkPhysicalDevice physical_device);
VkDescriptorSetLayout create_descriptor_set_layout(VkDevice device);
VkPipelineLayout create_pipeline_layout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout, uint32_t push_constant_size);
VkDescriptorPool create_descriptor_pool(VkDevice device);
VkDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorSetLayout descriptor_set_layout, VkDescriptorPool descriptor_pool);

VkPhysicalDeviceRayTracingPipelinePropertiesKHR query_ray_tracing_pipeline_properties(VkPhysicalDevice physical_device);
}
