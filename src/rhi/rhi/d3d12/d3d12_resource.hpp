#pragma once

#include "rhi/resource.hpp"

#include <agility_sdk/d3d12.h>

namespace D3D12MA
{
class Allocation;
}

namespace rhi::d3d12
{
struct D3D12_Buffer : public Buffer
{
    ID3D12Resource2* resource;
    D3D12MA::Allocation* allocation;
};

// No extra members required in D3D12
using D3D12_Buffer_View = Buffer_View;

struct D3D12_Image : public Image
{
    ID3D12Resource2* resource;
    D3D12MA::Allocation* allocation;
};

// TODO: remove; Allocate RTVs/DSVs on demand.
struct D3D12_Image_View : public Image_View
{
    uint32_t rtv_dsv_index;
};

// No extra members required in D3D12
using D3D12_Sampler = Sampler;

struct D3D12_Pipeline : public Pipeline
{
    ID3D12PipelineState* pso;
    ID3D12StateObject* rtpso;
};

[[nodiscard]] DXGI_FORMAT translate_format(Image_Format format) noexcept;
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
[[nodiscard]] D3D12_DEPTH_STENCILOP_DESC1 translate_depth_stencilop_desc(
    const Pipeline_Depth_Stencil_Op_Info& ds_op_info) noexcept;
[[nodiscard]] D3D12_DEPTH_STENCIL_DESC2 translate_depth_stencil_desc(
    const Pipeline_Depth_Stencil_State_Info& ds_info) noexcept;
[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY_TYPE translate_primitive_topology_type(
    Primitive_Topology_Type topology) noexcept;
[[nodiscard]] Image_Format translate_image_format_to_dxgi_format(DXGI_FORMAT format) noexcept;
}
