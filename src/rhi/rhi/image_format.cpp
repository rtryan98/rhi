#include "rhi/image_format.hpp"

#include <array>

namespace rhi
{
Image_Format_Info get_image_format_info(std::string_view string_format) noexcept
{
    constexpr static auto IMAGE_FORMATS =
        std::to_array<std::pair<const std::string_view, const Image_Format>>(
            {
                {"Undefined", Image_Format::Undefined},
                {"R8_UNORM", Image_Format::R8_UNORM},
                {"R8_SNORM",Image_Format::R8_SNORM},
                {"R8_UINT", Image_Format::R8_UINT},
                {"R8_SINT", Image_Format::R8_SINT},
                {"R8G8_UNORM", Image_Format::R8G8_UNORM},
                {"R8G8_SNORM", Image_Format::R8G8_SNORM},
                {"R8G8_UINT", Image_Format::R8G8_UINT},
                {"R8G8_SINT", Image_Format::R8G8_SINT},
                {"R8G8B8A8_UNORM", Image_Format::R8G8B8A8_UNORM},
                {"R8G8B8A8_SNORM", Image_Format::R8G8B8A8_SNORM},
                {"R8G8B8A8_UINT", Image_Format::R8G8B8A8_UINT},
                {"R8G8B8A8_SINT", Image_Format::R8G8B8A8_SINT},
                {"R8G8B8A8_SRGB", Image_Format::R8G8B8A8_SRGB},
                {"B8G8R8A8_UNORM", Image_Format::B8G8R8A8_UNORM},
                {"B8G8R8A8_SNORM", Image_Format::B8G8R8A8_SNORM},
                {"B8G8R8A8_UINT", Image_Format::B8G8R8A8_UINT},
                {"B8G8R8A8_SINT", Image_Format::B8G8R8A8_SINT},
                {"B8G8R8A8_SRGB", Image_Format::B8G8R8A8_SRGB},
                {"A2R10G10B10_UNORM_PACK32", Image_Format::A2R10G10B10_UNORM_PACK32},
                {"R16_UNORM", Image_Format::R16_UNORM},
                {"R16_SNORM", Image_Format::R16_SNORM},
                {"R16_UINT", Image_Format::R16_UINT},
                {"R16_SINT", Image_Format::R16_SINT},
                {"R16_SFLOAT", Image_Format::R16_SFLOAT},
                {"R16G16_UNORM", Image_Format::R16G16_UNORM},
                {"R16G16_SNORM", Image_Format::R16G16_SNORM},
                {"R16G16_UINT", Image_Format::R16G16_UINT},
                {"R16G16_SINT", Image_Format::R16G16_SINT},
                {"R16G16_SFLOAT", Image_Format::R16G16_SFLOAT},
                {"R16G16B16A16_UNORM", Image_Format::R16G16B16A16_UNORM},
                {"R16G16B16A16_SNORM", Image_Format::R16G16B16A16_SNORM},
                {"R16G16B16A16_UINT", Image_Format::R16G16B16A16_UINT},
                {"R16G16B16A16_SINT", Image_Format::R16G16B16A16_SINT},
                {"R16G16B16A16_SFLOAT", Image_Format::R16G16B16A16_SFLOAT},
                {"R32_UINT", Image_Format::R32_UINT},
                {"R32_SINT", Image_Format::R32_SINT},
                {"R32_SFLOAT", Image_Format::R32_SFLOAT},
                {"R32G32_UINT", Image_Format::R32G32_UINT},
                {"R32G32_SINT", Image_Format::R32G32_SINT},
                {"R32G32_SFLOAT", Image_Format::R32G32_SFLOAT},
                {"R32G32B32_UINT", Image_Format::R32G32B32_UINT},
                {"R32G32B32_SINT", Image_Format::R32G32B32_SINT},
                {"R32G32B32_SFLOAT", Image_Format::R32G32B32_SFLOAT},
                {"R32G32B32A32_UINT", Image_Format::R32G32B32A32_UINT},
                {"R32G32B32A32_SINT", Image_Format::R32G32B32A32_SINT},
                {"R32G32B32A32_SFLOAT", Image_Format::R32G32B32A32_SFLOAT},
                {"B10G11R11_UFLOAT_PACK32", Image_Format::B10G11R11_UFLOAT_PACK32},
                {"E5B9G9R9_UFLOAT_PACK32", Image_Format::E5B9G9R9_UFLOAT_PACK32},
                {"D16_UNORM", Image_Format::D16_UNORM},
                {"D32_SFLOAT", Image_Format::D32_SFLOAT},
                {"D24_UNORM_S8_UINT", Image_Format::D24_UNORM_S8_UINT},
                {"D32_SFLOAT_S8_UINT", Image_Format::D32_SFLOAT_S8_UINT},
                {"BC1_RGB_UNORM_BLOCK", Image_Format::BC1_RGB_UNORM_BLOCK},
                {"BC1_RGB_SRGB_BLOCK", Image_Format::BC1_RGB_SRGB_BLOCK},
                {"BC1_RGBA_UNORM_BLOCK", Image_Format::BC1_RGBA_UNORM_BLOCK},
                {"BC1_RGBA_SRGB_BLOCK", Image_Format::BC1_RGBA_SRGB_BLOCK},
                {"BC2_UNORM_BLOCK", Image_Format::BC2_UNORM_BLOCK},
                {"BC2_SRGB_BLOCK", Image_Format::BC2_SRGB_BLOCK},
                {"BC3_UNORM_BLOCK", Image_Format::BC3_UNORM_BLOCK},
                {"BC3_SRGB_BLOCK", Image_Format::BC3_SRGB_BLOCK},
                {"BC4_UNORM_BLOCK", Image_Format::BC4_UNORM_BLOCK},
                {"BC4_SNORM_BLOCK", Image_Format::BC4_SNORM_BLOCK},
                {"BC5_UNORM_BLOCK", Image_Format::BC5_UNORM_BLOCK},
                {"BC5_SNORM_BLOCK", Image_Format::BC5_SNORM_BLOCK},
                {"BC6H_UFLOAT_BLOCK", Image_Format::BC6H_UFLOAT_BLOCK},
                {"BC6H_SFLOAT_BLOCK", Image_Format::BC6H_SFLOAT_BLOCK},
                {"BC7_UNORM_BLOCK", Image_Format::BC7_UNORM_BLOCK},
                {"BC7_SRGB_BLOCK", Image_Format::BC7_SRGB_BLOCK}
            });
    for (const auto& [string_val, enum_val] : IMAGE_FORMATS)
    {
        if (string_val == string_format)
            return get_image_format_info(enum_val);
    }
    return get_image_format_info(Image_Format::Undefined);
}

Image_Format_Info get_image_format_info(Image_Format format) noexcept
{
    switch (format)
    {
    case Image_Format::Undefined:
        return {
            .format = Image_Format::Undefined,
            .bytes = 0,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8_UNORM:
        return {
            .format = Image_Format::R8_UNORM,
            .bytes = 1,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8_SNORM:
        return {
            .format = Image_Format::R8_SNORM,
            .bytes = 1,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8_UINT:
        return {
            .format = Image_Format::R8_UINT,
            .bytes = 1,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8_SINT:
        return {
            .format = Image_Format::R8_SINT,
            .bytes = 1,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8_UNORM:
        return {
            .format = Image_Format::R8G8_UNORM,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8_SNORM:
        return {
            .format = Image_Format::R8G8_SNORM,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8_UINT:
        return {
            .format = Image_Format::R8G8_UINT,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8_SINT:
        return {
            .format = Image_Format::R8G8_SINT,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8B8A8_UNORM:
        return {
            .format = Image_Format::R8G8B8A8_UNORM,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8B8A8_SNORM:
        return {
            .format = Image_Format::R8G8B8A8_SNORM,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8B8A8_UINT:
        return {
            .format = Image_Format::R8G8B8A8_UINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8B8A8_SINT:
        return {
            .format = Image_Format::R8G8B8A8_SINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R8G8B8A8_SRGB:
        return {
            .format = Image_Format::R8G8B8A8_SRGB,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::B8G8R8A8_UNORM:
        return {
            .format = Image_Format::B8G8R8A8_UNORM,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::B8G8R8A8_SNORM:
        return {
            .format = Image_Format::B8G8R8A8_SNORM,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::B8G8R8A8_UINT:
        return {
            .format = Image_Format::B8G8R8A8_UINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::B8G8R8A8_SINT:
        return {
            .format = Image_Format::B8G8R8A8_SINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::B8G8R8A8_SRGB:
        return {
            .format = Image_Format::B8G8R8A8_SRGB,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::A2R10G10B10_UNORM_PACK32:
        return {
            .format = Image_Format::A2R10G10B10_UNORM_PACK32,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16_UNORM:
        return {
            .format = Image_Format::R16_UNORM,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16_SNORM:
        return {
            .format = Image_Format::R16_SNORM,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16_UINT:
        return {
            .format = Image_Format::R16_UINT,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16_SINT:
        return {
            .format = Image_Format::R16_SINT,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16_SFLOAT:
        return {
            .format = Image_Format::R16_SFLOAT,
            .bytes = 2,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16_UNORM:
        return {
            .format = Image_Format::R16G16_UNORM,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16_SNORM:
        return {
            .format = Image_Format::R16G16_SNORM,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16_UINT:
        return {
            .format = Image_Format::R16G16_UINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16_SINT:
        return {
            .format = Image_Format::R16G16_SINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16_SFLOAT:
        return {
            .format = Image_Format::R16G16_SFLOAT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16B16A16_UNORM:
        return {
            .format = Image_Format::R16G16B16A16_UNORM,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16B16A16_SNORM:
        return {
            .format = Image_Format::R16G16B16A16_SNORM,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16B16A16_UINT:
        return {
            .format = Image_Format::R16G16B16A16_UINT,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16B16A16_SINT:
        return {
            .format = Image_Format::R16G16B16A16_SINT,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R16G16B16A16_SFLOAT:
        return {
            .format = Image_Format::R16G16B16A16_SFLOAT,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32_UINT:
        return {
            .format = Image_Format::R32_UINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32_SINT:
        return {
            .format = Image_Format::R32_SINT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32_SFLOAT:
        return {
            .format = Image_Format::R32_SFLOAT,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32_UINT:
        return {
            .format = Image_Format::R32G32_UINT,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32_SINT:
        return {
            .format = Image_Format::R32G32_SINT,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32_SFLOAT:
        return {
            .format = Image_Format::R32G32_SFLOAT,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32B32_UINT:
        return {
            .format = Image_Format::R32G32B32_UINT,
            .bytes = 12,
            .is_depth = false,
            .is_stencil = false // TODO: ?
        };
    case Image_Format::R32G32B32_SINT:
        return {
            .format = Image_Format::R32G32B32_SINT,
            .bytes = 12,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32B32_SFLOAT:
        return {
            .format = Image_Format::R32G32B32_SFLOAT,
            .bytes = 12,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32B32A32_UINT:
        return {
            .format = Image_Format::R32G32B32A32_UINT,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32B32A32_SINT:
        return {
            .format = Image_Format::R32G32B32A32_SINT,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::R32G32B32A32_SFLOAT:
        return {
            .format = Image_Format::R32G32B32A32_SFLOAT,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::B10G11R11_UFLOAT_PACK32:
        return {
            .format = Image_Format::B10G11R11_UFLOAT_PACK32,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::E5B9G9R9_UFLOAT_PACK32:
        return {
            .format = Image_Format::E5B9G9R9_UFLOAT_PACK32,
            .bytes = 4,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::D16_UNORM:
        return {
            .format = Image_Format::D16_UNORM,
            .bytes = 2,
            .is_depth = true,
            .is_stencil = false
        };
    case Image_Format::D32_SFLOAT:
        return {
            .format = Image_Format::D32_SFLOAT,
            .bytes = 4,
            .is_depth = true,
            .is_stencil = false
        };
    case Image_Format::D24_UNORM_S8_UINT:
        return {
            .format = Image_Format::D24_UNORM_S8_UINT,
            .bytes = 4,
            .is_depth = true,
            .is_stencil = true
        };
    case Image_Format::D32_SFLOAT_S8_UINT:
        return {
            .format = Image_Format::D32_SFLOAT_S8_UINT,
            .bytes = 8,
            .is_depth = true,
            .is_stencil = true
        };
    case Image_Format::BC1_RGB_UNORM_BLOCK:
        return {
            .format = Image_Format::BC1_RGB_UNORM_BLOCK,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC1_RGB_SRGB_BLOCK:
        return {
            .format = Image_Format::BC1_RGB_SRGB_BLOCK,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC1_RGBA_UNORM_BLOCK:
        return {
            .format = Image_Format::BC1_RGBA_UNORM_BLOCK,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC1_RGBA_SRGB_BLOCK:
        return {
            .format = Image_Format::BC1_RGBA_SRGB_BLOCK,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC2_UNORM_BLOCK:
        return {
            .format = Image_Format::BC2_UNORM_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC2_SRGB_BLOCK:
        return {
            .format = Image_Format::BC2_SRGB_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC3_UNORM_BLOCK:
        return {
            .format = Image_Format::BC3_UNORM_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC3_SRGB_BLOCK:
        return {
            .format = Image_Format::BC3_SRGB_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC4_UNORM_BLOCK:
        return {
            .format = Image_Format::BC4_UNORM_BLOCK,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC4_SNORM_BLOCK:
        return {
            .format = Image_Format::BC4_SNORM_BLOCK,
            .bytes = 8,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC5_UNORM_BLOCK:
        return {
            .format = Image_Format::BC5_UNORM_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC5_SNORM_BLOCK:
        return {
            .format = Image_Format::BC5_SNORM_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC6H_UFLOAT_BLOCK:
        return {
            .format = Image_Format::BC6H_UFLOAT_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC6H_SFLOAT_BLOCK:
        return {
            .format = Image_Format::BC6H_SFLOAT_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC7_UNORM_BLOCK:
        return {
            .format = Image_Format::BC7_UNORM_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    case Image_Format::BC7_SRGB_BLOCK:
        return {
            .format = Image_Format::BC7_SRGB_BLOCK,
            .bytes = 16,
            .is_depth = false,
            .is_stencil = false
        };
    default:
        std::unreachable();
    }
}
}
