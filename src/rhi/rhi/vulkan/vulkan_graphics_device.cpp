#include "rhi/vulkan/vulkan_graphics_device.hpp"

#include "rhi/vulkan/vulkan_cast.hpp"
#include "rhi/vulkan/vulkan_resource.hpp"
#include "rhi/vulkan/vulkan_result.hpp"

#ifndef VOLK_IMPLEMENTATION
#define VOLK_IMPLEMENTATION
#endif
#include <volk.h>

namespace rhi::vulkan
{
constexpr static auto APPLICATION_ENGINE_NAME = "rhi";
constexpr static auto VULKAN_RHI_VERSION = VK_MAKE_VERSION(0, 1, 0);

auto select_physical_device(vkb::Instance& instance)
{
    vkb::PhysicalDeviceSelector physical_device_selector{ instance };
    constexpr static VkPhysicalDeviceFeatures REQUIRED_VK10_FEATURES = {
        .fullDrawIndexUint32 = VK_TRUE,
        .imageCubeArray = VK_TRUE,
        .independentBlend = VK_TRUE,
        .geometryShader = VK_TRUE,
        .tessellationShader = VK_TRUE,
        .sampleRateShading = VK_TRUE,
        .dualSrcBlend = VK_TRUE,
        .logicOp = VK_TRUE,
        .multiDrawIndirect = VK_TRUE,
        .drawIndirectFirstInstance = VK_TRUE,
        .depthClamp = VK_TRUE,
        .depthBiasClamp = VK_TRUE,
        .fillModeNonSolid = VK_TRUE,
        .depthBounds = VK_TRUE,
        .wideLines = VK_TRUE,
        .largePoints = VK_TRUE,
        .alphaToOne = VK_TRUE,
        .multiViewport = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
        .textureCompressionBC = VK_TRUE,
        .pipelineStatisticsQuery = VK_TRUE,
        .vertexPipelineStoresAndAtomics = VK_TRUE,
        .shaderImageGatherExtended = VK_TRUE,
        .shaderStorageImageExtendedFormats = VK_TRUE,
        .shaderStorageImageMultisample = VK_TRUE,
        .shaderStorageImageReadWithoutFormat = VK_TRUE,
        .shaderStorageImageWriteWithoutFormat = VK_TRUE,
        .shaderUniformBufferArrayDynamicIndexing = VK_TRUE,
        .shaderSampledImageArrayDynamicIndexing = VK_TRUE,
        .shaderStorageBufferArrayDynamicIndexing = VK_TRUE,
        .shaderStorageImageArrayDynamicIndexing = VK_TRUE,
        .shaderClipDistance = VK_TRUE,
        .shaderCullDistance = VK_TRUE,
        .shaderInt64 = VK_TRUE,
        .shaderInt16 = VK_TRUE,
        .shaderResourceMinLod = VK_TRUE
    };
    constexpr static VkPhysicalDeviceVulkan11Features REQUIRED_VK11_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = nullptr,
        .storageBuffer16BitAccess = VK_TRUE,
        .uniformAndStorageBuffer16BitAccess = VK_TRUE,
        .shaderDrawParameters = VK_TRUE
    };
    constexpr static VkPhysicalDeviceVulkan12Features REQUIRED_VK12_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = nullptr,
        .descriptorIndexing = VK_TRUE,
        .shaderInputAttachmentArrayDynamicIndexing = VK_TRUE,
        .shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE,
        .shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE,
        .shaderUniformBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageImageArrayNonUniformIndexing = VK_TRUE,
        .shaderInputAttachmentArrayNonUniformIndexing = VK_TRUE,
        .shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
        .timelineSemaphore = VK_TRUE
    };
    constexpr static VkPhysicalDeviceVulkan13Features REQUIRED_VK13_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = nullptr,
        .descriptorBindingInlineUniformBlockUpdateAfterBind = VK_TRUE,
        .shaderDemoteToHelperInvocation = VK_TRUE, // Match D3D12 behavior
        .shaderTerminateInvocation = VK_TRUE,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
        .maintenance4 = VK_TRUE
    };
    constexpr static VkPhysicalDeviceVulkan14Features REQUIRED_VK14_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = nullptr,
        .maintenance5 = VK_TRUE,
        .maintenance6 = VK_TRUE
    };
    constexpr static VkPhysicalDeviceDescriptorHeapFeaturesEXT REQUIRED_DESCRIPTOR_HEAP_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT,
        .pNext = nullptr,
        .descriptorHeap = VK_TRUE,
        .descriptorHeapCaptureReplay = VK_FALSE
    };
    constexpr static VkPhysicalDeviceShaderUntypedPointersFeaturesKHR REQUIRED_SHADER_UNTYPED_POINTERS_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR,
        .pNext = nullptr,
        .shaderUntypedPointers = VK_TRUE
    };

    // Ray tracing
    constexpr static VkPhysicalDeviceAccelerationStructureFeaturesKHR REQUIRED_ACCELERATION_STRUCTURE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
        .pNext = nullptr,
        .accelerationStructure = VK_TRUE,
        .accelerationStructureCaptureReplay = VK_FALSE,
        .accelerationStructureIndirectBuild = VK_FALSE,
        .accelerationStructureHostCommands = VK_FALSE,
        .descriptorBindingAccelerationStructureUpdateAfterBind = VK_FALSE
    };
    constexpr static VkPhysicalDeviceRayTracingPipelineFeaturesKHR REQUIRED_RAY_TRACING_PIPELINE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
        .pNext = nullptr,
        .rayTracingPipeline = VK_TRUE,
        .rayTracingPipelineShaderGroupHandleCaptureReplay = VK_FALSE,
        .rayTracingPipelineShaderGroupHandleCaptureReplayMixed = VK_FALSE,
        .rayTracingPipelineTraceRaysIndirect = VK_TRUE,
        .rayTraversalPrimitiveCulling = VK_TRUE
    };
    constexpr static VkPhysicalDeviceRayQueryFeaturesKHR REQUIRED_RAY_QUERY_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
        .pNext = nullptr,
        .rayQuery = VK_TRUE
    };
    constexpr static VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR REQUIRED_RAY_TRACING_MAINTENANCE1_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR,
        .pNext = nullptr,
        .rayTracingMaintenance1 = VK_TRUE,
        .rayTracingPipelineTraceRaysIndirect2 = VK_FALSE
    };

    // Mesh shading
    constexpr static VkPhysicalDeviceMeshShaderFeaturesEXT REQUIRED_MESH_SHADER_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
        .pNext = nullptr,
        .taskShader = VK_TRUE,
        .meshShader = VK_TRUE,
        .multiviewMeshShader = VK_FALSE,
        .primitiveFragmentShadingRateMeshShader = VK_FALSE,
        .meshShaderQueries = VK_TRUE
    };

    physical_device_selector
        .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
        .require_present()
        .require_separate_compute_queue()
        .require_separate_transfer_queue()
        .set_required_features(REQUIRED_VK10_FEATURES)
        .set_required_features_11(REQUIRED_VK11_FEATURES)
        .set_required_features_12(REQUIRED_VK12_FEATURES)
        .set_required_features_13(REQUIRED_VK13_FEATURES)
        .set_required_features_14(REQUIRED_VK14_FEATURES)
        .add_required_extension(VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_DESCRIPTOR_HEAP_FEATURES)
        .add_required_extension(VK_KHR_SHADER_UNTYPED_POINTERS_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_SHADER_UNTYPED_POINTERS_FEATURES)
        .add_required_extension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_ACCELERATION_STRUCTURE_FEATURES)
        .add_required_extension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_RAY_TRACING_PIPELINE_FEATURES)
        .add_required_extension(VK_KHR_RAY_QUERY_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_RAY_QUERY_FEATURES)
        .add_required_extension(VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_RAY_TRACING_MAINTENANCE1_FEATURES)
        .add_required_extension(VK_EXT_MESH_SHADER_EXTENSION_NAME)
        .add_required_extension_features(REQUIRED_MESH_SHADER_FEATURES)
        .defer_surface_initialization();
    return physical_device_selector.select();
}

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
                    vkDestroyBufferView(m_device.device, buffer_view->buffer_view, nullptr);
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
                    vkDestroyImageView(m_device.device, image_view->image_view, nullptr);
                    image_view->image_view = VK_NULL_HANDLE;
                }
            }
        })
{
    volkInitialize();

    vkb::InstanceBuilder instance_builder;
    instance_builder
        .set_app_name(APPLICATION_ENGINE_NAME)
        .set_app_version(VULKAN_RHI_VERSION)
        .set_engine_name(APPLICATION_ENGINE_NAME)
        .set_engine_version(VULKAN_RHI_VERSION)
        .require_api_version(VK_VERSION_1_4)
        .request_validation_layers(create_info.enable_validation);
    if (create_info.enable_gpu_validation)
    {
        instance_builder.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
    }
    auto instance_result = instance_builder.build();
    if (!instance_result)
    {
        assert(false && "Failed to create Vulkan instance.");
    }
    m_instance = instance_result.value();

    volkLoadInstance(m_instance);

    auto physical_device_result = select_physical_device(m_instance);
    if (!physical_device_result)
    {
        assert(false && "Failed to find suitable Vulkan device.");
    }
    m_physical_device = physical_device_result.value();

    vkb::DeviceBuilder device_builder{ m_physical_device };
    auto device_result = device_builder.build();
    if (!device_result)
    {
        assert(false && "Failed to create Vulkan device.");
    }
    m_device = device_result.value();

    volkLoadDevice(m_device);

    VmaAllocatorCreateInfo allocator_create_info = {
        .flags = VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT | VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT,
        .physicalDevice = m_physical_device.physical_device,
        .device = m_device.device,
        .instance = m_instance.instance,
        .vulkanApiVersion = m_instance.api_version,
    };
    auto allocator_result = vmaCreateAllocator(&allocator_create_info, &m_allocator);
    if (allocator_result != VK_SUCCESS)
    {
        assert(false && "Failed to create Vulkan memory allocator.");
    }

    auto create_descriptor_heap = [this](bool sampler_heap) -> Descriptor_Heap
    {
        static const auto sampler_descriptor_size = vkGetPhysicalDeviceDescriptorSizeEXT(
            m_physical_device, VK_DESCRIPTOR_TYPE_SAMPLER);
        static const auto resource_descriptor_size = std::max({
            vkGetPhysicalDeviceDescriptorSizeEXT(
                m_physical_device, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE),
            vkGetPhysicalDeviceDescriptorSizeEXT(
                m_physical_device, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE),
            vkGetPhysicalDeviceDescriptorSizeEXT(
                m_physical_device, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
            vkGetPhysicalDeviceDescriptorSizeEXT(
                m_physical_device, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
            });

        Descriptor_Heap result = {
            .alignment = sampler_heap
                ? sampler_descriptor_size * MAX_SAMPLER_INDEX
                : resource_descriptor_size * MAX_RESOURCE_INDEX * 2 // Two descriptor slots per resource
        };

        VkPhysicalDeviceDescriptorHeapPropertiesEXT descriptor_heap_properties = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_PROPERTIES_EXT,
            .pNext = nullptr
        };
        VkPhysicalDeviceProperties2 physical_device_properties2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
            .pNext = &descriptor_heap_properties
        };
        vkGetPhysicalDeviceProperties2(m_physical_device, &physical_device_properties2);

        result.reserved_offset = result.alignment;

        if (sampler_heap)
        {
            result.reserved_size = descriptor_heap_properties.minSamplerHeapReservedRange;
        }
        else
        {
            result.reserved_size = descriptor_heap_properties.minResourceHeapReservedRange;
        }

        const auto queue_families = std::to_array({
            m_device.get_queue_index(vkb::QueueType::graphics).value(),
            m_device.get_queue_index(vkb::QueueType::compute).value(),
            m_device.get_queue_index(vkb::QueueType::transfer).value()
            });

        VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = result.alignment + result.reserved_size,
            .usage = VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT,
            .sharingMode = VK_SHARING_MODE_CONCURRENT,
            .queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size()),
            .pQueueFamilyIndices = queue_families.data()
        };
        VmaAllocationCreateInfo allocation_create_info = {
            .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        VmaAllocationInfo allocation_info = {};
        vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_create_info, &result.buffer, &result.allocation, &allocation_info);
        result.data = allocation_info.pMappedData;

        return result;
    };
    m_resource_descriptor_heap = create_descriptor_heap(false);
    m_sampler_descriptor_heap = create_descriptor_heap(true);
}

Vulkan_Graphics_Device::~Vulkan_Graphics_Device() noexcept
{
    vkDeviceWaitIdle(m_device.device);
    vmaDestroyBuffer(m_allocator, m_resource_descriptor_heap.buffer, m_resource_descriptor_heap.allocation);
    vmaDestroyBuffer(m_allocator, m_sampler_descriptor_heap.buffer, m_sampler_descriptor_heap.allocation);
    vmaDestroyAllocator(m_allocator);
    vkb::destroy_device(m_device);
    vkb::destroy_instance(m_instance);
}

Result Vulkan_Graphics_Device::wait_idle() noexcept
{
    return(translate_result(vkDeviceWaitIdle(m_device.device)));
}

Result Vulkan_Graphics_Device::queue_wait_idle(Queue_Type queue, [[maybe_unused]] uint64_t timeout) noexcept
{
    vkb::Result<VkQueue> queue_result{ VK_NULL_HANDLE };
    switch (queue)
    {
    case rhi::Queue_Type::Graphics:
        queue_result = m_device.get_dedicated_queue(vkb::QueueType::graphics);
        break;
    case rhi::Queue_Type::Compute:
        queue_result = m_device.get_dedicated_queue(vkb::QueueType::compute);
        break;
    case rhi::Queue_Type::Copy:
        queue_result = m_device.get_dedicated_queue(vkb::QueueType::transfer);
        break;
    case rhi::Queue_Type::Video_Decode:
        assert(false && "Vulkan video decode queue is not implemented");
        break; // TODO: vkb custom queue
    case rhi::Queue_Type::Video_Encode:
        assert(false && "Vulkan video encode queue is not implemented");
        break; // TODO: vkb custom queue
    default:
        break;
    }
    if (!queue_result)
    {
        return Result::Error_Invalid_Parameters;
    }
    return(translate_result(vkQueueWaitIdle(queue_result.value())));
}

Graphics_API Vulkan_Graphics_Device::get_graphics_api() const noexcept
{
    return Graphics_API::Vulkan;
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
        m_device.device,
        &semaphore_create_info,
        nullptr,
        &vulkan_semaphore));
    if (result != Result::Success)
    {
        return std::unexpected(result);
    }

    Vulkan_Fence* fence = &*m_fences.emplace();
    fence->semaphore = vulkan_semaphore;
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
    vkDestroySemaphore(m_device.device, vulkan_fence->semaphore, nullptr);
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

    const auto queue_families = std::to_array({
        m_device.get_queue_index(vkb::QueueType::graphics).value(),
        m_device.get_queue_index(vkb::QueueType::compute).value(),
        m_device.get_queue_index(vkb::QueueType::transfer).value()
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
    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = create_info.size,
        .usage = create_info.acceleration_structure_memory
            ? VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR
            : static_cast<VkBufferUsageFlags>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
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
    auto buffer_view_result = vkCreateBufferView(m_device.device, &buffer_view_create_info, nullptr, &vulkan_buffer_view);

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
    m_resource_pool.release_buffer(buffer);
}

Result Vulkan_Fence::get_status(uint64_t value) noexcept
{
    auto current_value = ~0ull;
    auto result = vkGetSemaphoreCounterValue(device, semaphore, &current_value);
    if (value >= current_value)
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
        .pSemaphores = &semaphore,
        .pValues = &value
    };
    return(translate_result(vkWaitSemaphores(device, &wait_info, ~0ull)));
}

void Vulkan_Graphics_Device::map_buffer(Buffer* buffer, std::size_t offset, std::size_t size) noexcept
{
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
    vkMapMemory2(m_device.device, &map_info, &buffer->data);
}

void Vulkan_Graphics_Device::unmap_buffer(Buffer* buffer) noexcept
{
    auto* vulkan_buffer = static_cast<Vulkan_Buffer*>(buffer);

    VmaAllocationInfo allocation_info = {};
    vmaGetAllocationInfo(m_allocator, vulkan_buffer->allocation, &allocation_info);
    VkMemoryUnmapInfo unmap_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO,
        .pNext = nullptr,
        .flags = 0,
        .memory = allocation_info.deviceMemory
    };
    vkUnmapMemory2(m_device.device, &unmap_info);
}

std::expected<Image*, Result> Vulkan_Graphics_Device::create_image(const Image_Create_Info& create_info, uint32_t index) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
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
        .usage = vulkan_cast<VkImageUsageFlags>(create_info.usage),
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

    create_image_descriptors(image, static_cast<uint32_t>(create_info.usage & Image_Usage::Unordered_Access) > 0u);

    return std::unexpected(Result::Error_Unknown);
}

void Vulkan_Graphics_Device::destroy_image(Image* image) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_image(image);
}

std::expected<Sampler*, Result> Vulkan_Graphics_Device::create_sampler(const Sampler_Create_Info& create_info, uint32_t index) noexcept
{
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
        .compareEnable = create_info.reduction != Sampler_Reduction_Type::Comparison,
        .compareOp = vulkan_cast<VkCompareOp>(create_info.comparison_func),
        .minLod = create_info.min_lod,
        .maxLod = create_info.max_lod,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE, // TODO: border color not yet implemented
        .unnormalizedCoordinates = VK_FALSE
    };

    auto descriptor_range = descriptor_index_to_address(sampler->bindless_index, 0u, true);
    vkWriteSamplerDescriptorsEXT(m_device, 1, &sampler_create_info, &descriptor_range);

    return sampler;
}

void Vulkan_Graphics_Device::destroy_sampler(Sampler* sampler) noexcept
{
    std::unique_lock<std::mutex> lock_guard(m_resource_mutex, std::defer_lock);
    if (m_use_mutex)
    {
        lock_guard.lock();
    }

    m_resource_pool.release_sampler(sampler);
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
    vkSetDebugUtilsObjectNameEXT(m_device.device, &name_info);
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
    vkSetDebugUtilsObjectNameEXT(m_device.device, &name_info);
}

void Vulkan_Graphics_Device::create_acceleration_structure_descriptor(Vulkan_Buffer* buffer)
{
    // The Vulkan specification states:
    // For acceleration structures, the size of the range is not used by the descriptor, and can be set to 0.
    VkDeviceAddressRangeEXT descriptor_address_range = {
        .address = buffer->gpu_address,
        .size = 0
    };
    VkResourceDescriptorInfoEXT descriptor_info = {
        .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
        .pNext = nullptr,
        .type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        .data = {
            .pAddressRange = &descriptor_address_range
        }
    };
    auto descriptor_range = descriptor_index_to_address(buffer->buffer_view->bindless_index, 0u, false);
    vkWriteResourceDescriptorsEXT(m_device, 1, &descriptor_info, &descriptor_range);
}

void Vulkan_Graphics_Device::create_buffer_descriptors(Vulkan_Buffer* buffer, bool create_storage_buffer_descriptor)
{
    // The Vulkan specification states:
    // If a non-zero size is provided though, it must be a valid range.
    VkDeviceAddressRangeEXT descriptor_address_range = {
        .address = buffer->gpu_address,
        .size = buffer->size
    };
    VkResourceDescriptorInfoEXT descriptor_info = {
        .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
        .pNext = nullptr,
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .data = {
            .pAddressRange = &descriptor_address_range
        }
    };
    auto descriptor_range = descriptor_index_to_address(buffer->buffer_view->bindless_index, 0u, false);
    vkWriteResourceDescriptorsEXT(m_device, 1, &descriptor_info, &descriptor_range);

    // Unlike D3D12, in Vulkan there is no buffer SRV equivalent.
    // Two descriptors are created instead for it.
    if (!create_storage_buffer_descriptor)
    {
        return;
    }

    descriptor_range = descriptor_index_to_address(buffer->buffer_view->bindless_index, 1u, false);
    vkWriteResourceDescriptorsEXT(m_device, 1, &descriptor_info, &descriptor_range);
}

void Vulkan_Graphics_Device::create_image_descriptors(Vulkan_Image* image, bool create_storage_image_descriptor)
{
    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image->image,
        .viewType = vulkan_cast<VkImageViewType>(image->primary_view_type),
        .format = vulkan_cast<VkFormat>(image->format),
        .components = {}, // identity
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = 0,
            .layerCount = VK_REMAINING_ARRAY_LAYERS
        }
    };
    VkImageDescriptorInfoEXT image_descriptor_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_EXT,
        .pNext = nullptr,
        .pView = &image_view_create_info
    };
    VkResourceDescriptorInfoEXT descriptor_info = {
        .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
        .pNext = nullptr,
        .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .data = {
            .pImage = &image_descriptor_info
        }
    };
    auto descriptor_range = descriptor_index_to_address(image->image_view->bindless_index, 0u, false);
    vkWriteResourceDescriptorsEXT(m_device, 1, &descriptor_info, &descriptor_range);

    if (!create_storage_image_descriptor)
    {
        return;
    }

    descriptor_info.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptor_range = descriptor_index_to_address(image->image_view->bindless_index, 1u, false);
    vkWriteResourceDescriptorsEXT(m_device, 1, &descriptor_info, &descriptor_range);
}

VkHostAddressRangeEXT Vulkan_Graphics_Device::descriptor_index_to_address(uint32_t index, uint32_t offset, bool is_sampler)
{
    Descriptor_Heap descriptor_heap = is_sampler ? m_sampler_descriptor_heap : m_resource_descriptor_heap;
    return {
        .address = static_cast<void*>(&static_cast<char*>(descriptor_heap.data)[(index + offset) * descriptor_heap.alignment]),
        .size = descriptor_heap.alignment // TODO: always use max size?
    };
}
}
