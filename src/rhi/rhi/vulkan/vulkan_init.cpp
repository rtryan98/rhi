#include "rhi/vulkan/vulkan_init.hpp"

#include "rhi/resource.hpp"

#include <array>
#include <vector>
#include <ranges>

namespace rhi::vulkan
{
VkInstance create_instance(bool enable_validation_layers, bool enable_gpu_validation)
{
    std::vector<const char*> enabled_layers;
    if (enable_validation_layers)
    {
        enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    std::vector<const char*> enabled_extensions;
    enabled_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    enabled_extensions.push_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
    enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    enabled_extensions.push_back(VK_KHR_SURFACE_MAINTENANCE_1_EXTENSION_NAME);
    enabled_extensions.push_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
    enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = nullptr,
        .applicationVersion = VK_MAKE_VERSION(0,0,0),
        .pEngineName = nullptr,
        .engineVersion = VK_MAKE_VERSION(0,0,0),
        .apiVersion = VK_API_VERSION_1_4
    };
    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = static_cast<uint32_t>(enabled_layers.size()),
        .ppEnabledLayerNames = enabled_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()),
        .ppEnabledExtensionNames = enabled_extensions.data()
    };
    VkInstance instance = VK_NULL_HANDLE;
    vkCreateInstance(&instance_create_info, nullptr, &instance);
    volkLoadInstance(instance);
    return instance;
}

VkPhysicalDevice select_physical_device(VkInstance instance)
{
    std::vector<VkPhysicalDevice> physical_devices;

    {
        uint32_t physical_device_count = 0u;
        vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
        physical_devices.resize(physical_device_count, VK_NULL_HANDLE);
        vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());
    }

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
        .samplerMirrorClampToEdge = VK_TRUE,
        .drawIndirectCount = VK_TRUE,
        .shaderFloat16 = VK_TRUE,
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
        .timelineSemaphore = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE
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

    // Depth Clip
    constexpr static VkPhysicalDeviceDepthClipEnableFeaturesEXT REQUIRED_DEPTH_CLIP_ENABLE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT,
        .pNext = nullptr,
        .depthClipEnable = VK_TRUE
    };

    return physical_devices.at(0);
}

Queue_Infos get_queue_infos(VkPhysicalDevice physical_device)
{
    Queue_Infos queue_infos = {};

    std::vector<VkQueueFamilyProperties2> queue_family_properties_list;
    {
        uint32_t queue_family_property_count = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queue_family_property_count, nullptr);
        queue_family_properties_list.resize(queue_family_property_count);
        for (auto& queue_family_properties : queue_family_properties_list)
        {
            queue_family_properties.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        }
        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queue_family_property_count, queue_family_properties_list.data());

        for (const auto& [index, queue_family_properties] : queue_family_properties_list | std::views::enumerate)
        {
            const auto& props = queue_family_properties.queueFamilyProperties;
            if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) // Can only be graphics queue
            {
                queue_infos.graphics_queue_index = static_cast<uint32_t>(index);
            }
            else if ((props.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) // If no graphics bit, can only be compute queue
            {
                queue_infos.compute_queue_index = static_cast<uint32_t>(index);
            }
            else if ((props.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) != 0) // If no graphics nor compute bit, query video queues
            {
                queue_infos.video_decode_queue_index = static_cast<uint32_t>(index);
            }
            else if ((props.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) != 0)
            {
                queue_infos.video_encode_queue_index = static_cast<uint32_t>(index);
            }
            else if ((props.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0
                && (props.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) == 0) // Don't accidentally use NV_OPTICAL_FLOW queue for copy queue
            {
                queue_infos.copy_queue_index = static_cast<uint32_t>(index);
            }
        }
    }

    return queue_infos;
}

VkDevice create_device(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceFeatures2 REQUIRED_VK10_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = nullptr,
        .features = {
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
        }
    };
    VkPhysicalDeviceVulkan11Features REQUIRED_VK11_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &REQUIRED_VK10_FEATURES,
        .storageBuffer16BitAccess = VK_TRUE,
        .uniformAndStorageBuffer16BitAccess = VK_TRUE,
        .shaderDrawParameters = VK_TRUE
    };
    VkPhysicalDeviceVulkan12Features REQUIRED_VK12_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &REQUIRED_VK11_FEATURES,
        .samplerMirrorClampToEdge = VK_TRUE,
        .drawIndirectCount = VK_TRUE,
        .shaderFloat16 = VK_TRUE,
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
        .timelineSemaphore = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE
    };
    VkPhysicalDeviceVulkan13Features REQUIRED_VK13_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &REQUIRED_VK12_FEATURES,
        .descriptorBindingInlineUniformBlockUpdateAfterBind = VK_TRUE,
        .shaderDemoteToHelperInvocation = VK_TRUE, // Match D3D12 behavior
        .shaderTerminateInvocation = VK_TRUE,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
        .maintenance4 = VK_TRUE
    };
    VkPhysicalDeviceVulkan14Features REQUIRED_VK14_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = &REQUIRED_VK13_FEATURES,
        .maintenance5 = VK_TRUE,
        .maintenance6 = VK_TRUE
    };
    VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT REQUIRED_MUTABLE_DESCRIPTOR_TYPE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT,
        .pNext = &REQUIRED_VK14_FEATURES,
        .mutableDescriptorType = VK_TRUE
    };
    VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR REQUIRED_SWAPCHAIN_MAINTENANCE1_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT,
        .pNext = &REQUIRED_MUTABLE_DESCRIPTOR_TYPE_FEATURES,
        .swapchainMaintenance1 = VK_TRUE
    };

    // Ray tracing
    VkPhysicalDeviceAccelerationStructureFeaturesKHR REQUIRED_ACCELERATION_STRUCTURE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
        .pNext = &REQUIRED_SWAPCHAIN_MAINTENANCE1_FEATURES,
        .accelerationStructure = VK_TRUE,
        .accelerationStructureCaptureReplay = VK_FALSE,
        .accelerationStructureIndirectBuild = VK_FALSE,
        .accelerationStructureHostCommands = VK_FALSE,
        .descriptorBindingAccelerationStructureUpdateAfterBind = VK_TRUE
    };
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR REQUIRED_RAY_TRACING_PIPELINE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
        .pNext = &REQUIRED_ACCELERATION_STRUCTURE_FEATURES,
        .rayTracingPipeline = VK_TRUE,
        .rayTracingPipelineShaderGroupHandleCaptureReplay = VK_FALSE,
        .rayTracingPipelineShaderGroupHandleCaptureReplayMixed = VK_FALSE,
        .rayTracingPipelineTraceRaysIndirect = VK_TRUE,
        .rayTraversalPrimitiveCulling = VK_TRUE
    };
    VkPhysicalDeviceRayQueryFeaturesKHR REQUIRED_RAY_QUERY_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
        .pNext = &REQUIRED_RAY_TRACING_PIPELINE_FEATURES,
        .rayQuery = VK_TRUE
    };
    VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR REQUIRED_RAY_TRACING_MAINTENANCE1_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR,
        .pNext = &REQUIRED_RAY_QUERY_FEATURES,
        .rayTracingMaintenance1 = VK_TRUE,
        .rayTracingPipelineTraceRaysIndirect2 = VK_FALSE
    };

    // Mesh shading
    VkPhysicalDeviceMeshShaderFeaturesEXT REQUIRED_MESH_SHADER_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
        .pNext = &REQUIRED_RAY_TRACING_MAINTENANCE1_FEATURES,
        .taskShader = VK_TRUE,
        .meshShader = VK_TRUE,
        .multiviewMeshShader = VK_FALSE,
        .primitiveFragmentShadingRateMeshShader = VK_FALSE,
        .meshShaderQueries = VK_TRUE
    };

    // Depth Clip
    VkPhysicalDeviceDepthClipEnableFeaturesEXT REQUIRED_DEPTH_CLIP_ENABLE_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT,
        .pNext = &REQUIRED_MESH_SHADER_FEATURES,
        .depthClipEnable = VK_TRUE
    };

    const auto extensions = std::to_array<const char*>({
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
        VK_EXT_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        VK_KHR_RAY_QUERY_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_MESH_SHADER_EXTENSION_NAME,
        VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME });

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<VkQueueFamilyOwnershipTransferPropertiesKHR> queue_family_ownership_transfer_properties_list;
    std::vector<VkQueueFamilyProperties2> queue_family_properties_list;
    {
        uint32_t queue_family_property_count = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queue_family_property_count, nullptr);
        queue_family_properties_list.resize(queue_family_property_count);
        queue_family_ownership_transfer_properties_list.resize(queue_family_property_count);
        for (auto i = 0; i < queue_family_property_count; ++i)
        {
            auto& queue_family_ownership_transfer_properties = queue_family_ownership_transfer_properties_list[i];
            queue_family_ownership_transfer_properties.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_OWNERSHIP_TRANSFER_PROPERTIES_KHR;

            auto& queue_family_properties = queue_family_properties_list[i];
            queue_family_properties.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            queue_family_properties.pNext = &queue_family_ownership_transfer_properties;
        }

        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queue_family_property_count, queue_family_properties_list.data());

        for (const auto& [index, queue_family_properties] : queue_family_properties_list | std::views::enumerate)
        {
            const auto& props = queue_family_properties.queueFamilyProperties;
            static float priority = 1.f;
            VkDeviceQueueCreateInfo queue_create_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = static_cast<uint32_t>(index),
                .queueCount = 1,
                .pQueuePriorities = &priority
            };

            if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) // Can only be graphics queue
            {
                queue_create_infos.push_back(queue_create_info);
            }
            else if ((props.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) // If no graphics bit, can only be compute queue
            {
                queue_create_infos.push_back(queue_create_info);
            }
            else if ((props.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) != 0) // If no graphics nor compute bit, query video queues
            {
                queue_create_infos.push_back(queue_create_info);
            }
            else if ((props.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) != 0)
            {
                queue_create_infos.push_back(queue_create_info);
            }
            else if ((props.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0
                && (props.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) == 0) // Don't accidentally use NV_OPTICAL_FLOW queue for copy queue
            {
                queue_create_infos.push_back(queue_create_info);
            }
        }
    }

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &REQUIRED_DEPTH_CLIP_ENABLE_FEATURES,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr
    };

    VkDevice device = VK_NULL_HANDLE;
    vkCreateDevice(physical_device, &device_create_info, nullptr, &device);
    volkLoadDevice(device);
    return device;
}

VkDescriptorSetLayout create_descriptor_set_layout(VkDevice device)
{
    const auto bindless_flags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
        | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT
        | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    auto binding_flags = std::to_array<VkDescriptorBindingFlags>({ bindless_flags, bindless_flags, bindless_flags });
    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = static_cast<uint32_t>(binding_flags.size()),
        .pBindingFlags = binding_flags.data()
    };

    auto mutable_descriptors = std::to_array<VkDescriptorType>({
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        });
    VkMutableDescriptorTypeListEXT mutable_descriptor_list = {
        .descriptorTypeCount = static_cast<uint32_t>(mutable_descriptors.size()),
        .pDescriptorTypes = mutable_descriptors.data()
    };
    VkMutableDescriptorTypeCreateInfoEXT mutable_descriptor_type_create_info = {
        .sType = VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT,
        .pNext = &binding_flags_create_info,
        .mutableDescriptorTypeListCount = 1,
        .pMutableDescriptorTypeLists = &mutable_descriptor_list
    };

    auto bindings = std::to_array<VkDescriptorSetLayoutBinding>({{
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
        .descriptorCount = 2 * MAX_RESOURCE_INDEX,
        .stageFlags = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr
        }, {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        .descriptorCount = 2 * MAX_RESOURCE_INDEX,
        .stageFlags = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr
        }, {
        .binding = 2,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .descriptorCount = MAX_SAMPLER_INDEX,
        .stageFlags = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr
        }});
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &mutable_descriptor_type_create_info,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

    VkDescriptorSetLayout result = VK_NULL_HANDLE;
    vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, nullptr, &result);
    return result;
}

VkPipelineLayout create_pipeline_layout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout, uint32_t push_constant_size)
{
    VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = push_constant_size
    };
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &push_constant_range
    };

    VkPipelineLayout result = VK_NULL_HANDLE;
    vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &result);
    return result;
}

VkDescriptorPool create_descriptor_pool(VkDevice device)
{
    auto mutable_descriptors = std::to_array<VkDescriptorType>({
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        });
    VkMutableDescriptorTypeListEXT mutable_descriptor_list = {
        .descriptorTypeCount = static_cast<uint32_t>(mutable_descriptors.size()),
        .pDescriptorTypes = mutable_descriptors.data()
    };
    VkMutableDescriptorTypeCreateInfoEXT mutable_descriptor_type_create_info = {
        .sType = VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT,
        .pNext = nullptr,
        .mutableDescriptorTypeListCount = 1,
        .pMutableDescriptorTypeLists = &mutable_descriptor_list
    };

    auto pool_sizes = std::to_array<VkDescriptorPoolSize>({
        {
            .type = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
            .descriptorCount = MAX_RESOURCE_INDEX * 2
        },
        {
            .type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
            .descriptorCount = MAX_RESOURCE_INDEX * 2
        },
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = MAX_SAMPLER_INDEX
        },
        });
    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = &mutable_descriptor_type_create_info,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data()
    };
    VkDescriptorPool result = VK_NULL_HANDLE;
    vkCreateDescriptorPool(device, &descriptor_pool_create_info, nullptr, &result);
    return result;
}

VkDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorSetLayout descriptor_set_layout, VkDescriptorPool descriptor_pool)
{
    VkDescriptorSetAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptor_set_layout
    };

    VkDescriptorSet result = VK_NULL_HANDLE;
    vkAllocateDescriptorSets(device, &allocate_info, &result);
    return result;
}
}
