#include "rhi/resource.hpp"

namespace rhi
{
Image_Format_Info get_image_format_info(Image_Format format) noexcept
{
    switch (format)
    {
    case Image_Format::Undefined:
        return {};
    case Image_Format::R8_UNORM:
        return {
            .bytes = 1
        };
    case Image_Format::R8_SNORM:
        return {
            .bytes = 1
        };
    case Image_Format::R8_UINT:
        return {
            .bytes = 1
        };
    case Image_Format::R8_SINT:
        return {
            .bytes = 1
        };
    case Image_Format::R8G8_UNORM:
        return {
            .bytes = 2
        };
    case Image_Format::R8G8_SNORM:
        return {
            .bytes = 2
        };
    case Image_Format::R8G8_UINT:
        return {
            .bytes = 2
        };
    case Image_Format::R8G8_SINT:
        return {
            .bytes = 2
        };
    case Image_Format::R8G8B8A8_UNORM:
        return {
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_SNORM:
        return {
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_UINT:
        return {
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_SINT:
        return {
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_SRGB:
        return {
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_UNORM:
        return {
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_SNORM:
        return {
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_UINT:
        return {
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_SINT:
        return {
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_SRGB:
        return {
            .bytes = 4
        };
    case Image_Format::A2R10G10B10_UNORM_PACK32:
        return {
            .bytes = 4
        };
    case Image_Format::R16_UNORM:
        return {
            .bytes = 2
        };
    case Image_Format::R16_SNORM:
        return {
            .bytes = 2
        };
    case Image_Format::R16_UINT:
        return {
            .bytes = 2
        };
    case Image_Format::R16_SINT:
        return {
            .bytes = 2
        };
    case Image_Format::R16_SFLOAT:
        return {
            .bytes = 2
        };
    case Image_Format::R16G16_UNORM:
        return {
            .bytes = 4
        };
    case Image_Format::R16G16_SNORM:
        return {
            .bytes = 4
        };
    case Image_Format::R16G16_UINT:
        return {
            .bytes = 4
        };
    case Image_Format::R16G16_SINT:
        return {
            .bytes = 4
        };
    case Image_Format::R16G16_SFLOAT:
        return {
            .bytes = 4
        };
    case Image_Format::R16G16B16A16_UNORM:
        return {
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_SNORM:
        return {
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_UINT:
        return {
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_SINT:
        return {
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_SFLOAT:
        return {
            .bytes = 8
        };
    case Image_Format::R32_UINT:
        return {
            .bytes = 4
        };
    case Image_Format::R32_SINT:
        return {
            .bytes = 4
        };
    case Image_Format::R32_SFLOAT:
        return {
            .bytes = 4
        };
    case Image_Format::R32G32_UINT:
        return {
            .bytes = 8
        };
    case Image_Format::R32G32_SINT:
        return {
            .bytes = 8
        };
    case Image_Format::R32G32_SFLOAT:
        return {
            .bytes = 8
        };
    case Image_Format::R32G32B32A32_UINT:
        return {
            .bytes = 16
        };
    case Image_Format::R32G32B32A32_SINT:
        return {
            .bytes = 16
        };
    case Image_Format::R32G32B32A32_SFLOAT:
        return {
            .bytes = 16
        };
    case Image_Format::B10G11R11_UFLOAT_PACK32:
        return {
            .bytes = 4
        };
    case Image_Format::E5B9G9R9_UFLOAT_PACK32:
        return {
            .bytes = 4
        };
    case Image_Format::D16_UNORM:
        return {
            .bytes = 2
        };
    case Image_Format::D32_SFLOAT:
        return {
            .bytes = 4
        };
    case Image_Format::D24_UNORM_S8_UINT:
        return {
            .bytes = 4
        };
    case Image_Format::D32_SFLOAT_S8_UINT:
        return {
            .bytes = 8
        };
    case Image_Format::BC1_RGB_UNORM_BLOCK:
        return {}; // TODO: implement for BCn compression
    case Image_Format::BC1_RGB_SRGB_BLOCK:
        return {};
    case Image_Format::BC1_RGBA_UNORM_BLOCK:
        return {};
    case Image_Format::BC1_RGBA_SRGB_BLOCK:
        return {};
    case Image_Format::BC2_UNORM_BLOCK:
        return {};
    case Image_Format::BC2_SRGB_BLOCK:
        return {};
    case Image_Format::BC3_UNORM_BLOCK:
        return {};
    case Image_Format::BC3_SRGB_BLOCK:
        return {};
    case Image_Format::BC4_UNORM_BLOCK:
        return {};
    case Image_Format::BC4_SNORM_BLOCK:
        return {};
    case Image_Format::BC5_UNORM_BLOCK:
        return {};
    case Image_Format::BC5_SNORM_BLOCK:
        return {};
    case Image_Format::BC6H_UFLOAT_BLOCK:
        return {};
    case Image_Format::BC6H_SFLOAT_BLOCK:
        return {};
    case Image_Format::BC7_UNORM_BLOCK:
        return {};
    case Image_Format::BC7_SRGB_BLOCK:
        return {};
    default:
        return {};
    }
}
}
