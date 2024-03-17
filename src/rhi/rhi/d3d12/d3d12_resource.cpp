#pragma once

#include "rhi/d3d12/d3d12_resource.hpp"

namespace rhi::d3d12
{
DXGI_FORMAT translate_format(Image_Format format) noexcept
{
    switch (format)
    {
    case Image_Format::Undefined:
        return DXGI_FORMAT_UNKNOWN;
    case Image_Format::R8_UNORM:
        return DXGI_FORMAT_R8_UNORM;
    case Image_Format::R8_SNORM:
        return DXGI_FORMAT_R8_SNORM;
    case Image_Format::R8_UINT:
        return DXGI_FORMAT_R8_UINT;
    case Image_Format::R8_SINT:
        return DXGI_FORMAT_R8_SINT;
    case Image_Format::R8G8_UNORM:
        return DXGI_FORMAT_R8G8_UNORM;
    case Image_Format::R8G8_SNORM:
        return DXGI_FORMAT_R8G8_SNORM;
    case Image_Format::R8G8_UINT:
        return DXGI_FORMAT_R8G8_UINT;
    case Image_Format::R8G8_SINT:
        return DXGI_FORMAT_R8G8_SINT;
    case Image_Format::R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Image_Format::R8G8B8A8_SNORM:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    case Image_Format::R8G8B8A8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;
    case Image_Format::R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_SINT;
    case Image_Format::R8G8B8A8_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case Image_Format::A2R10G10B10_UNORM_PACK32:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case Image_Format::R16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
    case Image_Format::R16_SNORM:
        return DXGI_FORMAT_R16_SNORM;
    case Image_Format::R16_UINT:
        return DXGI_FORMAT_R16_UINT;
    case Image_Format::R16_SINT:
        return DXGI_FORMAT_R16_SINT;
    case Image_Format::R16_SFLOAT:
        return DXGI_FORMAT_R16_FLOAT;
    case Image_Format::R16G16_UNORM:
        return DXGI_FORMAT_R16G16_UNORM;
    case Image_Format::R16G16_SNORM:
        return DXGI_FORMAT_R16G16_SNORM;
    case Image_Format::R16G16_UINT:
        return DXGI_FORMAT_R16G16_UINT;
    case Image_Format::R16G16_SINT:
        return DXGI_FORMAT_R16G16_SINT;
    case Image_Format::R16G16_SFLOAT:
        return DXGI_FORMAT_R16G16_FLOAT;
    case Image_Format::R16G16B16A16_UNORM:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    case Image_Format::R16G16B16A16_SNORM:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
    case Image_Format::R16G16B16A16_UINT:
        return DXGI_FORMAT_R16G16B16A16_UINT;
    case Image_Format::R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_SINT;
    case Image_Format::R16G16B16A16_SFLOAT:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case Image_Format::R32_UINT:
        return DXGI_FORMAT_R32_UINT;
    case Image_Format::R32_SINT:
        return DXGI_FORMAT_R32_SINT;
    case Image_Format::R32_SFLOAT:
        return DXGI_FORMAT_R32_FLOAT;
    case Image_Format::R32G32_UINT:
        return DXGI_FORMAT_R32G32_UINT;
    case Image_Format::R32G32_SINT:
        return DXGI_FORMAT_R32G32_SINT;
    case Image_Format::R32G32_SFLOAT:
        return DXGI_FORMAT_R32G32_FLOAT;
    case Image_Format::R32G32B32A32_UINT:
        return DXGI_FORMAT_R32G32B32A32_UINT;
    case Image_Format::R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_SINT;
    case Image_Format::R32G32B32A32_SFLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case Image_Format::B10G11R11_UFLOAT_PACK32:
        return DXGI_FORMAT_R11G11B10_FLOAT;
    case Image_Format::E5B9G9R9_UFLOAT_PACK32:
        return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
    case Image_Format::D16_UNORM:
        return DXGI_FORMAT_D16_UNORM;
    case Image_Format::D32_SFLOAT:
        return DXGI_FORMAT_D32_FLOAT;
    case Image_Format::D24_UNORM_S8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case Image_Format::D32_SFLOAT_S8_UINT:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    case Image_Format::BC1_RGB_UNORM_BLOCK:
        return DXGI_FORMAT_BC1_UNORM;
    case Image_Format::BC1_RGB_SRGB_BLOCK:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
    case Image_Format::BC1_RGBA_UNORM_BLOCK:
        return DXGI_FORMAT_BC1_UNORM;
    case Image_Format::BC1_RGBA_SRGB_BLOCK:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
    case Image_Format::BC2_UNORM_BLOCK:
        return DXGI_FORMAT_BC2_UNORM;
    case Image_Format::BC2_SRGB_BLOCK:
        return DXGI_FORMAT_BC2_UNORM_SRGB;
    case Image_Format::BC3_UNORM_BLOCK:
        return DXGI_FORMAT_BC3_UNORM;
    case Image_Format::BC3_SRGB_BLOCK:
        return DXGI_FORMAT_BC3_UNORM_SRGB;
    case Image_Format::BC4_UNORM_BLOCK:
        return DXGI_FORMAT_BC4_UNORM;
    case Image_Format::BC4_SNORM_BLOCK:
        return DXGI_FORMAT_BC4_SNORM;
    case Image_Format::BC5_UNORM_BLOCK:
        return DXGI_FORMAT_BC5_UNORM;
    case Image_Format::BC5_SNORM_BLOCK:
        return DXGI_FORMAT_BC5_SNORM;
    case Image_Format::BC6H_UFLOAT_BLOCK:
        return DXGI_FORMAT_BC6H_UF16;
    case Image_Format::BC6H_SFLOAT_BLOCK:
        return DXGI_FORMAT_BC6H_SF16;
    case Image_Format::BC7_UNORM_BLOCK:
        return DXGI_FORMAT_BC7_UNORM;
    case Image_Format::BC7_SRGB_BLOCK:
        return DXGI_FORMAT_BC7_UNORM_SRGB;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

D3D12_SRV_DIMENSION translate_view_type_srv(Image_View_Type type) noexcept
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_SRV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_SRV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_SRV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
    case Image_View_Type::Texture_3D:
        return D3D12_SRV_DIMENSION_TEXTURE3D;
    case Image_View_Type::Texture_Cube:
        return D3D12_SRV_DIMENSION_TEXTURECUBE;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
    default:
        return D3D12_SRV_DIMENSION_UNKNOWN;
    }
}

D3D12_UAV_DIMENSION translate_view_type_uav(Image_View_Type type) noexcept
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_UAV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_UAV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_UAV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_UAV_DIMENSION_TEXTURE2DMSARRAY;
    case Image_View_Type::Texture_3D:
        return D3D12_UAV_DIMENSION_TEXTURE3D;
    case Image_View_Type::Texture_Cube:
        return D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    default:
        return D3D12_UAV_DIMENSION_UNKNOWN;
    }
}

D3D12_RTV_DIMENSION translate_view_type_rtv(Image_View_Type type) noexcept
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_RTV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_RTV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_RTV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
    case Image_View_Type::Texture_3D:
        return D3D12_RTV_DIMENSION_TEXTURE3D;
    case Image_View_Type::Texture_Cube:
        return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    default:
        return D3D12_RTV_DIMENSION_TEXTURE1D;
    }
}

D3D12_DSV_DIMENSION translate_view_type_dsv(Image_View_Type type) noexcept
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_DSV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_DSV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_DSV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
    // case Image_View_Type::Texture_3D:
    //     return D3D12_DSV_DIMENSION_UNKNOWN;
    case Image_View_Type::Texture_Cube:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    default:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    }
}

D3D12_FILTER_TYPE translate_filter_type(Sampler_Filter filter)
{
    if (filter == Sampler_Filter::Nearest)
    {
        return D3D12_FILTER_TYPE_POINT;
    }
    else
    {
        return D3D12_FILTER_TYPE_LINEAR;
    }
}

D3D12_FILTER_REDUCTION_TYPE translate_filter_reduction_type(Sampler_Reduction_Type reduction)
{
    if (reduction == Sampler_Reduction_Type::Standard)
    {
        return D3D12_FILTER_REDUCTION_TYPE_STANDARD;
    }
    else
    {
        return D3D12_FILTER_REDUCTION_TYPE_COMPARISON;
    }
}

#define RHI_D3D12_ENCODE_ANISOTROPIC_FILTER(mip, reduction) \
    ( ( D3D12_FILTER ) ( \
    D3D12_ANISOTROPIC_FILTERING_BIT | \
    D3D12_ENCODE_BASIC_FILTER(  D3D12_FILTER_TYPE_LINEAR, \
                                D3D12_FILTER_TYPE_LINEAR, \
                                mip, \
                                reduction ) ) )

D3D12_FILTER translate_filter(
    Sampler_Filter min,
    Sampler_Filter mag,
    Sampler_Filter mip,
    Sampler_Reduction_Type reduction,
    bool aniso) noexcept
{
    if (aniso)
    {
        return RHI_D3D12_ENCODE_ANISOTROPIC_FILTER(
            translate_filter_type(mip),
            translate_filter_reduction_type(reduction));
    }
    else
    {
        return D3D12_ENCODE_BASIC_FILTER(
            translate_filter_type(min),
            translate_filter_type(mag),
            translate_filter_type(mip),
            translate_filter_reduction_type(reduction));
    }
}
}
