#include "rhi/vulkan/vulkan_cast.hpp"

#include "rhi/vulkan/vulkan_format.hpp"
#include "rhi/resource.hpp"

#include <vulkan/vulkan.h>

namespace rhi::vulkan
{
template<>
VkFormat vulkan_cast<VkFormat, Image_Format>(const Image_Format image_format)
{
    return translate_format(image_format);
}

template<>
VkImageType vulkan_cast<VkImageType>(const Image_View_Type image_view_type)
{
    auto image_type = VK_IMAGE_TYPE_1D;
    switch (image_view_type)
    {
    case Image_View_Type::Texture_2D:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS_Array:
        image_type = VK_IMAGE_TYPE_2D;
        break;
    case Image_View_Type::Texture_3D:
        image_type = VK_IMAGE_TYPE_3D;
        break;
    default:
        break;
    }
    return image_type;
}

template<>
VkImageViewType vulkan_cast<VkImageViewType>(const Image_View_Type image_view_type)
{
    switch (image_view_type)
    {
    case Image_View_Type::Texture_1D:
        return VK_IMAGE_VIEW_TYPE_1D;
    case Image_View_Type::Texture_1D_Array:
        return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case Image_View_Type::Texture_2D:
        return VK_IMAGE_VIEW_TYPE_2D;
    case Image_View_Type::Texture_2D_Array:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case Image_View_Type::Texture_2D_MS:
        return VK_IMAGE_VIEW_TYPE_2D;
    case Image_View_Type::Texture_Cube:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case Image_View_Type::Texture_Cube_Array:
        return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    case Image_View_Type::Texture_2D_MS_Array:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case Image_View_Type::Texture_3D:
        return VK_IMAGE_VIEW_TYPE_3D;
    default:
        return VK_IMAGE_VIEW_TYPE_2D;
    }
}

template<>
VkImageUsageFlags vulkan_cast<VkImageUsageFlags>(const Image_Usage image_usage)
{
    VkImageUsageFlags result = 0;

    result |= (image_usage & Image_Usage::Sampled) == Image_Usage::Sampled
        ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
    result |= (image_usage & Image_Usage::Unordered_Access) == Image_Usage::Unordered_Access
        ? VK_IMAGE_USAGE_STORAGE_BIT : 0;
    result |= (image_usage & Image_Usage::Color_Attachment) == Image_Usage::Color_Attachment
        ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
    result |= (image_usage & Image_Usage::Depth_Stencil_Attachment) == Image_Usage::Depth_Stencil_Attachment
        ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
    result |= (image_usage & Image_Usage::Shading_Rate_Attachment) == Image_Usage::Shading_Rate_Attachment
        ? VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR : 0;

    return result;
}

template<>
VkFilter vulkan_cast<VkFilter>(const Sampler_Filter sampler_filter)
{
    switch (sampler_filter)
    {
    case Sampler_Filter::Nearest:
        return VK_FILTER_NEAREST;
    default:
        return VK_FILTER_LINEAR;
    }
}

template<>
VkSamplerMipmapMode vulkan_cast<VkSamplerMipmapMode>(const Sampler_Filter sampler_filter)
{
    switch (sampler_filter)
    {
    case Sampler_Filter::Nearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

template<>
VkSamplerAddressMode vulkan_cast<VkSamplerAddressMode>(const Image_Sample_Address_Mode address_mode)
{
    switch (address_mode)
    {
    case Image_Sample_Address_Mode::Wrap:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case Image_Sample_Address_Mode::Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case Image_Sample_Address_Mode::Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case Image_Sample_Address_Mode::Border:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    default:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

template<>
VkCompareOp vulkan_cast<VkCompareOp>(const Comparison_Func comparison_func)
{
    switch (comparison_func)
    {
    case Comparison_Func::None:
        return VK_COMPARE_OP_NEVER;
    case Comparison_Func::Never:
        return VK_COMPARE_OP_NEVER;
    case Comparison_Func::Less:
        return VK_COMPARE_OP_LESS;
    case Comparison_Func::Equal:
        return VK_COMPARE_OP_EQUAL;
    case Comparison_Func::Less_Equal:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case Comparison_Func::Greater:
        return VK_COMPARE_OP_GREATER;
    case Comparison_Func::Not_Equal:
        return VK_COMPARE_OP_NOT_EQUAL;
    case Comparison_Func::Greater_Equal:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case Comparison_Func::Always:
        return VK_COMPARE_OP_ALWAYS;
    }
}
}
