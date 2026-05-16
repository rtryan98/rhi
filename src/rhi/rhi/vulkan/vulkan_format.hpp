#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include "rhi/image_format.hpp"
#include <vulkan/vulkan.h>

namespace rhi::vulkan
{
inline VkFormat translate_format(Image_Format image_format)
{
    switch (image_format)
    {
    case Image_Format::Undefined: return VK_FORMAT_UNDEFINED;
    case Image_Format::R8_UNORM: return VK_FORMAT_R8_UNORM;
    case Image_Format::R8_SNORM: return VK_FORMAT_R8_SNORM;
    case Image_Format::R8_UINT:  return VK_FORMAT_R8_UINT;
    case Image_Format::R8_SINT:  return VK_FORMAT_R8_SINT;
    case Image_Format::R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
    case Image_Format::R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
    case Image_Format::R8G8_UINT:  return VK_FORMAT_R8G8_UINT;
    case Image_Format::R8G8_SINT:  return VK_FORMAT_R8G8_SINT;
    case Image_Format::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
    case Image_Format::R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
    case Image_Format::R8G8B8A8_UINT:  return VK_FORMAT_R8G8B8A8_UINT;
    case Image_Format::R8G8B8A8_SINT:  return VK_FORMAT_R8G8B8A8_SINT;
    case Image_Format::R8G8B8A8_SRGB:  return VK_FORMAT_R8G8B8A8_SRGB;
    case Image_Format::B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
    case Image_Format::B8G8R8A8_SNORM: return VK_FORMAT_B8G8R8A8_SNORM;
    case Image_Format::B8G8R8A8_UINT:  return VK_FORMAT_B8G8R8A8_UINT;
    case Image_Format::B8G8R8A8_SINT:  return VK_FORMAT_B8G8R8A8_SINT;
    case Image_Format::B8G8R8A8_SRGB:  return VK_FORMAT_B8G8R8A8_SRGB;
    case Image_Format::A2R10G10B10_UNORM_PACK32: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case Image_Format::R16_UNORM:  return VK_FORMAT_R16_UNORM;
    case Image_Format::R16_SNORM:  return VK_FORMAT_R16_SNORM;
    case Image_Format::R16_UINT:   return VK_FORMAT_R16_UINT;
    case Image_Format::R16_SINT:   return VK_FORMAT_R16_SINT;
    case Image_Format::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;
    case Image_Format::R16G16_UNORM:  return VK_FORMAT_R16G16_UNORM;
    case Image_Format::R16G16_SNORM:  return VK_FORMAT_R16G16_SNORM;
    case Image_Format::R16G16_UINT:   return VK_FORMAT_R16G16_UINT;
    case Image_Format::R16G16_SINT:   return VK_FORMAT_R16G16_SINT;
    case Image_Format::R16G16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;
    case Image_Format::R16G16B16A16_UNORM:  return VK_FORMAT_R16G16B16A16_UNORM;
    case Image_Format::R16G16B16A16_SNORM:  return VK_FORMAT_R16G16B16A16_SNORM;
    case Image_Format::R16G16B16A16_UINT:   return VK_FORMAT_R16G16B16A16_UINT;
    case Image_Format::R16G16B16A16_SINT:   return VK_FORMAT_R16G16B16A16_SINT;
    case Image_Format::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case Image_Format::R32_UINT:   return VK_FORMAT_R32_UINT;
    case Image_Format::R32_SINT:   return VK_FORMAT_R32_SINT;
    case Image_Format::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;
    case Image_Format::R32G32_UINT:   return VK_FORMAT_R32G32_UINT;
    case Image_Format::R32G32_SINT:   return VK_FORMAT_R32G32_SINT;
    case Image_Format::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
    case Image_Format::R32G32B32_UINT:   return VK_FORMAT_R32G32B32_UINT;
    case Image_Format::R32G32B32_SINT:   return VK_FORMAT_R32G32B32_SINT;
    case Image_Format::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
    case Image_Format::R32G32B32A32_UINT:   return VK_FORMAT_R32G32B32A32_UINT;;
    case Image_Format::R32G32B32A32_SINT:   return VK_FORMAT_R32G32B32A32_SINT;;
    case Image_Format::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;;
    case Image_Format::B10G11R11_UFLOAT_PACK32: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case Image_Format::E5B9G9R9_UFLOAT_PACK32: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    case Image_Format::D16_UNORM: return VK_FORMAT_D16_UNORM;
    case Image_Format::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
    case Image_Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
    case Image_Format::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case Image_Format::BC1_RGB_UNORM_BLOCK: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    case Image_Format::BC1_RGB_SRGB_BLOCK: return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
    case Image_Format::BC1_RGBA_UNORM_BLOCK: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    case Image_Format::BC1_RGBA_SRGB_BLOCK: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    case Image_Format::BC2_UNORM_BLOCK: return VK_FORMAT_BC2_UNORM_BLOCK;
    case Image_Format::BC2_SRGB_BLOCK: return VK_FORMAT_BC2_SRGB_BLOCK;
    case Image_Format::BC3_UNORM_BLOCK: return VK_FORMAT_BC3_UNORM_BLOCK;
    case Image_Format::BC3_SRGB_BLOCK: return VK_FORMAT_BC3_SRGB_BLOCK;
    case Image_Format::BC4_UNORM_BLOCK: return VK_FORMAT_BC4_UNORM_BLOCK;
    case Image_Format::BC4_SNORM_BLOCK: return VK_FORMAT_BC4_SNORM_BLOCK;
    case Image_Format::BC5_UNORM_BLOCK: return VK_FORMAT_BC5_UNORM_BLOCK;
    case Image_Format::BC5_SNORM_BLOCK: return VK_FORMAT_BC5_SNORM_BLOCK;
    case Image_Format::BC6H_UFLOAT_BLOCK: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
    case Image_Format::BC6H_SFLOAT_BLOCK: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
    case Image_Format::BC7_UNORM_BLOCK: return VK_FORMAT_BC7_UNORM_BLOCK;
    case Image_Format::BC7_SRGB_BLOCK: return VK_FORMAT_BC7_SRGB_BLOCK;
    default:
        std::unreachable();
    }
}

// TODO: finish up implementation
inline Image_Format translate_vkformat_to_image_format(VkFormat vk_format)
{
    switch (vk_format)
    {
    case VK_FORMAT_UNDEFINED:                return Image_Format::Undefined;
    case VK_FORMAT_R8_UNORM:                 return Image_Format::R8_UNORM;
    case VK_FORMAT_R8_SNORM:                 return Image_Format::R8_SNORM;
    case VK_FORMAT_R8_UINT:                  return Image_Format::R8_UINT;
    case VK_FORMAT_R8_SINT:                  return Image_Format::R8_SINT;
    case VK_FORMAT_R8G8_UNORM:               return Image_Format::R8G8_UNORM;
    case VK_FORMAT_R8G8_SNORM:               return Image_Format::R8G8_SNORM;
    case VK_FORMAT_R8G8_UINT:                return Image_Format::R8G8_UINT;
    case VK_FORMAT_R8G8_SINT:                return Image_Format::R8G8_SINT;
    case VK_FORMAT_R8G8B8A8_UNORM:           return Image_Format::R8G8B8A8_UNORM;
    case VK_FORMAT_R8G8B8A8_SNORM:           return Image_Format::R8G8B8A8_SNORM;
    case VK_FORMAT_R8G8B8A8_UINT:            return Image_Format::R8G8B8A8_UINT;
    case VK_FORMAT_R8G8B8A8_SINT:            return Image_Format::R8G8B8A8_SINT;
    case VK_FORMAT_R8G8B8A8_SRGB:            return Image_Format::R8G8B8A8_SRGB;
    case VK_FORMAT_B8G8R8A8_UNORM:           return Image_Format::B8G8R8A8_UNORM;
    case VK_FORMAT_B8G8R8A8_SNORM:           return Image_Format::B8G8R8A8_SNORM;
    case VK_FORMAT_B8G8R8A8_UINT:            return Image_Format::B8G8R8A8_UINT;
    case VK_FORMAT_B8G8R8A8_SINT:            return Image_Format::B8G8R8A8_SINT;
    case VK_FORMAT_B8G8R8A8_SRGB:            return Image_Format::B8G8R8A8_SRGB;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return Image_Format::A2R10G10B10_UNORM_PACK32;
    case VK_FORMAT_R16_UNORM:                return Image_Format::R16_UNORM;
    case VK_FORMAT_R16_SNORM:                return Image_Format::R16_SNORM;
    case VK_FORMAT_R16_UINT:                 return Image_Format::R16_UINT;
    case VK_FORMAT_R16_SINT:                 return Image_Format::R16_SINT;
    case VK_FORMAT_R16_SFLOAT:               return Image_Format::R16_SFLOAT;
    case VK_FORMAT_R16G16_UNORM:             return Image_Format::R16G16_UNORM;
    case VK_FORMAT_R16G16_SNORM:             return Image_Format::R16G16_SNORM;
    case VK_FORMAT_R16G16_UINT:              return Image_Format::R16G16_UINT;
    case VK_FORMAT_R16G16_SINT:              return Image_Format::R16G16_SINT;
    case VK_FORMAT_R16G16_SFLOAT:            return Image_Format::R16G16_SFLOAT;
    case VK_FORMAT_R16G16B16A16_UNORM:       return Image_Format::R16G16B16A16_UNORM;
    case VK_FORMAT_R16G16B16A16_SNORM:       return Image_Format::R16G16B16A16_SNORM;
    case VK_FORMAT_R16G16B16A16_UINT:        return Image_Format::R16G16B16A16_UINT;
    case VK_FORMAT_R16G16B16A16_SINT:        return Image_Format::R16G16B16A16_SINT;
    case VK_FORMAT_R16G16B16A16_SFLOAT:      return Image_Format::R16G16B16A16_SFLOAT;
    case VK_FORMAT_R32_UINT:                 return Image_Format::R32_UINT;
    case VK_FORMAT_R32_SINT:                 return Image_Format::R32_SINT;
    case VK_FORMAT_R32_SFLOAT:               return Image_Format::R32_SFLOAT;
    case VK_FORMAT_R32G32_UINT:              return Image_Format::R32G32_UINT;
    case VK_FORMAT_R32G32_SINT:              return Image_Format::R32G32_SINT;
    case VK_FORMAT_R32G32_SFLOAT:            return Image_Format::R32G32_SFLOAT;
    case VK_FORMAT_R32G32B32_UINT:           return Image_Format::R32G32B32_UINT;
    case VK_FORMAT_R32G32B32_SINT:           return Image_Format::R32G32B32_SINT;
    case VK_FORMAT_R32G32B32_SFLOAT:         return Image_Format::R32G32B32_SFLOAT;
    case VK_FORMAT_R32G32B32A32_UINT:        return Image_Format::R32G32B32A32_UINT;
    case VK_FORMAT_R32G32B32A32_SINT:        return Image_Format::R32G32B32A32_SINT;
    case VK_FORMAT_R32G32B32A32_SFLOAT:      return Image_Format::R32G32B32A32_SFLOAT;
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:  return Image_Format::B10G11R11_UFLOAT_PACK32;
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:   return Image_Format::E5B9G9R9_UFLOAT_PACK32;
    case VK_FORMAT_D16_UNORM:                return Image_Format::D16_UNORM;
    case VK_FORMAT_D32_SFLOAT:               return Image_Format::D32_SFLOAT;
    case VK_FORMAT_D24_UNORM_S8_UINT:        return Image_Format::D24_UNORM_S8_UINT;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:       return Image_Format::D32_SFLOAT_S8_UINT;
    default:
        return Image_Format::Undefined;
    }
}
}
