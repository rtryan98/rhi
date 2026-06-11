#include "rhi/vulkan/vulkan_graphics_device.hpp"

#include "rhi/vulkan/vulkan_init.hpp"
#include "rhi/vulkan/vulkan_cast.hpp"
#include "rhi/vulkan/vulkan_command_list.hpp"
#include "rhi/vulkan/vulkan_resource.hpp"
#include "rhi/vulkan/vulkan_result.hpp"
#include "rhi/vulkan/vulkan_swapchain.hpp"

namespace rhi::vulkan
{
constexpr static auto APPLICATION_ENGINE_NAME = "rhi";
constexpr static auto VULKAN_RHI_VERSION = VK_MAKE_VERSION(0, 1, 0);

Vulkan_Graphics_Device::Vulkan_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept
    : m_use_mutex(create_info.enable_locking)
    , m_resource_pool(
        MAX_RESOURCE_INDEX - create_info.reserved_bindless_resource_index_count,
        MAX_SAMPLER_INDEX - create_info.reserved_bindless_sampler_index_count,
        2, // TODO: stride same as D3D12?
        decltype(m_resource_pool)::Deleters {
            .buffer_delete_function = [this](Vulkan_Buffer* buffer) {
                if (buffer && buffer->buffer != VK_NULL_HANDLE && buffer->allocation != VK_NULL_HANDLE)
                {
                    vmaDestroyBuffer(m_allocator, buffer->buffer, buffer->allocation);
                    buffer->buffer = VK_NULL_HANDLE;
                    buffer->allocation = VK_NULL_HANDLE;
                }
            },
            .buffer_view_delete_function = [this](Vulkan_Buffer_View* buffer_view) {
                if (buffer_view && buffer_view->buffer_view != VK_NULL_HANDLE)
                {
                    vkDestroyBufferView(m_device, buffer_view->buffer_view, nullptr);
                    buffer_view->buffer_view = VK_NULL_HANDLE;
                }
            },
            .image_delete_function = [this](Vulkan_Image* image) {
                if (image && image->image != VK_NULL_HANDLE && image->allocation != VK_NULL_HANDLE)
                {
                    vmaDestroyImage(m_allocator, image->image, image->allocation);
                    image->image = VK_NULL_HANDLE;
                    image->allocation = VK_NULL_HANDLE;
                }
            },
            .image_view_delete_function = [this](Vulkan_Image_View* image_view) {
                if (image_view && image_view->image_view != VK_NULL_HANDLE)
                {
                    vkDestroyImageView(m_device, image_view->image_view, nullptr);
                    image_view->image_view = VK_NULL_HANDLE;
                }
            },
            .sampler_delete_function = [this](Vulkan_Sampler* sampler) {
                if (sampler && sampler->sampler != VK_NULL_HANDLE)
                {
                    vkDestroySampler(m_device, sampler->sampler, nullptr);
                    sampler->sampler = VK_NULL_HANDLE;
                }
            },
            .acceleration_structure_delete_function = [this](Vulkan_Acceleration_Structure* acceleration_structure) {
                if (acceleration_structure && acceleration_structure->acceleration_structure != VK_NULL_HANDLE)
                {
                    vkDestroyAccelerationStructureKHR(m_device, acceleration_structure->acceleration_structure, nullptr);
                    acceleration_structure->acceleration_structure = VK_NULL_HANDLE;
                }
            }
        })
{
    volkInitialize();

    m_instance = create_instance(create_info.enable_validation, create_info.enable_gpu_validation);
    m_physical_device = select_physical_device(m_instance);
    m_device = create_device(m_physical_device);

    // Queue setup
    {
        auto queue_infos = get_queue_infos(m_physical_device);

        vkGetDeviceQueue(m_device, queue_infos.graphics_queue_index, 0, &m_graphics_queue.queue);
        m_graphics_queue.index = queue_infos.graphics_queue_index;

        vkGetDeviceQueue(m_device, queue_infos.compute_queue_index, 0, &m_compute_queue.queue);
        m_compute_queue.index = queue_infos.compute_queue_index;

        vkGetDeviceQueue(m_device, queue_infos.copy_queue_index, 0, &m_copy_queue.queue);
        m_copy_queue.index = queue_infos.copy_queue_index;

        vkGetDeviceQueue(m_device, queue_infos.video_decode_queue_index, 0, &m_video_decode_queue.queue);
        m_video_decode_queue.index = queue_infos.video_decode_queue_index;

        vkGetDeviceQueue(m_device, queue_infos.video_encode_queue_index, 0, &m_video_encode_queue.queue);
        m_video_encode_queue.index = queue_infos.video_encode_queue_index;
    }

    VmaVulkanFunctions allocator_vulkan_functions = {};
    VmaAllocatorCreateInfo allocator_create_info = {
        .flags = VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT
            | VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT
            | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = m_physical_device,
        .device = m_device,
        .pVulkanFunctions = &allocator_vulkan_functions,
        .instance = m_instance,
        .vulkanApiVersion = VK_API_VERSION_1_4,
    };
    vmaImportVulkanFunctionsFromVolk(&allocator_create_info, &allocator_vulkan_functions);
    auto allocator_result = vmaCreateAllocator(&allocator_create_info, &m_allocator);
    if (allocator_result != VK_SUCCESS)
    {
        assert(false && "Failed to create Vulkan memory allocator.");
    }

    m_descriptor_set_layout = create_descriptor_set_layout(m_device);
    m_descriptor_pool = create_descriptor_pool(m_device);
    m_descriptor_set = create_descriptor_set(m_device, m_descriptor_set_layout, m_descriptor_pool);
    m_pipeline_layout = create_pipeline_layout(m_device, m_descriptor_set_layout, PUSH_CONSTANT_MAX_SIZE);
}

Vulkan_Graphics_Device::~Vulkan_Graphics_Device() noexcept
{
    wait_idle();
    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

Result Vulkan_Graphics_Device::wait_idle() noexcept
{
    return(translate_result(vkDeviceWaitIdle(m_device)));
}

Result Vulkan_Graphics_Device::queue_wait_idle(Queue_Type queue, [[maybe_unused]] uint64_t timeout) noexcept
{
    switch (queue)
    {
    case rhi::Queue_Type::Graphics:
        return(translate_result(vkQueueWaitIdle(m_graphics_queue)));
    case rhi::Queue_Type::Compute:
        return(translate_result(vkQueueWaitIdle(m_compute_queue)));
    case rhi::Queue_Type::Copy:
        return(translate_result(vkQueueWaitIdle(m_copy_queue)));
    case rhi::Queue_Type::Video_Decode:
        return(translate_result(vkQueueWaitIdle(m_video_decode_queue)));
    case rhi::Queue_Type::Video_Encode:
        return(translate_result(vkQueueWaitIdle(m_video_encode_queue)));
    default:
        return Result::Error_Invalid_Parameters;
    }
}

Graphics_API Vulkan_Graphics_Device::get_graphics_api() const noexcept
{
    return Graphics_API::Vulkan;
}

std::unique_ptr<Swapchain> Vulkan_Graphics_Device::create_swapchain(const Swapchain_Win32_Create_Info& create_info) noexcept
{
    return std::make_unique<Vulkan_Swapchain>(this, create_info);
}

std::unique_ptr<Command_Pool> Vulkan_Graphics_Device::create_command_pool(const Command_Pool_Create_Info& create_info) noexcept
{
    return std::make_unique<Vulkan_Command_Pool>(this, create_info);
}

std::expected<Fence*, Result> Vulkan_Graphics_Device::create_fence(uint64_t initial_value) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    VkSemaphore vulkan_semaphore = VK_NULL_HANDLE;

    VkSemaphoreTypeCreateInfo semaphore_type_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = initial_value
    };
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &semaphore_type_create_info,
        .flags = 0
    };

    auto result = translate_result(vkCreateSemaphore(
        m_device,
        &semaphore_create_info,
        nullptr,
        &vulkan_semaphore));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    Vulkan_Fence* fence = &*m_fences.emplace();
    fence->semaphore = vulkan_semaphore;
    fence->device = m_device;
    return fence;
}

void Vulkan_Graphics_Device::destroy_fence(Fence* fence) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto* vulkan_fence = static_cast<Vulkan_Fence*>(fence);
    vkDestroySemaphore(m_device, vulkan_fence->semaphore, nullptr);
    m_fences.erase(m_fences.get_iterator(vulkan_fence));
}

std::expected<Buffer*, Result> Vulkan_Graphics_Device::create_buffer(
    const Buffer_Create_Info& create_info, uint32_t index) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    const auto queue_families = std::to_array<uint32_t>({
        m_graphics_queue,
        m_compute_queue,
        m_copy_queue
        });

    VkBuffer vulkan_buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0.f
    };

    if (create_info.heap == Memory_Heap_Type::CPU_Readback)
    {
        allocation_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    }
    else
    {
        allocation_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = create_info.size,
        .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
            | (create_info.acceleration_structure_memory
                ? VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR
                : static_cast<VkBufferUsageFlags>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
            | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            | VK_BUFFER_USAGE_TRANSFER_DST_BIT
            | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size()),
        .pQueueFamilyIndices = queue_families.data()
    };
    VmaAllocationInfo allocation_info = {};
    auto buffer_result = vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_create_info, &vulkan_buffer, &allocation, &allocation_info);

    if (buffer_result != VK_SUCCESS)
    {
        return std::unexpected(translate_result(buffer_result));
    }

    auto* buffer = m_resource_pool.acquire_buffer(create_info, index);
    buffer->buffer = vulkan_buffer;
    buffer->allocation = allocation;
    buffer->data = allocation_info.pMappedData;

    VkBufferDeviceAddressInfo buffer_device_address_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext = nullptr,
        .buffer = buffer->buffer
    };
    buffer->gpu_address = vkGetBufferDeviceAddress(m_device, &buffer_device_address_info);

    create_buffer_descriptors(buffer, !create_info.acceleration_structure_memory);

    return buffer;
}

std::expected<Buffer_View*, Result> Vulkan_Graphics_Device::create_buffer_view(
    Buffer* buffer, const Buffer_View_Create_Info& create_info, uint32_t index) noexcept
{
    if (!buffer) return std::unexpected(Result::Error_Invalid_Parameters);

    if (create_info.size + create_info.offset > buffer->size)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    VkBufferViewCreateInfo buffer_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .buffer = static_cast<Vulkan_Buffer*>(buffer)->buffer,
        .format = VK_FORMAT_R8_UINT,
        .offset = create_info.offset,
        .range = create_info.size
    };
    VkBufferView vulkan_buffer_view = VK_NULL_HANDLE;
    auto buffer_view_result = vkCreateBufferView(m_device, &buffer_view_create_info, nullptr, &vulkan_buffer_view);

    if (buffer_view_result != VK_SUCCESS)
    {
        return std::unexpected(translate_result(buffer_view_result));
    }

    auto* buffer_view = m_resource_pool.acquire_buffer_view(buffer, create_info, index);
    buffer_view->buffer_view = vulkan_buffer_view;

    return buffer_view;
}

void Vulkan_Graphics_Device::destroy_buffer(Buffer* buffer) noexcept
{
    if (!buffer) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_buffer(buffer);
}

Result Vulkan_Fence::get_status(uint64_t value) noexcept
{
    auto current_value = ~0ull;
    auto result = vkGetSemaphoreCounterValue(device, semaphore, &current_value);
    if (result == VK_SUCCESS && current_value >= value)
    {
        return Result::Success;
    }
    return translate_result(result);
}

Result Vulkan_Fence::wait_for_value(uint64_t value) noexcept
{
    VkSemaphoreWaitInfo wait_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &semaphore,
        .pValues = &value
    };
    return(translate_result(vkWaitSemaphores(device, &wait_info, ~0ull)));
}

void Vulkan_Graphics_Device::map_buffer(Buffer* buffer, std::size_t offset, std::size_t size) noexcept
{
    return; // persistently mapped

    auto* vulkan_buffer = static_cast<Vulkan_Buffer*>(buffer);

    VmaAllocationInfo allocation_info = {};
    vmaGetAllocationInfo(m_allocator, vulkan_buffer->allocation, &allocation_info);
    VkMemoryMapInfo map_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO,
        .pNext = nullptr,
        .flags = 0,
        .memory = allocation_info.deviceMemory,
        .offset = offset,
        .size = size
    };
    vkMapMemory2(m_device, &map_info, &buffer->data);
}

void Vulkan_Graphics_Device::unmap_buffer(Buffer* buffer) noexcept
{
    return; // persistently mapped

    auto* vulkan_buffer = static_cast<Vulkan_Buffer*>(buffer);

    VmaAllocationInfo allocation_info = {};
    vmaGetAllocationInfo(m_allocator, vulkan_buffer->allocation, &allocation_info);
    VkMemoryUnmapInfo unmap_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO,
        .pNext = nullptr,
        .flags = 0,
        .memory = allocation_info.deviceMemory
    };
    vkUnmapMemory2(m_device, &unmap_info);
}

// TODO: put this function elsewhere
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

std::expected<Image*, Result> Vulkan_Graphics_Device::create_image(const Image_Create_Info& create_info, uint32_t index) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    VkImageCreateFlags image_create_flags = 0;
    if (create_info.primary_view_type == Image_View_Type::Texture_3D)
        image_create_flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
    if (create_info.primary_view_type == Image_View_Type::Texture_Cube
        || create_info.primary_view_type == Image_View_Type::Texture_Cube_Array)
        image_create_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

    VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = image_create_flags,
        .imageType = vulkan_cast<VkImageType>(create_info.primary_view_type),
        .format = vulkan_cast<VkFormat>(create_info.format),
        .extent = {
            .width = create_info.width,
            .height = create_info.height,
            .depth = create_info.depth
        },
        .mipLevels = create_info.mip_levels,
        .arrayLayers = create_info.array_size,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = vulkan_cast<VkImageUsageFlags>(create_info.usage)
            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VkImage vulkan_image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationCreateInfo allocation_create_info = {
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0.f
    };
    VmaAllocationInfo allocation_info = {};
    auto image_result = vmaCreateImage(m_allocator, &image_create_info, &allocation_create_info, &vulkan_image, &allocation, &allocation_info);

    if (image_result != VK_SUCCESS)
    {
        return std::unexpected(translate_result(image_result));
    }

    auto* image = m_resource_pool.acquire_image(create_info, index);
    image->image = vulkan_image;
    image->allocation = allocation;

    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = static_cast<Vulkan_Image*>(image)->image,
        .viewType = vulkan_cast<VkImageViewType>(image->primary_view_type),
        .format = vulkan_cast<VkFormat>(image->format),
        .components = {},
        .subresourceRange = {
            .aspectMask = get_aspect_mask(image),
            .baseMipLevel = 0,
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = 0,
            .layerCount = VK_REMAINING_ARRAY_LAYERS
        }
    };
    vkCreateImageView(m_device, &image_view_create_info, nullptr, &static_cast<Vulkan_Image_View*>(image->image_view)->image_view);

    create_image_descriptors(image, static_cast<uint32_t>(create_info.usage & Image_Usage::Unordered_Access) > 0u);

    return image;
}

std::expected<Image_View*, Result> Vulkan_Graphics_Device::create_image_view(
    Image* image, const Image_View_Create_Info& create_info, uint32_t index) noexcept
{
    if (!image) return std::unexpected(Result::Error_Invalid_Parameters);

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto vulkan_image = static_cast<Vulkan_Image*>(image);
    auto image_view = m_resource_pool.acquire_image_view(image, create_info, index);

    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = static_cast<Vulkan_Image*>(image_view->image)->image,
        .viewType = vulkan_cast<VkImageViewType>(create_info.view_type),
        .format = vulkan_cast<VkFormat>(image_view->image->format),
        .components = vulkan_cast<VkComponentMapping>(create_info.component_mapping),
        .subresourceRange = {
            .aspectMask = get_aspect_mask(image),
            .baseMipLevel = create_info.first_mip_level,
            .levelCount = create_info.mip_levels,
            .baseArrayLayer = create_info.first_array_level,
            .layerCount = create_info.array_levels
        }
    };
    vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view->image_view);

    create_image_view_descriptors(image_view, create_info, create_info.descriptor_type == Descriptor_Type::Resource);

    return image_view;
}

void Vulkan_Graphics_Device::destroy_image(Image* image) noexcept
{
    if (!image) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_image(image);
}

Vulkan_Image* Vulkan_Graphics_Device::create_proxy_image() noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    return m_resource_pool.acquire_proxy_image();
}

void Vulkan_Graphics_Device::destroy_proxy_image(Vulkan_Image* image) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_proxy_image(image);
}

std::expected<Sampler*, Result> Vulkan_Graphics_Device::create_sampler(const Sampler_Create_Info& create_info, uint32_t index) noexcept
{
    // A sampler with comparison must have a comparison function
    if (create_info.comparison_func == Comparison_Func::None &&
        create_info.reduction == Sampler_Reduction_Type::Comparison)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto* sampler = m_resource_pool.acquire_sampler(index);

    VkSamplerCreateInfo sampler_create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = vulkan_cast<VkFilter>(create_info.filter_mag),
        .minFilter = vulkan_cast<VkFilter>(create_info.filter_min),
        .mipmapMode = vulkan_cast<VkSamplerMipmapMode>(create_info.filter_mip),
        .addressModeU = vulkan_cast<VkSamplerAddressMode>(create_info.address_mode_u),
        .addressModeV = vulkan_cast<VkSamplerAddressMode>(create_info.address_mode_v),
        .addressModeW = vulkan_cast<VkSamplerAddressMode>(create_info.address_mode_w),
        .mipLodBias = create_info.mip_lod_bias,
        .anisotropyEnable = create_info.anisotropy_enable,
        .maxAnisotropy = static_cast<float>(create_info.max_anisotropy),
        .compareEnable = create_info.reduction == Sampler_Reduction_Type::Comparison,
        .compareOp = vulkan_cast<VkCompareOp>(create_info.comparison_func),
        .minLod = create_info.min_lod,
        .maxLod = create_info.max_lod,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE, // TODO: border color not yet implemented
        .unnormalizedCoordinates = VK_FALSE
    };
    vkCreateSampler(m_device, &sampler_create_info, nullptr, &sampler->sampler);

    VkDescriptorImageInfo image_info = {
        .sampler = sampler->sampler,
        .imageView = nullptr,
        .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_set,
        .dstBinding = 2,
        .dstArrayElement = sampler->bindless_index,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &image_info
    };
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);

    return sampler;
}

void Vulkan_Graphics_Device::destroy_sampler(Sampler* sampler) noexcept
{
    if (!sampler) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_sampler(sampler);
}

std::expected<Acceleration_Structure*, Result> Vulkan_Graphics_Device::create_acceleration_structure(
    const Acceleration_Structure_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto* acceleration_structure = m_resource_pool.acquire_acceleration_structure(NO_RESOURCE_INDEX);

    VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .createFlags = 0,
        .buffer = static_cast<Vulkan_Buffer*>(create_info.buffer)->buffer,
        .offset = create_info.offset,
        .size = create_info.size,
        .type = vulkan_cast<VkAccelerationStructureTypeKHR>(create_info.type),
        .deviceAddress = 0
    };
    vkCreateAccelerationStructureKHR(m_device, &acceleration_structure_create_info, nullptr, &acceleration_structure->acceleration_structure);

    VkAccelerationStructureDeviceAddressInfoKHR acceleration_structure_address_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
        .pNext = nullptr,
        .accelerationStructure = acceleration_structure->acceleration_structure
    };
    acceleration_structure->address = vkGetAccelerationStructureDeviceAddressKHR(m_device, &acceleration_structure_address_info);

    if (create_info.type != Acceleration_Structure_Type::Bottom_Level)
    {
        create_acceleration_structure_descriptor(acceleration_structure);
    }

    return acceleration_structure;
}

void Vulkan_Graphics_Device::destroy_acceleration_structure(Acceleration_Structure* acceleration_structure) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_acceleration_structure(acceleration_structure);
}

std::expected<Shader_Blob*, Result> Vulkan_Graphics_Device::create_shader_blob(const Shader_Blob_Create_Info& create_info) noexcept
{
    static_assert(
        sizeof(decltype(Shader_Blob::data)::value_type) == sizeof(uint8_t),
        "Size of blob changed.");

    if (create_info.data == nullptr || create_info.data_size == 0)
    {
        return std::unexpected(Result::Error_Invalid_Parameters);
    }

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto blob = &*m_shader_blobs.emplace();
    blob->data.resize(create_info.data_size);
    blob->groups_x = create_info.groups_x;
    blob->groups_y = create_info.groups_y;
    blob->groups_z = create_info.groups_z;
    memcpy(blob->data.data(), create_info.data, create_info.data_size);
    return blob;
}

Result Vulkan_Graphics_Device::recreate_shader_blob(Shader_Blob* shader_blob, const Shader_Blob_Create_Info& create_info) noexcept
{
    if (!shader_blob) return Result::Error_Invalid_Parameters;

    shader_blob->data.clear();
    shader_blob->data.resize(create_info.data_size);
    shader_blob->groups_x = create_info.groups_x;
    shader_blob->groups_y = create_info.groups_y;
    shader_blob->groups_z = create_info.groups_z;
    memcpy(shader_blob->data.data(), create_info.data, create_info.data_size);

    return Result::Success;
}

Result Vulkan_Graphics_Device::recreate_shader_blob_deserialize_memory(Shader_Blob* shader_blob, void* memory) noexcept
{
    if (!shader_blob || !memory) return Result::Error_Invalid_Parameters;

    uint8_t* blob_ptr = static_cast<uint8_t*>(memory);
    memcpy(&shader_blob->groups_x, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    memcpy(&shader_blob->groups_y, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    memcpy(&shader_blob->groups_z, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    uint32_t dxil_blob_size = 0;
    memcpy(&dxil_blob_size, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    uint32_t spirv_blob_size = 0;
    memcpy(&spirv_blob_size, blob_ptr, sizeof(uint32_t));
    blob_ptr += sizeof(uint32_t);
    blob_ptr += dxil_blob_size;

    shader_blob->data.clear();
    shader_blob->data.resize(spirv_blob_size);
    memcpy(shader_blob->data.data(), blob_ptr, spirv_blob_size);

    return Result::Success;
}

void Vulkan_Graphics_Device::destroy_shader_blob(Shader_Blob* shader_blob) noexcept
{
    if (shader_blob == nullptr) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_shader_blobs.erase(m_shader_blobs.get_iterator(shader_blob));
}

std::expected<Pipeline*, Result> Vulkan_Graphics_Device::create_pipeline(const Graphics_Pipeline_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto pipeline = &*m_pipelines.emplace();
    pipeline->type = Pipeline_Type::Vertex_Shading;
    pipeline->vertex_shading_info = create_info;

    std::vector<VkShaderModuleCreateInfo> shader_module_create_infos;
    shader_module_create_infos.reserve(5);
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
    shader_stage_create_infos.reserve(5);

    auto create_stage = [&](Shader_Blob* blob, VkShaderStageFlagBits vulkan_stage) {
        if (!blob)
            return;

        auto& module = shader_module_create_infos.emplace_back();
        module = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = static_cast<uint32_t>(blob->data.size()),
            .pCode = reinterpret_cast<uint32_t*>(blob->data.data())
        };
        auto& stage = shader_stage_create_infos.emplace_back();
        stage = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = &module,
            .flags = 0,
            .stage = vulkan_stage,
            .module = VK_NULL_HANDLE,
            .pName = "main",
            .pSpecializationInfo = nullptr
        };
    };

    create_stage(create_info.vs, VK_SHADER_STAGE_VERTEX_BIT);
    create_stage(create_info.hs, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    create_stage(create_info.ds, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
    create_stage(create_info.gs, VK_SHADER_STAGE_GEOMETRY_BIT);
    create_stage(create_info.ps, VK_SHADER_STAGE_FRAGMENT_BIT);

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = vulkan_cast<VkPrimitiveTopology>(create_info.primitive_topology),
        .primitiveRestartEnable = VK_FALSE
    };
    VkPipelineTessellationStateCreateInfo tessellation_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .patchControlPoints = 0
    };
    VkViewport dummy_viewport = {};
    VkRect2D dummy_scissor = {};
    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .viewportCount = 1,
        .pViewports = &dummy_viewport,
        .scissorCount = 1,
        .pScissors = &dummy_scissor
    };
    VkPipelineRasterizationDepthClipStateCreateInfoEXT depth_clip_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .depthClipEnable = create_info.rasterizer_state_info.depth_clip_enable
    };
    bool depth_bias_enable = false;
    depth_bias_enable |= create_info.rasterizer_state_info.depth_bias != 0.f;
    depth_bias_enable |= create_info.rasterizer_state_info.depth_bias_slope_scale != 0.f;
    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = &depth_clip_state_create_info,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = vulkan_cast<VkPolygonMode>(create_info.rasterizer_state_info.fill_mode),
        .cullMode = vulkan_cast<VkCullModeFlags>(create_info.rasterizer_state_info.cull_mode),
        .frontFace = vulkan_cast<VkFrontFace>(create_info.rasterizer_state_info.winding_order),
        .depthBiasEnable = depth_bias_enable,
        .depthBiasConstantFactor = create_info.rasterizer_state_info.depth_bias,
        .depthBiasClamp = create_info.rasterizer_state_info.depth_bias_clamp,
        .depthBiasSlopeFactor = create_info.rasterizer_state_info.depth_bias_slope_scale,
        .lineWidth = 1.f
    };
    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = create_info.depth_stencil_info.depth_enable,
        .depthWriteEnable = create_info.depth_stencil_info.depth_write_enable,
        .depthCompareOp = vulkan_cast<VkCompareOp>(create_info.depth_stencil_info.comparison_func),
        .depthBoundsTestEnable = create_info.depth_stencil_info.depth_bounds_test_mode != Depth_Bounds_Test_Mode::Disabled,
        .stencilTestEnable = create_info.depth_stencil_info.stencil_enable,
        .front = {
            .failOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_front_face.fail),
            .passOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_front_face.pass),
            .depthFailOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_front_face.depth_fail),
            .compareOp = vulkan_cast<VkCompareOp>(create_info.depth_stencil_info.stencil_front_face.comparison_func),
            .compareMask = ~0u,
            .writeMask = create_info.depth_stencil_info.stencil_front_face.stencil_write_mask,
            .reference = create_info.depth_stencil_info.stencil_front_face.stencil_read_mask
        },
        .back = {
            .failOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_back_face.fail),
            .passOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_back_face.pass),
            .depthFailOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_back_face.depth_fail),
            .compareOp = vulkan_cast<VkCompareOp>(create_info.depth_stencil_info.stencil_back_face.comparison_func),
            .compareMask = ~0u,
            .writeMask = create_info.depth_stencil_info.stencil_back_face.stencil_write_mask,
            .reference = create_info.depth_stencil_info.stencil_back_face.stencil_read_mask
        },
        .minDepthBounds = create_info.depth_stencil_info.depth_bounds_min,
        .maxDepthBounds = create_info.depth_stencil_info.depth_bounds_max
    };

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states;
    color_blend_attachment_states.reserve(create_info.color_attachment_count);
    for (auto i = 0; i < create_info.color_attachment_count; ++i)
    {
        const auto& attachment_create_info = create_info.blend_state_info.color_attachments[i];
        auto& color_blend_attachment_state = color_blend_attachment_states.emplace_back();
        color_blend_attachment_state = {
            .blendEnable = attachment_create_info.blend_enable,
            .srcColorBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.color_src_blend),
            .dstColorBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.color_dst_blend),
            .colorBlendOp = vulkan_cast<VkBlendOp>(attachment_create_info.color_blend_op),
            .srcAlphaBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.alpha_src_blend),
            .dstAlphaBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.alpha_dst_blend),
            .alphaBlendOp = vulkan_cast<VkBlendOp>(attachment_create_info.alpha_blend_op),
            .colorWriteMask = vulkan_cast<VkColorComponentFlags>(attachment_create_info.color_write_mask)
        };
    }
    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_CLEAR,
        .attachmentCount = static_cast<uint32_t>(color_blend_attachment_states.size()),
        .pAttachments = color_blend_attachment_states.data(),
        .blendConstants = {
            1.f, 1.f, 1.f, 1.f
        }
    };
    for (auto i = 0; i < create_info.color_attachment_count; ++i)
    {
        // TODO: this doesn't match nicely to D3D12. Find a better way for logic ops.
        const auto& attachment_create_info = create_info.blend_state_info.color_attachments[i];
        if (attachment_create_info.logic_op_enable)
        {
            color_blend_state_create_info.logicOpEnable = VK_TRUE;
            color_blend_state_create_info.logicOp = vulkan_cast<VkLogicOp>(attachment_create_info.logic_op);
            break;
        }
    }

    auto dynamic_states = std::vector<VkDynamicState>({
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
        });
    if (create_info.depth_stencil_info.depth_bounds_test_mode == Depth_Bounds_Test_Mode::Dynamic)
    {
        dynamic_states.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
    }
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data()
    };

    std::vector<VkFormat> color_attachment_formats;
    color_attachment_formats.reserve(create_info.color_attachment_count);
    for (auto i = 0; i < create_info.color_attachment_count; ++i)
    {
        color_attachment_formats.push_back(vulkan_cast<VkFormat>(create_info.color_attachment_formats[i]));
    }
    VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = create_info.color_attachment_count,
        .pColorAttachmentFormats = color_attachment_formats.data(),
        .depthAttachmentFormat = vulkan_cast<VkFormat>(create_info.depth_stencil_format),
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED // TODO: stencil format
    };
    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipeline_rendering_create_info,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(shader_stage_create_infos.size()),
        .pStages = shader_stage_create_infos.data(),
        .pVertexInputState = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pTessellationState = &tessellation_state_create_info,
        .pViewportState = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState = &multisample_state_create_info,
        .pDepthStencilState = &depth_stencil_state_create_info,
        .pColorBlendState = &color_blend_state_create_info,
        .pDynamicState = &dynamic_state_create_info,
        .layout = m_pipeline_layout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };
    vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline->pipeline);

    return pipeline;
}

std::expected<Pipeline*, Result> Vulkan_Graphics_Device::create_pipeline(const Compute_Pipeline_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto pipeline = &*m_pipelines.emplace();
    pipeline->type = Pipeline_Type::Compute;
    pipeline->compute_shading_info = create_info;

    VkShaderModuleCreateInfo stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = static_cast<uint32_t>(create_info.cs->data.size()),
        .pCode = reinterpret_cast<uint32_t*>(create_info.cs->data.data())
    };
    VkComputePipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = &stage_create_info,
            .flags = 0,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = VK_NULL_HANDLE,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
        .layout = m_pipeline_layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };
    vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline->pipeline);

    return pipeline;
}

std::expected<Pipeline*, Result> Vulkan_Graphics_Device::create_pipeline(const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto pipeline = &*m_pipelines.emplace();
    pipeline->type = Pipeline_Type::Mesh_Shading;
    pipeline->mesh_shading_info = create_info;

    std::vector<VkShaderModuleCreateInfo> shader_module_create_infos;
    shader_module_create_infos.reserve(5);
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
    shader_stage_create_infos.reserve(5);

    auto create_stage = [&](Shader_Blob* blob, VkShaderStageFlagBits vulkan_stage) {
        if (!blob)
            return;

        auto& module = shader_module_create_infos.emplace_back();
        module = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = static_cast<uint32_t>(blob->data.size()),
            .pCode = reinterpret_cast<uint32_t*>(blob->data.data())
        };
        auto& stage = shader_stage_create_infos.emplace_back();
        stage = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = &module,
            .flags = 0,
            .stage = vulkan_stage,
            .module = VK_NULL_HANDLE,
            .pName = "main",
            .pSpecializationInfo = nullptr
        };
        };

    create_stage(create_info.ts, VK_SHADER_STAGE_TASK_BIT_EXT);
    create_stage(create_info.ms, VK_SHADER_STAGE_MESH_BIT_EXT);
    create_stage(create_info.ps, VK_SHADER_STAGE_FRAGMENT_BIT);

    VkViewport dummy_viewport = {};
    VkRect2D dummy_scissor = {};
    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .viewportCount = 1,
        .pViewports = &dummy_viewport,
        .scissorCount = 1,
        .pScissors = &dummy_scissor
    };
    VkPipelineRasterizationDepthClipStateCreateInfoEXT depth_clip_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .depthClipEnable = create_info.rasterizer_state_info.depth_clip_enable
    };
    bool depth_bias_enable = false;
    depth_bias_enable |= create_info.rasterizer_state_info.depth_bias != 0.f;
    depth_bias_enable |= create_info.rasterizer_state_info.depth_bias_slope_scale != 0.f;
    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = &depth_clip_state_create_info,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = vulkan_cast<VkPolygonMode>(create_info.rasterizer_state_info.fill_mode),
        .cullMode = vulkan_cast<VkCullModeFlags>(create_info.rasterizer_state_info.cull_mode),
        .frontFace = vulkan_cast<VkFrontFace>(create_info.rasterizer_state_info.winding_order),
        .depthBiasEnable = depth_bias_enable,
        .depthBiasConstantFactor = create_info.rasterizer_state_info.depth_bias,
        .depthBiasClamp = create_info.rasterizer_state_info.depth_bias_clamp,
        .depthBiasSlopeFactor = create_info.rasterizer_state_info.depth_bias_slope_scale,
        .lineWidth = 1.f
    };
    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = create_info.depth_stencil_info.depth_enable,
        .depthWriteEnable = create_info.depth_stencil_info.depth_write_enable,
        .depthCompareOp = vulkan_cast<VkCompareOp>(create_info.depth_stencil_info.comparison_func),
        .depthBoundsTestEnable = create_info.depth_stencil_info.depth_bounds_test_mode != Depth_Bounds_Test_Mode::Disabled,
        .stencilTestEnable = create_info.depth_stencil_info.stencil_enable,
        .front = {
            .failOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_front_face.fail),
            .passOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_front_face.pass),
            .depthFailOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_front_face.depth_fail),
            .compareOp = vulkan_cast<VkCompareOp>(create_info.depth_stencil_info.stencil_front_face.comparison_func),
            .compareMask = ~0u,
            .writeMask = create_info.depth_stencil_info.stencil_front_face.stencil_write_mask,
            .reference = create_info.depth_stencil_info.stencil_front_face.stencil_read_mask
        },
        .back = {
            .failOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_back_face.fail),
            .passOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_back_face.pass),
            .depthFailOp = vulkan_cast<VkStencilOp>(create_info.depth_stencil_info.stencil_back_face.depth_fail),
            .compareOp = vulkan_cast<VkCompareOp>(create_info.depth_stencil_info.stencil_back_face.comparison_func),
            .compareMask = ~0u,
            .writeMask = create_info.depth_stencil_info.stencil_back_face.stencil_write_mask,
            .reference = create_info.depth_stencil_info.stencil_back_face.stencil_read_mask
        },
        .minDepthBounds = create_info.depth_stencil_info.depth_bounds_min,
        .maxDepthBounds = create_info.depth_stencil_info.depth_bounds_max
    };

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states;
    color_blend_attachment_states.reserve(create_info.color_attachment_count);
    for (auto i = 0; i < create_info.color_attachment_count; ++i)
    {
        const auto& attachment_create_info = create_info.blend_state_info.color_attachments[i];
        auto& color_blend_attachment_state = color_blend_attachment_states.emplace_back();
        color_blend_attachment_state = {
            .blendEnable = attachment_create_info.blend_enable,
            .srcColorBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.color_src_blend),
            .dstColorBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.color_dst_blend),
            .colorBlendOp = vulkan_cast<VkBlendOp>(attachment_create_info.color_blend_op),
            .srcAlphaBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.alpha_src_blend),
            .dstAlphaBlendFactor = vulkan_cast<VkBlendFactor>(attachment_create_info.alpha_dst_blend),
            .alphaBlendOp = vulkan_cast<VkBlendOp>(attachment_create_info.alpha_blend_op),
            .colorWriteMask = vulkan_cast<VkColorComponentFlags>(attachment_create_info.color_write_mask)
        };
    }
    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_CLEAR,
        .attachmentCount = static_cast<uint32_t>(color_blend_attachment_states.size()),
        .pAttachments = color_blend_attachment_states.data(),
        .blendConstants = {
            1.f, 1.f, 1.f, 1.f
        }
    };
    for (auto i = 0; i < create_info.color_attachment_count; ++i)
    {
        // TODO: this doesn't match nicely to D3D12. Find a better way for logic ops.
        const auto& attachment_create_info = create_info.blend_state_info.color_attachments[i];
        if (attachment_create_info.logic_op_enable)
        {
            color_blend_state_create_info.logicOpEnable = VK_TRUE;
            color_blend_state_create_info.logicOp = vulkan_cast<VkLogicOp>(attachment_create_info.logic_op);
            break;
        }
    }

    auto dynamic_states = std::vector<VkDynamicState>({
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
        });
    if (create_info.depth_stencil_info.depth_bounds_test_mode == Depth_Bounds_Test_Mode::Dynamic)
    {
        dynamic_states.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
    }
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data()
    };

    std::vector<VkFormat> color_attachment_formats;
    color_attachment_formats.reserve(create_info.color_attachment_count);
    for (auto i = 0; i < create_info.color_attachment_count; ++i)
    {
        color_attachment_formats.push_back(vulkan_cast<VkFormat>(create_info.color_attachment_formats[i]));
    }
    VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = create_info.color_attachment_count,
        .pColorAttachmentFormats = color_attachment_formats.data(),
        .depthAttachmentFormat = vulkan_cast<VkFormat>(create_info.depth_stencil_format),
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED // TODO: stencil format
    };
    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipeline_rendering_create_info,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(shader_stage_create_infos.size()),
        .pStages = shader_stage_create_infos.data(),
        .pVertexInputState = nullptr,
        .pInputAssemblyState = nullptr,
        .pTessellationState = nullptr,
        .pViewportState = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState = &multisample_state_create_info,
        .pDepthStencilState = &depth_stencil_state_create_info,
        .pColorBlendState = &color_blend_state_create_info,
        .pDynamicState = &dynamic_state_create_info,
        .layout = m_pipeline_layout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };
    vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline->pipeline);

    return pipeline;
}

void Vulkan_Graphics_Device::destroy_pipeline(Pipeline* pipeline) noexcept
{
    if (pipeline == nullptr) return;

    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    auto vulkan_pipeline = static_cast<Vulkan_Pipeline*>(pipeline);
    if (vulkan_pipeline->pipeline)
        vkDestroyPipeline(m_device, vulkan_pipeline->pipeline, nullptr);
    vulkan_pipeline->pipeline = VK_NULL_HANDLE;

    m_pipelines.erase(m_pipelines.get_iterator(vulkan_pipeline));
}

Acceleration_Structure_Build_Sizes Vulkan_Graphics_Device::get_acceleration_structure_build_sizes(
    const Acceleration_Structure_Build_Geometry_Info& build_info) noexcept
{
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> build_ranges;
    std::vector<VkAccelerationStructureGeometryKHR> geometries;
    std::vector<uint32_t> max_primitives;

    if (build_info.type == Acceleration_Structure_Type::Bottom_Level)
    {
        geometries.reserve(build_info.geometry_or_instance_count);
        build_ranges.reserve(build_info.geometry_or_instance_count);

        for (auto i = 0; i < build_info.geometry_or_instance_count; ++i)
        {
            auto& geometry = build_info.geometry[i];
            auto& vulkan_geometry = geometries.emplace_back();
            auto& build_range = build_ranges.emplace_back();

            vulkan_geometry = {
                .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
                .pNext = nullptr,
                .flags = std::bit_cast<VkGeometryFlagsKHR>(geometry.flags)
            };

            if (geometry.type == Acceleration_Structure_Geometry_Type::Triangles)
            {
                const auto& triangles = geometry.geometry.triangles;

                vulkan_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
                vulkan_geometry.geometry = {
                    .triangles = {
                        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
                        .pNext = nullptr,
                        .vertexFormat = vulkan_cast<VkFormat>(triangles.vertex_format),
                        .vertexData = triangles.vertex_gpu_address,
                        .vertexStride = triangles.vertex_stride,
                        .maxVertex = triangles.vertex_count - 1, // TODO: spec mentions amount of vertices in vertexData - 1
                        .indexType = triangles.index_type == Index_Type::U16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32,
                        .indexData = triangles.index_gpu_address,
                        .transformData = triangles.transform_gpu_address
                    }
                };
                build_range = {
                    .primitiveCount = triangles.index_count / 3,
                    .primitiveOffset = 0,
                    .firstVertex = 0,
                    .transformOffset = 0
                };

                max_primitives.push_back(build_range.primitiveCount);
            }
            else
            {
                const auto& aabbs = geometry.geometry.aabbs;

                vulkan_geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
                vulkan_geometry.geometry = {
                    .aabbs = {
                        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR,
                        .pNext = nullptr,
                        .data = aabbs.aabb_gpu_address,
                        .stride = aabbs.aabb_stride
                    }
                };
                build_range = {
                    .primitiveCount = static_cast<uint32_t>(aabbs.aabb_count),
                    .primitiveOffset = 0,
                    .firstVertex = 0,
                    .transformOffset = 0
                };

                max_primitives.push_back(build_range.primitiveCount);
            }
        }
    }
    else // TLAS
    {
        auto& vulkan_geometry = geometries.emplace_back();
        auto& build_range = build_ranges.emplace_back();

        vulkan_geometry = {
            .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
            .pNext = nullptr,
            .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
            .geometry = {
                .instances = {
                    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
                    .pNext = nullptr,
                    .arrayOfPointers = build_info.instances.array_of_pointers,
                    .data = build_info.instances.instance_gpu_address
                }
            },
            .flags = 0
        };

        build_range = {
            .primitiveCount = build_info.geometry_or_instance_count,
            .primitiveOffset = 0,
            .firstVertex = 0,
            .transformOffset = 0
        };

        max_primitives.push_back(build_info.geometry_or_instance_count);
    }

    VkBuildAccelerationStructureFlagsKHR flags = 0;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Allow_Update) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Allow_Compaction) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Fast_Trace) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Fast_Build) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;

    VkAccelerationStructureBuildGeometryInfoKHR build_geometry_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
        .pNext = nullptr,
        .type = vulkan_cast<VkAccelerationStructureTypeKHR>(build_info.type),
        .flags = flags,
        .mode = build_info.src != nullptr
            ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
            : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
        .srcAccelerationStructure = build_info.src != nullptr
            ? static_cast<Vulkan_Acceleration_Structure*>(build_info.src)->acceleration_structure
            : VK_NULL_HANDLE,
        .dstAccelerationStructure = build_info.dst != nullptr
            ? static_cast<Vulkan_Acceleration_Structure*>(build_info.dst)->acceleration_structure
            : VK_NULL_HANDLE,
        .geometryCount = build_info.type == Acceleration_Structure_Type::Bottom_Level
            ? build_info.geometry_or_instance_count
            : 1,
        .pGeometries = geometries.data(),
        .ppGeometries = nullptr,
        .scratchData = 0ull
    };

    VkAccelerationStructureBuildSizesInfoKHR build_sizes_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
        .pNext = nullptr,
    };

    vkGetAccelerationStructureBuildSizesKHR(m_device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &build_geometry_info, max_primitives.data(), &build_sizes_info);

    Acceleration_Structure_Build_Sizes result = {
        .acceleration_structure_size = build_sizes_info.accelerationStructureSize,
        .acceleration_structure_scratch_build_size = build_sizes_info.buildScratchSize,
        .acceleration_structure_scratch_update_size = build_sizes_info.updateScratchSize
    };
    return result;
}

Result Vulkan_Graphics_Device::submit(const Submit_Info& submit_info) noexcept
{
    std::mutex* queue_mutex = nullptr;
    VkQueue queue = VK_NULL_HANDLE;
    switch (submit_info.queue_type)
    {
    case Queue_Type::Graphics:
        queue = m_graphics_queue;
        queue_mutex = &m_direct_queue_mutex;
        break;
    case Queue_Type::Compute:
        queue = m_compute_queue;
        queue_mutex = &m_compute_queue_mutex;
        break;
    case Queue_Type::Copy:
        queue = m_copy_queue;
        queue_mutex = &m_copy_queue_mutex;
        break;
    case Queue_Type::Video_Decode:
        queue = m_video_decode_queue;
        queue_mutex = &m_video_decode_queue_mutex;
        break;
    case Queue_Type::Video_Encode:
        queue = m_video_encode_queue;
        queue_mutex = &m_video_encode_queue_mutex;
        break;
    default:
        return Result::Error_Invalid_Parameters;
    }
    std::unique_lock<std::mutex> lock_guard(*queue_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    std::vector<VkSemaphoreSubmitInfo> semaphore_submit_wait_infos;
    semaphore_submit_wait_infos.reserve(submit_info.wait_infos.size() + 1ull);
    for (const auto& wait_info : submit_info.wait_infos)
    {
        semaphore_submit_wait_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = static_cast<Vulkan_Fence*>(wait_info.fence)->semaphore,
            .value = wait_info.value,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .deviceIndex = 0
            });
    }
    if (submit_info.wait_swapchain != nullptr)
    {
        semaphore_submit_wait_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = static_cast<Vulkan_Swapchain*>(submit_info.wait_swapchain)->get_current_acquire_semaphore(),
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .deviceIndex = 0
            });
    }

    std::vector<VkCommandBufferSubmitInfo> command_buffer_submit_infos;
    command_buffer_submit_infos.reserve(submit_info.command_lists.size());
    for (const auto* command_list : submit_info.command_lists)
    {
        vkEndCommandBuffer(static_cast<const Vulkan_Command_List*>(command_list)->get_internal_command_list());

        command_buffer_submit_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .pNext = nullptr,
            .commandBuffer = static_cast<const Vulkan_Command_List*>(command_list)->get_internal_command_list(),
            .deviceMask = 0
            });
    }

    std::vector<VkSemaphoreSubmitInfo> semaphore_submit_signal_infos;
    semaphore_submit_signal_infos.reserve(submit_info.signal_infos.size() + 1ull);
    for (const auto& signal_info : submit_info.signal_infos)
    {
        semaphore_submit_signal_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = static_cast<Vulkan_Fence*>(signal_info.fence)->semaphore,
            .value = signal_info.value,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .deviceIndex = 0
            });
    }
    if (submit_info.present_swapchain != nullptr)
    {
        semaphore_submit_signal_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = static_cast<Vulkan_Swapchain*>(submit_info.present_swapchain)->get_current_present_semaphore(),
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .deviceIndex = 0
            });
    }

    VkSubmitInfo2 submit = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = nullptr,
        .flags = 0,
        .waitSemaphoreInfoCount = static_cast<uint32_t>(semaphore_submit_wait_infos.size()),
        .pWaitSemaphoreInfos = semaphore_submit_wait_infos.data(),
        .commandBufferInfoCount = static_cast<uint32_t>(command_buffer_submit_infos.size()),
        .pCommandBufferInfos = command_buffer_submit_infos.data(),
        .signalSemaphoreInfoCount = static_cast<uint32_t>(semaphore_submit_signal_infos.size()),
        .pSignalSemaphoreInfos = semaphore_submit_signal_infos.data()
    };
    return translate_result(vkQueueSubmit2(queue, 1, &submit, VK_NULL_HANDLE));
}

void Vulkan_Graphics_Device::name_resource(Buffer* buffer, const char* name) noexcept
{
    VkDebugUtilsObjectNameInfoEXT name_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext = nullptr,
        .objectType = VK_OBJECT_TYPE_BUFFER,
        .objectHandle = std::bit_cast<uint64_t>(static_cast<Vulkan_Buffer*>(buffer)->buffer),
        .pObjectName = name
    };
    vkSetDebugUtilsObjectNameEXT(m_device, &name_info);
}

void Vulkan_Graphics_Device::name_resource(Image* image, const char* name) noexcept
{
    VkDebugUtilsObjectNameInfoEXT name_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext = nullptr,
        .objectType = VK_OBJECT_TYPE_IMAGE,
        .objectHandle = std::bit_cast<uint64_t>(static_cast<Vulkan_Image*>(image)->image),
        .pObjectName = name
    };
    vkSetDebugUtilsObjectNameEXT(m_device, &name_info);
}

uint32_t Vulkan_Graphics_Device::get_queue_family_index(VkQueueFlagBits queue_type) const noexcept
{
    switch (queue_type)
    {
    case VK_QUEUE_GRAPHICS_BIT:
        return m_graphics_queue.index;
    case VK_QUEUE_COMPUTE_BIT:
        return m_compute_queue.index;
    case VK_QUEUE_TRANSFER_BIT:
        return m_copy_queue.index;
    case VK_QUEUE_VIDEO_DECODE_BIT_KHR:
        return m_video_decode_queue.index;
    case VK_QUEUE_VIDEO_ENCODE_BIT_KHR:
        return m_video_encode_queue.index;
    default:
        std::unreachable();
    }
}

const VkQueue Vulkan_Graphics_Device::get_queue(Queue_Type queue_type) const noexcept
{
    switch (queue_type)
    {
    case Queue_Type::Graphics:
        return m_graphics_queue.queue;
    case Queue_Type::Compute:
        return m_compute_queue.queue;
    case Queue_Type::Copy:
        return m_copy_queue.queue;
    case Queue_Type::Video_Decode:
        return m_video_decode_queue.queue;
    case Queue_Type::Video_Encode:
        return m_video_encode_queue.queue;
    default:
        std::unreachable();
    }
}

void Vulkan_Graphics_Device::create_acceleration_structure_descriptor(Vulkan_Acceleration_Structure* acceleration_structure)
{
    VkWriteDescriptorSetAccelerationStructureKHR write_acceleration_structure = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
        .pNext = nullptr,
        .accelerationStructureCount = 1,
        .pAccelerationStructures = &acceleration_structure->acceleration_structure
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = &write_acceleration_structure,
        .dstSet = m_descriptor_set,
        .dstBinding = 1,
        .dstArrayElement = acceleration_structure->bindless_index,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
    };
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

void Vulkan_Graphics_Device::create_buffer_descriptors(Vulkan_Buffer* buffer, bool create_storage_buffer_descriptor)
{
    VkDescriptorBufferInfo buffer_info = {
        .buffer = static_cast<Vulkan_Buffer*>(buffer)->buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_set,
        .dstBinding = 0,
        .dstArrayElement = buffer->buffer_view->bindless_index,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, // TODO: use two descriptors?
        .pBufferInfo = &buffer_info
    };
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);

    if (!create_storage_buffer_descriptor)
    {
        return;
    }

    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write_descriptor_set.dstArrayElement += 1;
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

void Vulkan_Graphics_Device::create_image_descriptors(Vulkan_Image* image, bool create_storage_image_descriptor)
{
    VkDescriptorImageInfo image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = static_cast<Vulkan_Image_View*>(image->image_view)->image_view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_set,
        .dstBinding = 0,
        .dstArrayElement = image->image_view->bindless_index,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &image_info
    };
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);

    if (!create_storage_image_descriptor)
    {
        return;
    }

    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write_descriptor_set.dstArrayElement += 1;
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

void Vulkan_Graphics_Device::create_image_view_descriptors(
    Vulkan_Image_View* image_view, const Image_View_Create_Info& create_info, bool create_storage_image_descriptor)
{
    VkDescriptorImageInfo image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = static_cast<Vulkan_Image_View*>(image_view)->image_view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_set,
        .dstBinding = 0,
        .dstArrayElement = image_view->bindless_index,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &image_info
    };
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);

    if (!create_storage_image_descriptor)
    {
        return;
    }

    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write_descriptor_set.dstArrayElement += 1;
    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}
}
