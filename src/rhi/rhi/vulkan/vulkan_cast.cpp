#include "rhi/vulkan/vulkan_cast.hpp"

#include "rhi/vulkan/vulkan_format.hpp"
#include "rhi/resource.hpp"

#include <bit>
#include <vulkan/vulkan.h>

namespace rhi::vulkan
{
template<>
VkFormat vulkan_cast<VkFormat, Image_Format>(const Image_Format image_format)
{
    return translate_format(image_format);
}

template<>
VkImageType vulkan_cast<VkImageType>(const Image_View_Type image_view_type)
{
    auto image_type = VK_IMAGE_TYPE_1D;
    switch (image_view_type)
    {
    case Image_View_Type::Texture_2D:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube:
        [[fallthrough]];
    case Image_View_Type::Texture_Cube_Array:
        [[fallthrough]];
    case Image_View_Type::Texture_2D_MS_Array:
        image_type = VK_IMAGE_TYPE_2D;
        break;
    case Image_View_Type::Texture_3D:
        image_type = VK_IMAGE_TYPE_3D;
        break;
    default:
        break;
    }
    return image_type;
}

template<>
VkImageViewType vulkan_cast<VkImageViewType>(const Image_View_Type image_view_type)
{
    switch (image_view_type)
    {
    case Image_View_Type::Texture_1D:
        return VK_IMAGE_VIEW_TYPE_1D;
    case Image_View_Type::Texture_1D_Array:
        return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case Image_View_Type::Texture_2D:
        return VK_IMAGE_VIEW_TYPE_2D;
    case Image_View_Type::Texture_2D_Array:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case Image_View_Type::Texture_2D_MS:
        return VK_IMAGE_VIEW_TYPE_2D;
    case Image_View_Type::Texture_Cube:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case Image_View_Type::Texture_Cube_Array:
        return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    case Image_View_Type::Texture_2D_MS_Array:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case Image_View_Type::Texture_3D:
        return VK_IMAGE_VIEW_TYPE_3D;
    default:
        return VK_IMAGE_VIEW_TYPE_2D;
    }
}

template<>
VkImageUsageFlags vulkan_cast<VkImageUsageFlags>(const Image_Usage image_usage)
{
    VkImageUsageFlags result = 0;

    result |= (image_usage & Image_Usage::Sampled) == Image_Usage::Sampled
        ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
    result |= (image_usage & Image_Usage::Unordered_Access) == Image_Usage::Unordered_Access
        ? VK_IMAGE_USAGE_STORAGE_BIT : 0;
    result |= (image_usage & Image_Usage::Color_Attachment) == Image_Usage::Color_Attachment
        ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
    result |= (image_usage & Image_Usage::Depth_Stencil_Attachment) == Image_Usage::Depth_Stencil_Attachment
        ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
    result |= (image_usage & Image_Usage::Shading_Rate_Attachment) == Image_Usage::Shading_Rate_Attachment
        ? VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR : 0;

    return result;
}

template<>
VkFilter vulkan_cast<VkFilter>(const Sampler_Filter sampler_filter)
{
    switch (sampler_filter)
    {
    case Sampler_Filter::Nearest:
        return VK_FILTER_NEAREST;
    default:
        return VK_FILTER_LINEAR;
    }
}

template<>
VkSamplerMipmapMode vulkan_cast<VkSamplerMipmapMode>(const Sampler_Filter sampler_filter)
{
    switch (sampler_filter)
    {
    case Sampler_Filter::Nearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

template<>
VkSamplerAddressMode vulkan_cast<VkSamplerAddressMode>(const Image_Sample_Address_Mode address_mode)
{
    switch (address_mode)
    {
    case Image_Sample_Address_Mode::Wrap:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case Image_Sample_Address_Mode::Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case Image_Sample_Address_Mode::Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case Image_Sample_Address_Mode::Border:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    default:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

template<>
VkCompareOp vulkan_cast<VkCompareOp>(const Comparison_Func comparison_func)
{
    switch (comparison_func)
    {
    case Comparison_Func::None:
        return VK_COMPARE_OP_NEVER;
    case Comparison_Func::Never:
        return VK_COMPARE_OP_NEVER;
    case Comparison_Func::Less:
        return VK_COMPARE_OP_LESS;
    case Comparison_Func::Equal:
        return VK_COMPARE_OP_EQUAL;
    case Comparison_Func::Less_Equal:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case Comparison_Func::Greater:
        return VK_COMPARE_OP_GREATER;
    case Comparison_Func::Not_Equal:
        return VK_COMPARE_OP_NOT_EQUAL;
    case Comparison_Func::Greater_Equal:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case Comparison_Func::Always:
        return VK_COMPARE_OP_ALWAYS;
    }
    return VK_COMPARE_OP_NEVER;
}

template<>
VkAccelerationStructureTypeKHR vulkan_cast<VkAccelerationStructureTypeKHR>(const Acceleration_Structure_Type acceleration_structure_type)
{
    if (acceleration_structure_type == Acceleration_Structure_Type::Bottom_Level)
        return VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    return VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
}

template<>
VkComponentSwizzle vulkan_cast<VkComponentSwizzle>(const Image_Component_Swizzle component_swizzle)
{
    switch (component_swizzle)
    {
    case Image_Component_Swizzle::Identity: return VK_COMPONENT_SWIZZLE_IDENTITY;
    case Image_Component_Swizzle::Zero: return VK_COMPONENT_SWIZZLE_ZERO;
    case Image_Component_Swizzle::One: return VK_COMPONENT_SWIZZLE_ONE;
    case Image_Component_Swizzle::R: return VK_COMPONENT_SWIZZLE_R;
    case Image_Component_Swizzle::G: return VK_COMPONENT_SWIZZLE_G;
    case Image_Component_Swizzle::B: return VK_COMPONENT_SWIZZLE_B;
    case Image_Component_Swizzle::A: return VK_COMPONENT_SWIZZLE_A;
    default: return VK_COMPONENT_SWIZZLE_IDENTITY;
    }
}

template<>
VkComponentMapping vulkan_cast<VkComponentMapping>(const Image_Component_Mapping component_mapping)
{
    return {
        .r = vulkan_cast<VkComponentSwizzle>(component_mapping.r),
        .g = vulkan_cast<VkComponentSwizzle>(component_mapping.g),
        .b = vulkan_cast<VkComponentSwizzle>(component_mapping.b),
        .a = vulkan_cast<VkComponentSwizzle>(component_mapping.a)
    };
}

template<>
VkStencilOp vulkan_cast<VkStencilOp>(const Stencil_Op stencil_op)
{
    switch (stencil_op)
    {
    case Stencil_Op::Keep: return VK_STENCIL_OP_KEEP;
    case Stencil_Op::Zero: return VK_STENCIL_OP_ZERO;
    case Stencil_Op::Replace: return VK_STENCIL_OP_REPLACE;
    case Stencil_Op::Incr_Sat: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case Stencil_Op::Decr_Sat: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case Stencil_Op::Invert: return VK_STENCIL_OP_INVERT;
    case Stencil_Op::Incr: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case Stencil_Op::Decr: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    default: return VK_STENCIL_OP_KEEP;
    }
}

template<>
VkLogicOp vulkan_cast<VkLogicOp>(const Logic_Op logic_op)
{
    switch (logic_op)
    {
    case Logic_Op::Clear: return VK_LOGIC_OP_CLEAR;
    case Logic_Op::Set: return VK_LOGIC_OP_SET;
    case Logic_Op::Copy: return VK_LOGIC_OP_COPY;
    case Logic_Op::Copy_Inverted: return VK_LOGIC_OP_COPY_INVERTED;
    case Logic_Op::Noop: return VK_LOGIC_OP_NO_OP;
    case Logic_Op::Invert: return VK_LOGIC_OP_INVERT;
    case Logic_Op::AND: return VK_LOGIC_OP_AND;
    case Logic_Op::NAND: return VK_LOGIC_OP_NAND;
    case Logic_Op::OR: return VK_LOGIC_OP_OR;
    case Logic_Op::NOR: return VK_LOGIC_OP_NOR;
    case Logic_Op::XOR: return VK_LOGIC_OP_XOR;
    case Logic_Op::Equiv: return VK_LOGIC_OP_EQUIVALENT;
    case Logic_Op::AND_Reverse: return VK_LOGIC_OP_AND_REVERSE;
    case Logic_Op::AND_Inverted: return VK_LOGIC_OP_AND_INVERTED;
    case Logic_Op::OR_Reverse: return VK_LOGIC_OP_OR_REVERSE;
    case Logic_Op::OR_Inverted: return VK_LOGIC_OP_OR_INVERTED;
    default: return VK_LOGIC_OP_CLEAR;
    }
}

template<>
VkBlendFactor vulkan_cast<VkBlendFactor>(const Blend_Factor blend_factor)
{
    switch (blend_factor)
    {
    case Blend_Factor::Zero: return VK_BLEND_FACTOR_ZERO;
    case Blend_Factor::One: return VK_BLEND_FACTOR_ONE;
    case Blend_Factor::Src_Color: return VK_BLEND_FACTOR_SRC_COLOR;
    case Blend_Factor::One_Minus_Src_Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case Blend_Factor::Dst_Color: return VK_BLEND_FACTOR_DST_COLOR;
    case Blend_Factor::One_Minus_Dst_Color: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case Blend_Factor::Src_Alpha: return VK_BLEND_FACTOR_SRC_ALPHA;
    case Blend_Factor::One_Minus_Src_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case Blend_Factor::Dst_Alpha: return VK_BLEND_FACTOR_DST_ALPHA;
    case Blend_Factor::One_Minus_Dst_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case Blend_Factor::Constant_Color: return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case Blend_Factor::One_Minus_Constant_Color: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case Blend_Factor::Constant_Alpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case Blend_Factor::One_Minus_Constant_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case Blend_Factor::Src1_Color: return VK_BLEND_FACTOR_SRC1_COLOR;
    case Blend_Factor::One_Minus_Src1_Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case Blend_Factor::Src1_Alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
    case Blend_Factor::One_Minus_Src1_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    default: return VK_BLEND_FACTOR_ONE;
    }
}

template<>
VkBlendOp vulkan_cast<VkBlendOp>(const Blend_Op blend_op)
{
    switch (blend_op)
    {
    case Blend_Op::Add: return VK_BLEND_OP_ADD;
    case Blend_Op::Sub: return VK_BLEND_OP_SUBTRACT;
    case Blend_Op::Reverse_Sub: return VK_BLEND_OP_REVERSE_SUBTRACT;
    case Blend_Op::Min: return VK_BLEND_OP_MIN;
    case Blend_Op::Max: return VK_BLEND_OP_MAX;
    default: return VK_BLEND_OP_ADD;
    }
}

template<>
VkColorComponentFlags vulkan_cast<VkColorComponentFlags>(const Color_Component color_component)
{
    return std::bit_cast<VkColorComponentFlags>(color_component);
}

template<>
VkPrimitiveTopology vulkan_cast<VkPrimitiveTopology>(const Primitive_Topology_Type primitive_topology)
{
    switch (primitive_topology)
    {
    case Primitive_Topology_Type::Point: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case Primitive_Topology_Type::Line: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case Primitive_Topology_Type::Triangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case Primitive_Topology_Type::Patch: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

template<>
VkPolygonMode vulkan_cast<VkPolygonMode>(const Fill_Mode fill_mode)
{
    if (fill_mode == Fill_Mode::Solid)
        return VK_POLYGON_MODE_FILL;
    return VK_POLYGON_MODE_LINE;
}

template<>
VkCullModeFlags vulkan_cast<VkCullModeFlags>(const Cull_Mode cull_mode)
{
    switch (cull_mode)
    {
    case Cull_Mode::None: return VK_CULL_MODE_NONE;
    case Cull_Mode::Front: return VK_CULL_MODE_FRONT_BIT;
    case Cull_Mode::Back: return VK_CULL_MODE_BACK_BIT;
    default: return VK_CULL_MODE_NONE;
    }
}

template<>
VkFrontFace vulkan_cast<VkFrontFace>(const Winding_Order winding_order)
{
    if (winding_order == Winding_Order::Front_Face_CCW)
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    return VK_FRONT_FACE_CLOCKWISE;
}
}
