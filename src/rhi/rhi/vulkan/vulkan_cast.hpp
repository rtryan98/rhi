#pragma once

namespace rhi::vulkan
{
template<typename VulkanType, typename BaseType>
VulkanType vulkan_cast(const BaseType base);
}
