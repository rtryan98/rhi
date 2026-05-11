#include "rhi/d3d12/d3d12_cast.hpp"

#include "rhi/d3d12/d3d12_resource.hpp"

namespace rhi::d3d12
{
template<>
DXGI_FORMAT d3d12_cast<DXGI_FORMAT, Image_Format>(const Image_Format image_format)
{
    return translate_format(image_format);
}

template<>
D3D12_SRV_DIMENSION d3d12_cast<D3D12_SRV_DIMENSION, Image_View_Type>(const Image_View_Type type)
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

template<>
D3D12_UAV_DIMENSION d3d12_cast<D3D12_UAV_DIMENSION, Image_View_Type>(const Image_View_Type type)
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

template<>
D3D12_RTV_DIMENSION d3d12_cast<D3D12_RTV_DIMENSION, Image_View_Type>(const Image_View_Type type)
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

template<>
D3D12_DSV_DIMENSION d3d12_cast<D3D12_DSV_DIMENSION, Image_View_Type>(const Image_View_Type type)
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
}
