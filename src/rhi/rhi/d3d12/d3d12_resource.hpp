#pragma once

#include "rhi/resource.hpp"

#include <agility_sdk/d3d12.h>

namespace D3D12MA
{
class Allocation;
}

namespace rhi::d3d12
{
struct D3D12_Buffer_View;
struct D3D12_Image_View;

struct D3D12_Buffer : public Buffer
{
    ID3D12Resource2* resource;
    D3D12MA::Allocation* allocation;
    D3D12_Buffer_View* buffer_view_linked_list_head;
};

struct D3D12_Buffer_View : public Buffer_View
{
    D3D12_Buffer_View* next_buffer_view;
};

struct D3D12_Image : public Image
{
    ID3D12Resource2* resource;
    D3D12MA::Allocation* allocation;
    D3D12_Image_View* image_view_linked_list_head;
};

struct D3D12_Image_View : public Image_View
{
    uint32_t rtv_dsv_index;
    D3D12_Image_View* next_image_view;
};

struct D3D12_Sampler : public Sampler
{};

struct D3D12_Pipeline : public Pipeline
{
    ID3D12PipelineState* pso;
    ID3D12StateObject* rtpso;
};

[[nodiscard]] DXGI_FORMAT translate_format(Image_Format format) noexcept;
[[nodiscard]] D3D12_SRV_DIMENSION translate_view_type_srv(Image_View_Type type) noexcept;
[[nodiscard]] D3D12_UAV_DIMENSION translate_view_type_uav(Image_View_Type type) noexcept;
[[nodiscard]] D3D12_RTV_DIMENSION translate_view_type_rtv(Image_View_Type type) noexcept;
[[nodiscard]] D3D12_DSV_DIMENSION translate_view_type_dsv(Image_View_Type type) noexcept;
[[nodiscard]] D3D12_FILTER translate_filter(
    Sampler_Filter min, Sampler_Filter mag, Sampler_Filter mip,
    Sampler_Reduction_Type reduction, bool aniso) noexcept;
[[nodiscard]] D3D12_SHADER_BYTECODE translate_shader_blob_safe(Shader_Blob* blob) noexcept;
[[nodiscard]] D3D12_FILL_MODE translate_fill_mode(Fill_Mode fill_mode) noexcept;
[[nodiscard]] D3D12_CULL_MODE translate_cull_mode(Cull_Mode cull_mode) noexcept;
[[nodiscard]] D3D12_RASTERIZER_DESC1 translate_rasterizer_desc(
    const Pipeline_Rasterization_State_Info& raster_info) noexcept;
[[nodiscard]] D3D12_BLEND translate_blend(Blend_Factor blend_factor) noexcept;
[[nodiscard]] D3D12_BLEND_OP translate_blend_op(Blend_Op blend_op) noexcept;
[[nodiscard]] D3D12_LOGIC_OP translate_logic_op(Logic_Op logic_op) noexcept;
[[nodiscard]] uint8_t translate_renter_target_write_mask(Color_Component components) noexcept;
[[nodiscard]] D3D12_RENDER_TARGET_BLEND_DESC translate_render_target_blend_desc(
    const Pipeline_Color_Attachment_Blend_Info& attachment_blend_info) noexcept;
[[nodiscard]] D3D12_BLEND_DESC translate_blend_state_desc(
    const Pipeline_Blend_State_Info& blend_info) noexcept;
[[nodiscard]] D3D12_COMPARISON_FUNC translate_comparison_func(Comparison_Func comparison_func) noexcept;
[[nodiscard]] D3D12_STENCIL_OP translate_stencil_op(Stencil_Op stencil_op) noexcept;
[[nodiscard]] D3D12_DEPTH_STENCILOP_DESC1 translate_depth_stencilop_desc(
    const Pipeline_Depth_Stencil_Op_Info& ds_op_info) noexcept;
[[nodiscard]] D3D12_DEPTH_STENCIL_DESC2 translate_depth_stencil_desc(
    const Pipeline_Depth_Stencil_State_Info& ds_info) noexcept;
[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY_TYPE translate_primitive_topology_type(
    Primitive_Topology_Type topology) noexcept;
[[nodiscard]] Image_Format translate_image_format_to_dxgi_format(DXGI_FORMAT format) noexcept;
}
