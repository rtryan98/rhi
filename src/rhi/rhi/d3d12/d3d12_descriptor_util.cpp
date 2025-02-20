#include "rhi/d3d12/d3d12_descriptor_util.hpp"

namespace rhi::d3d12
{
D3D12_SHADER_RESOURCE_VIEW_DESC make_raw_buffer_srv(uint32_t size)
{
    return {
        .Format = DXGI_FORMAT_R32_TYPELESS,
        .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Buffer = {
            .FirstElement = 0,
            .NumElements = size >> 2, // Counting number of 4 byte elements
            .StructureByteStride = 0,
            .Flags = D3D12_BUFFER_SRV_FLAG_RAW
        }
    };
}

D3D12_UNORDERED_ACCESS_VIEW_DESC make_raw_buffer_uav(uint32_t size)
{
    return {
        .Format = DXGI_FORMAT_R32_TYPELESS,
        .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
        .Buffer = {
            .FirstElement = 0,
            .NumElements = size >> 2, // Counting number of 4 byte elements
            .StructureByteStride = 0,
            .CounterOffsetInBytes = 0,
            .Flags = D3D12_BUFFER_UAV_FLAG_RAW
        }
    };
}

D3D12_SHADER_RESOURCE_VIEW_DESC make_full_texture_srv(
    DXGI_FORMAT format,
    D3D12_SRV_DIMENSION srv_dimension,
    uint32_t depth_or_array_layers)
{
    return make_texture_srv(
        format,
        srv_dimension,
        0,
        depth_or_array_layers,
        0,
        ~0u,
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);
}

D3D12_SHADER_RESOURCE_VIEW_DESC make_texture_srv(
    DXGI_FORMAT format,
    D3D12_SRV_DIMENSION srv_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t first_mip_level,
    uint32_t mip_levels,
    uint32_t shader_4_component_mapping)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
        .Format = format,
        .ViewDimension = srv_dimension,
        .Shader4ComponentMapping = shader_4_component_mapping
    };

    switch (srv_dimension)
    {
    case D3D12_SRV_DIMENSION_TEXTURE1D:
        srv_desc.Texture1D = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .ResourceMinLODClamp = 0.0f
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
        srv_desc.Texture1DArray = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers,
            .ResourceMinLODClamp = 0.0f
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURE2D:
        srv_desc.Texture2D = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .ResourceMinLODClamp = 0.0f
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
        srv_desc.Texture2DArray = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers,
            .ResourceMinLODClamp = 0.0f
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURE2DMS:
        srv_desc.Texture2DMS = {
            .UnusedField_NothingToDefine = 0
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
        srv_desc.Texture2DMSArray = {
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURE3D:
        srv_desc.Texture3D = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .ResourceMinLODClamp = 0.0f
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURECUBE:
        srv_desc.TextureCube = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .ResourceMinLODClamp = 0.0f
        };
        break;
    case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
        srv_desc.TextureCubeArray = {
            .MostDetailedMip = first_mip_level,
            .MipLevels = mip_levels,
            .First2DArrayFace = first_array_index,
            .NumCubes = depth_or_array_layers / 6u, // TODO: this might be wrong`with first_array_index = 0
            .ResourceMinLODClamp = 0.0f
        };
        break;
    default:
        break;
    }

    return srv_desc;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC make_full_texture_uav(
    DXGI_FORMAT format,
    D3D12_UAV_DIMENSION uav_dimension,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice)
{
    return make_texture_uav(format, uav_dimension, 0, depth_or_array_layers, mip_slice, 0);
}

D3D12_UNORDERED_ACCESS_VIEW_DESC make_texture_uav(
    DXGI_FORMAT format,
    D3D12_UAV_DIMENSION uav_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {
        .Format = format,
        .ViewDimension = uav_dimension,
    };

    switch (uav_dimension)
    {
    case D3D12_UAV_DIMENSION_TEXTURE1D:
        uav_desc.Texture1D = {
            .MipSlice = mip_slice
        };
        break;
    case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
        uav_desc.Texture1DArray = {
            .MipSlice = mip_slice,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_UAV_DIMENSION_TEXTURE2D:
        uav_desc.Texture2D = {
            .MipSlice = mip_slice,
            .PlaneSlice = plane_slice
        };
        break;
    case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
        uav_desc.Texture2DArray = {
            .MipSlice = mip_slice,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers,
            .PlaneSlice = plane_slice,
        };
        break;
    case D3D12_UAV_DIMENSION_TEXTURE2DMS:
        uav_desc.Texture2DMS = {
            .UnusedField_NothingToDefine = 0
        };
        break;
    case D3D12_UAV_DIMENSION_TEXTURE2DMSARRAY:
        uav_desc.Texture2DMSArray = {
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_UAV_DIMENSION_TEXTURE3D:
        uav_desc.Texture3D = {
            .MipSlice = mip_slice,
            .FirstWSlice = 0,
            .WSize = ~0u
        };
        break;
    default:
        break;
    }

    return uav_desc;
}

D3D12_RENDER_TARGET_VIEW_DESC make_full_texture_rtv(
    DXGI_FORMAT format,
    D3D12_RTV_DIMENSION rtv_dimension,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice)
{
    return make_texture_rtv(format, rtv_dimension, 0, depth_or_array_layers, mip_slice, plane_slice);
}

D3D12_RENDER_TARGET_VIEW_DESC make_texture_rtv(
    DXGI_FORMAT format,
    D3D12_RTV_DIMENSION rtv_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice)
{
    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
        .Format = format,
        .ViewDimension = rtv_dimension,
    };

    switch (rtv_dimension)
    {
    case D3D12_RTV_DIMENSION_TEXTURE1D:
        rtv_desc.Texture1D = {
            .MipSlice = mip_slice
        };
        break;
    case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
        rtv_desc.Texture1DArray = {
            .MipSlice = mip_slice,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_RTV_DIMENSION_TEXTURE2D:
        rtv_desc.Texture2D = {
            .MipSlice = mip_slice,
            .PlaneSlice = plane_slice
        };
        break;
    case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
        rtv_desc.Texture2DArray = {
            .MipSlice = mip_slice,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers,
            .PlaneSlice = plane_slice,
        };
        break;
    case D3D12_RTV_DIMENSION_TEXTURE2DMS:
        rtv_desc.Texture2DMS = {
            .UnusedField_NothingToDefine = 0
        };
        break;
    case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
        rtv_desc.Texture2DMSArray = {
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_RTV_DIMENSION_TEXTURE3D:
        rtv_desc.Texture3D = {
            .MipSlice = mip_slice,
            .FirstWSlice = 0,
            .WSize = ~0u
        };
        break;
    default:
        break;
    }

    return rtv_desc;
}

D3D12_DEPTH_STENCIL_VIEW_DESC make_full_texture_dsv(
    DXGI_FORMAT format,
    D3D12_DSV_DIMENSION dsv_dimension,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice)
{
    return make_texture_dsv(format, dsv_dimension, 0, depth_or_array_layers, mip_slice);
}

D3D12_DEPTH_STENCIL_VIEW_DESC make_texture_dsv(
    DXGI_FORMAT format,
    D3D12_DSV_DIMENSION dsv_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {
        .Format = format,
        .ViewDimension = dsv_dimension,
    };

    switch (dsv_dimension)
    {
    case D3D12_DSV_DIMENSION_TEXTURE1D:
        dsv_desc.Texture1D = {
            .MipSlice = mip_slice
        };
        break;
    case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
        dsv_desc.Texture1DArray = {
            .MipSlice = mip_slice,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_DSV_DIMENSION_TEXTURE2D:
        dsv_desc.Texture2D = {
            .MipSlice = mip_slice
        };
        break;
    case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
        dsv_desc.Texture2DArray = {
            .MipSlice = mip_slice,
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    case D3D12_DSV_DIMENSION_TEXTURE2DMS:
        dsv_desc.Texture2DMS = {
            .UnusedField_NothingToDefine = 0
        };
        break;
    case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
        dsv_desc.Texture2DMSArray = {
            .FirstArraySlice = first_array_index,
            .ArraySize = depth_or_array_layers
        };
        break;
    default:
        break;
    }

    return dsv_desc;
}

}
