#pragma once

#include <agility_sdk/d3d12.h>

namespace rhi
{
template<typename T, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Subobject_Type>
struct alignas(void*) Pipeline_Stream_Object {
    const D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type = Subobject_Type;
    T data;
};

using Pipeline_Stream_Object_Flags
    = Pipeline_Stream_Object<D3D12_PIPELINE_STATE_FLAGS, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS>;

using Pipeline_Stream_Object_Node_Mask
    = Pipeline_Stream_Object<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK>;

using Pipeline_Stream_Object_Root_Signature
    = Pipeline_Stream_Object<ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE>;

using Pipeline_Stream_Object_Input_Layout_Desc
    = Pipeline_Stream_Object<D3D12_INPUT_LAYOUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT>;

using Pipeline_Stream_Object_Primitive_Topology_Type
    = Pipeline_Stream_Object<D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY>;

using Pipeline_Stream_Object_Vertex_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS>;

using Pipeline_Stream_Object_Hull_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS>;

using Pipeline_Stream_Object_Domain_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS>;

using Pipeline_Stream_Object_Geometry_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS>;

using Pipeline_Stream_Object_Amplification_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS>;

using Pipeline_Stream_Object_Mesh_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS>;

using Pipeline_Stream_Object_Pixel_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS>;

using Pipeline_Stream_Object_Compute_Shader
    = Pipeline_Stream_Object<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS>;

using Pipeline_Stream_Object_Stream_Output_Desc
    = Pipeline_Stream_Object<D3D12_STREAM_OUTPUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT>;

using Pipeline_Stream_Object_Blend_Desc
    = Pipeline_Stream_Object<D3D12_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND>;

using Pipeline_Stream_Object_Depth_Stencil_Desc
    = Pipeline_Stream_Object<D3D12_DEPTH_STENCIL_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL>;

using Pipeline_Stream_Object_Depth_Stencil_Desc1
    = Pipeline_Stream_Object<D3D12_DEPTH_STENCIL_DESC1, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1>;

using Pipeline_Stream_Object_Depth_Stencil_Desc2
    = Pipeline_Stream_Object<D3D12_DEPTH_STENCIL_DESC2, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL2>;

using Pipeline_Stream_Object_Depth_Stencil_Format
    = Pipeline_Stream_Object<DXGI_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT>;

using Pipeline_Stream_Object_Rasterizer_Desc
    = Pipeline_Stream_Object<D3D12_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER>;

using Pipeline_Stream_Object_Rasterizer_Desc1
    = Pipeline_Stream_Object<D3D12_RASTERIZER_DESC1, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER1>;

using Pipeline_Stream_Object_Rasterizer_Desc2
    = Pipeline_Stream_Object<D3D12_RASTERIZER_DESC2, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER2>;

using Pipeline_Stream_Object_Render_Target_Formats
    = Pipeline_Stream_Object<D3D12_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS>;

using Pipeline_Stream_Object_Sample_Desc
    = Pipeline_Stream_Object<DXGI_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC>;

using Pipeline_Stream_Object_Sample_Mask
    = Pipeline_Stream_Object<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK>;

using Pipeline_Stream_Object_Cached_Pipeline_State
    = Pipeline_Stream_Object<D3D12_CACHED_PIPELINE_STATE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO>;

using Pipeline_Stream_Object_View_Instancing_Desc
    = Pipeline_Stream_Object<D3D12_VIEW_INSTANCING_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING>;

struct Graphics_Pipeline_Desc
{
    Pipeline_Stream_Object_Root_Signature root_signature;
    Pipeline_Stream_Object_Vertex_Shader vs;
    Pipeline_Stream_Object_Hull_Shader hs;
    Pipeline_Stream_Object_Domain_Shader ds;
    Pipeline_Stream_Object_Geometry_Shader gs;
    Pipeline_Stream_Object_Pixel_Shader ps;
    Pipeline_Stream_Object_Stream_Output_Desc stream_output;
    Pipeline_Stream_Object_Blend_Desc blend_state;
    Pipeline_Stream_Object_Sample_Mask sample_mask;
    Pipeline_Stream_Object_Rasterizer_Desc1 rasterizer_state;
    Pipeline_Stream_Object_Depth_Stencil_Desc2 depth_stencil_state;
    Pipeline_Stream_Object_Input_Layout_Desc input_layout_desc;
    Pipeline_Stream_Object_Primitive_Topology_Type primitive_topology_type;
    Pipeline_Stream_Object_Render_Target_Formats render_target_formats;
    Pipeline_Stream_Object_Depth_Stencil_Format depth_stencil_format;
    Pipeline_Stream_Object_Sample_Desc sample_desc;
    Pipeline_Stream_Object_Node_Mask node_mask;
    Pipeline_Stream_Object_Cached_Pipeline_State cached_pso;
    Pipeline_Stream_Object_Flags flags;
};

struct Mesh_Shader_Pipeline_Desc
{
    Pipeline_Stream_Object_Root_Signature root_signature;
    Pipeline_Stream_Object_Amplification_Shader as;
    Pipeline_Stream_Object_Mesh_Shader ms;
    Pipeline_Stream_Object_Pixel_Shader ps;
    Pipeline_Stream_Object_Blend_Desc blend_state;
    Pipeline_Stream_Object_Sample_Mask sample_mask;
    Pipeline_Stream_Object_Rasterizer_Desc1 rasterizer_state;
    Pipeline_Stream_Object_Depth_Stencil_Desc2 depth_stencil_state;
    Pipeline_Stream_Object_Input_Layout_Desc input_layout_desc;
    Pipeline_Stream_Object_Primitive_Topology_Type primitive_topology_type;
    Pipeline_Stream_Object_Render_Target_Formats render_target_formats;
    Pipeline_Stream_Object_Depth_Stencil_Format depth_stencil_format;
    Pipeline_Stream_Object_Sample_Desc sample_desc;
    Pipeline_Stream_Object_Node_Mask node_mask;
    Pipeline_Stream_Object_Cached_Pipeline_State cached_pso;
    Pipeline_Stream_Object_Flags flags;
};

}
