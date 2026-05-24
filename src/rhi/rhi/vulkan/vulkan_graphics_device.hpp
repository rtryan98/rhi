#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
#include <VkBootstrap.h>

#include "rhi/graphics_device.hpp"
#include "rhi/common/resource_pool.hpp"
#include "rhi/vulkan/vulkan_resource.hpp"

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

// Descriptor heap management
struct Descriptor_Heap
{
    VkBuffer buffer;
    VmaAllocation allocation;
    void* data;
    VkDeviceSize descriptor_size;
    VkDeviceSize stride;
    VkDeviceAddressRangeEXT heap_range;
    VkDeviceSize reserved_offset;
    VkDeviceSize reserved_size;
};

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

    virtual Result submit(const Submit_Info& submit_info) noexcept override;

    virtual void name_resource(Buffer* buffer, const char* name) noexcept override;
    virtual void name_resource(Image* image, const char* name) noexcept override;

    [[nodiscard]] VkInstance get_instance() const noexcept { return m_instance.instance; }
    [[nodiscard]] VkPhysicalDevice get_physical_device() const noexcept { return m_physical_device.physical_device; }
    [[nodiscard]] const vkb::Device& get_device() const noexcept { return m_device; }
    [[nodiscard]] VkQueue get_graphics_queue() const noexcept { return m_device.get_queue(vkb::QueueType::graphics).value(); }
    [[nodiscard]] uint32_t get_graphics_queue_family() const noexcept { return m_device.get_queue_index(vkb::QueueType::graphics).value(); }

    [[nodiscard]] uint32_t get_queue_family_index( vkb::QueueType queue_type ) const noexcept
    {
        return m_device.get_queue_index(queue_type).value();
    }

    [[nodiscard]] const Descriptor_Heap& get_resource_descriptor_heap() const noexcept { return m_resource_descriptor_heap; }
    [[nodiscard]] const Descriptor_Heap& get_sampler_descriptor_heap() const noexcept { return m_sampler_descriptor_heap; }

private:
    void create_acceleration_structure_descriptor(Vulkan_Acceleration_Structure* acceleration_structure);
    void create_buffer_descriptors(Vulkan_Buffer* buffer, bool create_storage_buffer_descriptor);
    void create_image_descriptors(Vulkan_Image* image, bool create_storage_image_descriptor);
    void create_image_view_descriptors(
        Vulkan_Image_View* image_view, const Image_View_Create_Info& create_info, bool create_storage_image_descriptor);
    VkHostAddressRangeEXT descriptor_index_to_address(uint32_t index, uint32_t offset, bool is_sampler);

private:
    vkb::Instance m_instance;
    vkb::PhysicalDevice m_physical_device;
    vkb::Device m_device;
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    bool m_use_mutex;
    std::mutex m_resource_mutex;
    std::mutex m_direct_queue_mutex;
    std::mutex m_compute_queue_mutex;
    std::mutex m_copy_queue_mutex;

    Vulkan_Resource_Pool m_resource_pool;

    plf::colony<Vulkan_Fence> m_fences;
    plf::colony<Shader_Blob> m_shader_blobs;
    plf::colony<Vulkan_Pipeline> m_pipelines;

    Descriptor_Heap m_resource_descriptor_heap;
    Descriptor_Heap m_sampler_descriptor_heap;
};
}
