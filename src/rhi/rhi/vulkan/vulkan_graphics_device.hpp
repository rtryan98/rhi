#pragma once

#include "rhi/graphics_device.hpp"
#include "rhi/common/resource_pool.hpp"
#include "rhi/vulkan/vulkan_resource.hpp"

#include <volk.h>
#include <mutex>
#include <plf_colony.h>
#include <vk_mem_alloc.h>

namespace rhi::vulkan
{
struct Vulkan_Fence : public Fence
{
    VkSemaphore semaphore;
    VkDevice device;

    virtual [[nodiscard]] Result get_status(uint64_t value) noexcept override;
    virtual Result wait_for_value(uint64_t value) noexcept override;
};

using Vulkan_Resource_Pool = Resource_Pool<
    Vulkan_Buffer,
    Vulkan_Buffer_View,
    Vulkan_Image,
    Vulkan_Image_View,
    Vulkan_Sampler,
    Vulkan_Acceleration_Structure>;

class Vulkan_Graphics_Device final : public Graphics_Device
{
public:
    Vulkan_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~Vulkan_Graphics_Device() noexcept override;

    virtual Result wait_idle() noexcept override;
    virtual Result queue_wait_idle(Queue_Type queue, uint64_t timeout) noexcept override;

    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept override;

    virtual [[nodiscard]] std::unique_ptr<Swapchain> create_swapchain(
        const Swapchain_Win32_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::unique_ptr<Command_Pool> create_command_pool(
        const Command_Pool_Create_Info& create_info) noexcept override;

    virtual [[nodsicard]] std::expected<Fence*, Result> create_fence(uint64_t initial_value) noexcept override;
    virtual void destroy_fence(Fence* fence) noexcept override;

    virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(
        const Buffer_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual [[nodiscard]] std::expected<Buffer_View*, Result> create_buffer_view(
        Buffer* buffer, const Buffer_View_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual void destroy_buffer(Buffer* buffer) noexcept override;
    virtual void map_buffer(Buffer* buffer, std::size_t offset, std::size_t size) noexcept override;
    virtual void unmap_buffer(Buffer* buffer) noexcept override;

    virtual [[nodiscard]] std::expected<Image*, Result> create_image(
        const Image_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual [[nodiscard]] std::expected<Image_View*, Result> create_image_view(
        Image* image, const Image_View_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual void destroy_image(Image* image) noexcept override;

    [[nodiscard]] Vulkan_Image* create_proxy_image() noexcept;
    void destroy_proxy_image(Vulkan_Image* image) noexcept;

    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(
        const Sampler_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual void destroy_sampler(Sampler* sampler) noexcept override;

    virtual [[nodiscard]] std::expected<Acceleration_Structure*, Result> create_acceleration_structure(
        const Acceleration_Structure_Create_Info& create_info) noexcept override;
    virtual void destroy_acceleration_structure(Acceleration_Structure* acceleration_structure) noexcept override;

    virtual [[nodiscard]] std::expected<Shader_Blob*, Result> create_shader_blob(
        const Shader_Blob_Create_Info& create_info) noexcept override;
    virtual Result recreate_shader_blob(Shader_Blob* shader_blob, const Shader_Blob_Create_Info& create_info) noexcept override;
    virtual Result recreate_shader_blob_deserialize_memory(Shader_Blob* shader_blob, void* memory) noexcept override;
    virtual void destroy_shader_blob(Shader_Blob* shader_blob) noexcept override;

    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Graphics_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Compute_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept override;
    virtual void destroy_pipeline(Pipeline* pipeline) noexcept override;

    virtual [[nodiscard]] Acceleration_Structure_Build_Sizes get_acceleration_structure_build_sizes(
        const Acceleration_Structure_Build_Geometry_Info& build_info) noexcept override;

    virtual Result submit(const Submit_Info& submit_info) noexcept override;

    virtual void name_resource(Buffer* buffer, const char* name) noexcept override;
    virtual void name_resource(Image* image, const char* name) noexcept override;

    [[nodiscard]] uint32_t get_queue_family_index(VkQueueFlagBits queue_type) const noexcept;

    [[nodiscard]] operator VkInstance() const noexcept { return m_instance; }
    [[nodiscard]] operator VkPhysicalDevice() const noexcept { return m_physical_device; }
    [[nodiscard]] operator VkDevice() const noexcept { return m_device; }

    [[nodiscard]] const VkQueue get_queue(Queue_Type queue_type) const noexcept;

    [[nodiscard]] const VkDescriptorSet get_descriptor_set() const noexcept { return m_descriptor_set; }
    [[nodiscard]] const VkPipelineLayout get_pipeline_layout() const noexcept { return m_pipeline_layout; }

private:
    void create_acceleration_structure_descriptor(Vulkan_Acceleration_Structure* acceleration_structure);
    void create_buffer_descriptors(Vulkan_Buffer* buffer, bool create_storage_buffer_descriptor);
    void create_image_descriptors(Vulkan_Image* image, bool create_storage_image_descriptor);
    void create_image_view_descriptors(
        Vulkan_Image_View* image_view, const Image_View_Create_Info& create_info, bool create_storage_image_descriptor);

private:
    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;

    struct Queue
    {
        VkQueue queue;
        uint32_t index;

        operator VkQueue() const { return queue; }
        operator uint32_t() const { return index; }
    };
    Queue m_graphics_queue;
    Queue m_compute_queue;
    Queue m_copy_queue;

    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkDescriptorSetLayout m_descriptor_set_layout;
    VkDescriptorPool m_descriptor_pool;
    VkDescriptorSet m_descriptor_set;
    VkPipelineLayout m_pipeline_layout;

    bool m_use_mutex;
    std::mutex m_resource_mutex;
    std::mutex m_direct_queue_mutex;
    std::mutex m_compute_queue_mutex;
    std::mutex m_copy_queue_mutex;

    Vulkan_Resource_Pool m_resource_pool;

    plf::colony<Vulkan_Fence> m_fences;
    plf::colony<Shader_Blob> m_shader_blobs;
    plf::colony<Vulkan_Pipeline> m_pipelines;
};
}
