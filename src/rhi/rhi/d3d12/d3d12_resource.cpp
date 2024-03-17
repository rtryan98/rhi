#pragma once

#include "rhi/d3d12/d3d12_resource.hpp"

#include <bit>

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


D3D12_SHADER_BYTECODE translate_shader_blob_safe(Shader_Blob* blob) noexcept
{
    D3D12_SHADER_BYTECODE bytecode = {};
    if (blob)
    {
        bytecode.BytecodeLength = blob->data.size();
        bytecode.pShaderBytecode = blob->data.data();
    }
    return bytecode;
}

D3D12_FILL_MODE translate_fill_mode(Fill_Mode fill_mode) noexcept
{
    return fill_mode == Fill_Mode::Solid
        ? D3D12_FILL_MODE_SOLID
        : D3D12_FILL_MODE_WIREFRAME;
}

D3D12_CULL_MODE translate_cull_mode(Cull_Mode cull_mode) noexcept
{
    switch (cull_mode)
    {
    case rhi::Cull_Mode::None:
        return D3D12_CULL_MODE_NONE;
    case rhi::Cull_Mode::Front:
        return D3D12_CULL_MODE_FRONT;
    case rhi::Cull_Mode::Back:
        return D3D12_CULL_MODE_BACK;
    default:
        return D3D12_CULL_MODE_NONE;
    }
    return D3D12_CULL_MODE_NONE;
}

D3D12_RASTERIZER_DESC1 translate_rasterizer_desc(const Pipeline_Rasterization_State_Info& raster_info) noexcept
{
    D3D12_RASTERIZER_DESC1 result = {
        .FillMode = translate_fill_mode(raster_info.fill_mode),
        .CullMode = translate_cull_mode(raster_info.cull_mode),
        .FrontCounterClockwise = raster_info.winding_order == Winding_Order::Front_Face_CCW ? true : false,
        .DepthBias = raster_info.depth_bias,
        .DepthBiasClamp = raster_info.depth_bias_clamp,
        .SlopeScaledDepthBias = raster_info.depth_bias_slope_scale,
        .DepthClipEnable = raster_info.depth_clip_enable,
        .MultisampleEnable = false,
        .AntialiasedLineEnable = false,
        .ForcedSampleCount = 0,
        .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };
    return result;
}

D3D12_BLEND translate_blend(Blend_Factor blend_factor) noexcept
{
    switch (blend_factor)
    {
    case Blend_Factor::Zero:
        return D3D12_BLEND_ZERO;
    case Blend_Factor::One:
        return D3D12_BLEND_ONE;
    case Blend_Factor::Src_Color:
        return D3D12_BLEND_SRC_COLOR;
    case Blend_Factor::One_Minus_Src_Color:
        return D3D12_BLEND_INV_SRC_COLOR;
    case Blend_Factor::Dst_Color:
        return D3D12_BLEND_DEST_COLOR;
    case Blend_Factor::One_Minus_Dst_Color:
        return D3D12_BLEND_INV_DEST_COLOR;
    case Blend_Factor::Src_Alpha:
        return D3D12_BLEND_SRC_ALPHA;
    case Blend_Factor::One_Minus_Src_Alpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case Blend_Factor::Dst_Alpha:
        return D3D12_BLEND_DEST_ALPHA;
    case Blend_Factor::One_Minus_Dst_Alpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
    case Blend_Factor::Constant_Color:
        return D3D12_BLEND_BLEND_FACTOR;
    case Blend_Factor::One_Minus_Constant_Color:
        return D3D12_BLEND_INV_BLEND_FACTOR;
    case Blend_Factor::Constant_Alpha:
        return D3D12_BLEND_ALPHA_FACTOR;
    case Blend_Factor::One_Minus_Constant_Alpha:
        return D3D12_BLEND_INV_ALPHA_FACTOR;
    case Blend_Factor::Src1_Color:
        return D3D12_BLEND_SRC1_COLOR;
    case Blend_Factor::One_Minus_Src1_Color:
        return D3D12_BLEND_INV_SRC1_COLOR;
    case Blend_Factor::Src1_Alpha:
        return D3D12_BLEND_SRC1_ALPHA;
    case Blend_Factor::One_Minus_Src1_Alpha:
        return D3D12_BLEND_INV_SRC1_ALPHA;
    default:
        return D3D12_BLEND_ZERO;
    }
}

D3D12_BLEND_OP translate_blend_op(Blend_Op blend_op) noexcept
{
    return std::bit_cast<D3D12_BLEND_OP>(blend_op);
}

D3D12_LOGIC_OP translate_logic_op(Logic_Op logic_op) noexcept
{
    return std::bit_cast<D3D12_LOGIC_OP>(logic_op);
}

uint8_t translate_renter_target_write_mask(Color_Component components) noexcept
{
    return static_cast<uint8_t>(components);
}

D3D12_RENDER_TARGET_BLEND_DESC translate_render_target_blend_desc(
    const Pipeline_Color_Attachment_Blend_Info& attachment_blend_info) noexcept
{
    D3D12_RENDER_TARGET_BLEND_DESC result = {
        .BlendEnable = attachment_blend_info.blend_enable,
        .LogicOpEnable = attachment_blend_info.logic_op_enable,
        .SrcBlend = translate_blend(attachment_blend_info.color_src_blend),
        .DestBlend = translate_blend(attachment_blend_info.color_dst_blend),
        .BlendOp = translate_blend_op(attachment_blend_info.color_blend_op),
        .SrcBlendAlpha = translate_blend(attachment_blend_info.alpha_src_blend),
        .DestBlendAlpha = translate_blend(attachment_blend_info.alpha_dst_blend),
        .BlendOpAlpha = translate_blend_op(attachment_blend_info.alpha_blend_op),
        .LogicOp = translate_logic_op(attachment_blend_info.logic_op),
        .RenderTargetWriteMask = translate_renter_target_write_mask(attachment_blend_info.color_write_mask)
    };
    return result;
}

D3D12_BLEND_DESC translate_blend_state_desc(const Pipeline_Blend_State_Info& blend_info) noexcept
{
    D3D12_BLEND_DESC result = {
        .AlphaToCoverageEnable = false,
        .IndependentBlendEnable = blend_info.independent_blend_enable,
        .RenderTarget = {}
    };
    for (auto i = 0; i < blend_info.color_attachments.size(); ++i)
    {
        result.RenderTarget[i] = translate_render_target_blend_desc(blend_info.color_attachments[i]);
    }
    return result;
}

D3D12_COMPARISON_FUNC translate_comparison_func(Comparison_Func comparison_func) noexcept
{
    return std::bit_cast<D3D12_COMPARISON_FUNC>(comparison_func);
}

D3D12_STENCIL_OP translate_stencil_op(Stencil_Op stencil_op) noexcept
{
    return std::bit_cast<D3D12_STENCIL_OP>(stencil_op);
}

D3D12_DEPTH_STENCILOP_DESC1 translate_depth_stencilop_desc(const Pipeline_Depth_Stencil_Op_Info& ds_op_info) noexcept
{
    D3D12_DEPTH_STENCILOP_DESC1 result = {
        .StencilFailOp = translate_stencil_op(ds_op_info.fail),
        .StencilDepthFailOp = translate_stencil_op(ds_op_info.depth_fail),
        .StencilPassOp = translate_stencil_op(ds_op_info.pass),
        .StencilFunc = translate_comparison_func(ds_op_info.comparison_func),
        .StencilReadMask = ds_op_info.stencil_read_mask,
        .StencilWriteMask = ds_op_info.stencil_write_mask
    };
    return result;
}

D3D12_DEPTH_STENCIL_DESC2 translate_depth_stencil_desc(const Pipeline_Depth_Stencil_State_Info& ds_info) noexcept
{
    D3D12_DEPTH_STENCIL_DESC2 result = {
        .DepthEnable = ds_info.depth_enable,
        .DepthWriteMask = ds_info.depth_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc = translate_comparison_func(ds_info.comparison_func),
        .StencilEnable = ds_info.stencil_enable,
        .FrontFace = translate_depth_stencilop_desc(ds_info.stencil_front_face),
        .BackFace = translate_depth_stencilop_desc(ds_info.stencil_back_face),
        .DepthBoundsTestEnable = ds_info.depth_bounds_test_mode == Depth_Bounds_Test_Mode::Disabled
            ? false
            : true
    };
    return result;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE translate_primitive_topology_type(Primitive_Topology_Type topology) noexcept
{
    return std::bit_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(topology);
}
}
