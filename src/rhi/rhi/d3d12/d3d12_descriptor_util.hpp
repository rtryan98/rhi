#pragma once

#include <cstdint>
#include <agility_sdk/d3d12.h>

namespace rhi::d3d12
{
[[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC make_raw_buffer_srv(uint32_t size);
[[nodiscard]] D3D12_UNORDERED_ACCESS_VIEW_DESC make_raw_buffer_uav(uint32_t size);

[[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC make_full_texture_srv(
    DXGI_FORMAT format,
    D3D12_SRV_DIMENSION srv_dimension);

[[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC make_texture_srv(
    DXGI_FORMAT format,
    D3D12_SRV_DIMENSION srv_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t first_mip_level,
    uint32_t mip_levels,
    uint32_t shader_4_component_mapping);

[[nodiscard]] D3D12_UNORDERED_ACCESS_VIEW_DESC make_full_texture_uav(
    DXGI_FORMAT format,
    D3D12_UAV_DIMENSION uav_dimension,
    uint32_t mip_slice);

[[nodiscard]] D3D12_UNORDERED_ACCESS_VIEW_DESC make_texture_uav(
    DXGI_FORMAT format,
    D3D12_UAV_DIMENSION uav_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice);

[[nodiscard]] D3D12_RENDER_TARGET_VIEW_DESC make_full_texture_rtv(
    DXGI_FORMAT format,
    D3D12_RTV_DIMENSION rtv_dimension,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice);

[[nodiscard]] D3D12_RENDER_TARGET_VIEW_DESC make_texture_rtv(
    DXGI_FORMAT format,
    D3D12_RTV_DIMENSION rtv_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice,
    uint32_t plane_slice);

[[nodiscard]] D3D12_DEPTH_STENCIL_VIEW_DESC make_full_texture_dsv(
    DXGI_FORMAT format,
    D3D12_DSV_DIMENSION dsv_dimension,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice);

[[nodiscard]] D3D12_DEPTH_STENCIL_VIEW_DESC make_texture_dsv(
    DXGI_FORMAT format,
    D3D12_DSV_DIMENSION dsv_dimension,
    uint32_t first_array_index,
    uint32_t depth_or_array_layers,
    uint32_t mip_slice);
}
