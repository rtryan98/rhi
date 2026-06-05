#pragma once

#include <vulkan/vulkan.h>

#include "rhi/result.hpp"

namespace rhi::vulkan
{
inline Result translate_result(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS:
        return Result::Success;
    case VK_NOT_READY:
        return Result::Wait_Timeout;
    case VK_TIMEOUT:
        return Result::Wait_Timeout;
    case VK_EVENT_SET:
        return Result::Success;
    case VK_EVENT_RESET:
        return Result::Success;
    case VK_INCOMPLETE:
        return Result::Error_Unknown;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return Result::Error_Out_Of_Memory;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return Result::Error_Out_Of_Memory;
    case VK_ERROR_INITIALIZATION_FAILED:
        return Result::Error_Device_Lost;
    case VK_ERROR_DEVICE_LOST:
        return Result::Error_Device_Lost;
    case VK_ERROR_MEMORY_MAP_FAILED:
        return Result::Error_Out_Of_Memory;
    case VK_ERROR_TOO_MANY_OBJECTS:
        return Result::Error_Out_Of_Memory;
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return Result::Error_Invalid_Parameters;
    case VK_ERROR_FRAGMENTED_POOL:
        return Result::Error_Out_Of_Memory;
    case VK_ERROR_UNKNOWN:
        return Result::Error_Unknown;
    case VK_ERROR_VALIDATION_FAILED:
        return Result::Error_Unknown;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return Result::Error_Out_Of_Memory;
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return Result::Error_Invalid_Parameters;
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return Result::Error_Invalid_Parameters;
    case VK_ERROR_FRAGMENTATION:
        return Result::Error_Out_Of_Memory;
    case VK_PIPELINE_COMPILE_REQUIRED:
        return Result::Error_Invalid_Parameters;
    case VK_ERROR_NOT_PERMITTED:
        return Result::Error_Invalid_Parameters;
    case VK_ERROR_SURFACE_LOST_KHR:
        return Result::Error_Unknown;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return Result::Error_Unknown;
    case VK_SUBOPTIMAL_KHR:
        return Result::Wait_Timeout;
    case VK_ERROR_OUT_OF_DATE_KHR:
        return Result::Error_Unknown;
    default:
        return Result::Error_Unknown;
    }
}
}
