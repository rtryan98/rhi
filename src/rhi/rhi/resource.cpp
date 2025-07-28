#include "rhi/resource.hpp"

namespace rhi
{
Cull_Mode cull_mode_from_string(const std::string_view str)
{
    constexpr static auto CULL_MODES =
        std::to_array<std::pair<const std::string_view, const Cull_Mode>>(
        {
            {"None", Cull_Mode::None},
            {"Front", Cull_Mode::Front},
            {"Back", Cull_Mode::Back},
        });
    for (const auto& [string_val, enum_val] : CULL_MODES)
    {
        if (string_val == str)
            return enum_val;
    }
    return Cull_Mode::None;
}

Comparison_Func comparison_func_from_string(std::string_view str)
{
    constexpr static auto COMPARISON_FUNCS =
        std::to_array<std::pair<const std::string_view, const Comparison_Func>>(
        {
            {"None", Comparison_Func::None},
            {"Never", Comparison_Func::Never},
            {"Less", Comparison_Func::Less},
            {"Equal", Comparison_Func::Equal},
            {"Less_Equal", Comparison_Func::Less_Equal},
            {"Greater", Comparison_Func::Greater},
            {"Not_Equal", Comparison_Func::Not_Equal},
            {"Greater_Equal", Comparison_Func::Greater_Equal},
            {"Always", Comparison_Func::Always},
        });
    for (const auto& [string_val, enum_val] : COMPARISON_FUNCS)
    {
        if (string_val == str)
            return enum_val;
    }
    return Comparison_Func::None;
}

Stencil_Op stencil_op_from_string(std::string_view str)
{
    constexpr static auto STENCIL_OPS =
        std::to_array<std::pair<const std::string_view, const Stencil_Op>>(
        {
            {"Keep", Stencil_Op::Keep},
            {"Zero", Stencil_Op::Zero},
            {"Replace", Stencil_Op::Replace},
            {"Incr_Sat", Stencil_Op::Incr_Sat},
            {"Decr_Sat", Stencil_Op::Decr_Sat},
            {"Invert", Stencil_Op::Invert},
            {"Incr", Stencil_Op::Incr},
            {"Decr", Stencil_Op::Decr},
        });
    for (const auto& [string_val, enum_val] : STENCIL_OPS)
    {
        if (string_val == str)
            return enum_val;
    }
    return Stencil_Op::Keep;
}

Depth_Bounds_Test_Mode depth_bounds_test_mode_from_string(std::string_view str)
{
    constexpr static auto DEPTH_BOUNDS_TEST_MODES =
        std::to_array<std::pair<const std::string_view, const Depth_Bounds_Test_Mode>>(
        {
            {"Disabled", Depth_Bounds_Test_Mode::Disabled},
            {"Static", Depth_Bounds_Test_Mode::Static},
            {"Dynamic", Depth_Bounds_Test_Mode::Dynamic},
        });
    for (const auto& [string_val, enum_val] : DEPTH_BOUNDS_TEST_MODES)
    {
        if (string_val == str)
            return enum_val;
    }
    return Depth_Bounds_Test_Mode::Disabled;
}

Primitive_Topology_Type primitive_topology_from_string(std::string_view str)
{
    constexpr static auto PRIMITIVE_TOPOLOGY_TYPES =
        std::to_array<std::pair<const std::string_view, const Primitive_Topology_Type>>(
        {
            {"Point", Primitive_Topology_Type::Point},
            {"Line", Primitive_Topology_Type::Line},
            {"Triangle", Primitive_Topology_Type::Triangle},
            {"Patch", Primitive_Topology_Type::Patch},
        });
    for (const auto& [string_val, enum_val] : PRIMITIVE_TOPOLOGY_TYPES)
    {
        if (string_val == str)
            return enum_val;
    }
    return Primitive_Topology_Type::Triangle;
}

Blend_Factor blend_factor_from_string(std::string_view str)
{
    constexpr static auto BLEND_FACTORS =
        std::to_array<std::pair<const std::string_view, const Blend_Factor>>(
        {
            {"Zero", Blend_Factor::Zero},
            {"One", Blend_Factor::One},
            {"Src_Color", Blend_Factor::Src_Color},
            {"One_Minus_Src_Color", Blend_Factor::One_Minus_Src_Color},
            {"Dst_Color", Blend_Factor::Dst_Color},
            {"One_Minus_Dst_Color", Blend_Factor::One_Minus_Dst_Color},
            {"Src_Alpha", Blend_Factor::Src_Alpha},
            {"One_Minus_Src_Alpha", Blend_Factor::One_Minus_Src_Alpha},
            {"Dst_Alpha", Blend_Factor::Dst_Alpha},
            {"One_Minus_Dst_Alpha", Blend_Factor::One_Minus_Dst_Alpha},
            {"Constant_Color", Blend_Factor::Constant_Color},
            {"One_Minus_Constant_Color", Blend_Factor::One_Minus_Constant_Color},
            {"Constant_Alpha", Blend_Factor::Constant_Alpha},
            {"One_Minus_Constant_Alpha", Blend_Factor::One_Minus_Constant_Alpha},
            {"Src1_Color", Blend_Factor::Src1_Color},
            {"One_Minus_Src1_Color", Blend_Factor::One_Minus_Src1_Color},
            {"Src1_Alpha", Blend_Factor::Src1_Alpha},
            {"One_Minus_Src1_Alpha", Blend_Factor::One_Minus_Src1_Alpha}
        });
    for (const auto& [string_val, enum_val] : BLEND_FACTORS)
    {
        if (string_val == str)
            return enum_val;
    }
    return Blend_Factor::Zero;
}

Blend_Op blend_op_from_string(std::string_view str)
{
    constexpr static auto BLEND_OPS =
        std::to_array<std::pair<const std::string_view, const Blend_Op>>(
        {
            {"Add", Blend_Op::Add},
            {"Sub", Blend_Op::Sub},
            {"Reverse_Sub", Blend_Op::Reverse_Sub},
            {"Min", Blend_Op::Min},
            {"Max", Blend_Op::Max},
        });
    for (const auto& [string_val, enum_val] : BLEND_OPS)
    {
        if (string_val == str)
            return enum_val;
    }
    return Blend_Op::Add;
}

Logic_Op logic_op_from_string(std::string_view str)
{
    constexpr static auto LOGIC_OPS =
        std::to_array<std::pair<const std::string_view, const Logic_Op>>(
        {
            {"Clear", Logic_Op::Clear},
            {"Set", Logic_Op::Set},
            {"Copy", Logic_Op::Copy},
            {"Copy_Inverted", Logic_Op::Copy_Inverted},
            {"Noop", Logic_Op::Noop},
            {"Invert", Logic_Op::Invert},
            {"AND", Logic_Op::AND},
            {"NAND", Logic_Op::NAND},
            {"OR", Logic_Op::OR},
            {"NOR", Logic_Op::NOR},
            {"XOR", Logic_Op::XOR},
            {"Equiv", Logic_Op::Equiv},
            {"AND_Reverse", Logic_Op::AND_Reverse},
            {"AND_Inverted", Logic_Op::AND_Inverted},
            {"OR_Reverse", Logic_Op::OR_Reverse},
            {"OR_Inverted", Logic_Op::OR_Inverted},
        });
    for (const auto& [string_val, enum_val] : LOGIC_OPS)
    {
        if (string_val == str)
            return enum_val;
    }
    return Logic_Op::Clear;
}

Color_Component color_component_from_string(std::string_view str)
{
    Color_Component components{0};
    if (str.contains('R') || str.contains('r') ) components = components | Color_Component::R_Bit;
    if (str.contains('G') || str.contains('g') ) components = components | Color_Component::G_Bit;
    if (str.contains('B') || str.contains('b') ) components = components | Color_Component::B_Bit;
    if (str.contains('A') || str.contains('a') ) components = components | Color_Component::A_Bit;
    return components;
}

Image_Format_Info get_image_format_info(std::string_view string_format) noexcept
{
    constexpr static auto IMAGE_FORMATS =
        std::to_array<std::pair<const std::string_view, const Image_Format>>(
        {
            {"Undefined", Image_Format::Undefined},
            {"R8_UNORM", Image_Format::R8_UNORM},
            {"R8_SNORM",Image_Format::R8_SNORM},
            {"R8_UINT", Image_Format::R8_UINT},
            {"R8_SINT", Image_Format::R8_SINT},
            {"R8G8_UNORM", Image_Format::R8G8_UNORM},
            {"R8G8_SNORM", Image_Format::R8G8_SNORM},
            {"R8G8_UINT", Image_Format::R8G8_UINT},
            {"R8G8_SINT", Image_Format::R8G8_SINT},
            {"R8G8B8A8_UNORM", Image_Format::R8G8B8A8_UNORM},
            {"R8G8B8A8_SNORM", Image_Format::R8G8B8A8_SNORM},
            {"R8G8B8A8_UINT", Image_Format::R8G8B8A8_UINT},
            {"R8G8B8A8_SINT", Image_Format::R8G8B8A8_SINT},
            {"R8G8B8A8_SRGB", Image_Format::R8G8B8A8_SRGB},
            {"B8G8R8A8_UNORM", Image_Format::B8G8R8A8_UNORM},
            {"B8G8R8A8_SNORM", Image_Format::B8G8R8A8_SNORM},
            {"B8G8R8A8_UINT", Image_Format::B8G8R8A8_UINT},
            {"B8G8R8A8_SINT", Image_Format::B8G8R8A8_SINT},
            {"B8G8R8A8_SRGB", Image_Format::B8G8R8A8_SRGB},
            {"A2R10G10B10_UNORM_PACK32", Image_Format::A2R10G10B10_UNORM_PACK32},
            {"R16_UNORM", Image_Format::R16_UNORM},
            {"R16_SNORM", Image_Format::R16_SNORM},
            {"R16_UINT", Image_Format::R16_UINT},
            {"R16_SINT", Image_Format::R16_SINT},
            {"R16_SFLOAT", Image_Format::R16_SFLOAT},
            {"R16G16_UNORM", Image_Format::R16G16_UNORM},
            {"R16G16_SNORM", Image_Format::R16G16_SNORM},
            {"R16G16_UINT", Image_Format::R16G16_UINT},
            {"R16G16_SINT", Image_Format::R16G16_SINT},
            {"R16G16_SFLOAT", Image_Format::R16G16_SFLOAT},
            {"R16G16B16A16_UNORM", Image_Format::R16G16B16A16_UNORM},
            {"R16G16B16A16_SNORM", Image_Format::R16G16B16A16_SNORM},
            {"R16G16B16A16_UINT", Image_Format::R16G16B16A16_UINT},
            {"R16G16B16A16_SINT", Image_Format::R16G16B16A16_SINT},
            {"R16G16B16A16_SFLOAT", Image_Format::R16G16B16A16_SFLOAT},
            {"R32_UINT", Image_Format::R32_UINT},
            {"R32_SINT", Image_Format::R32_SINT},
            {"R32_SFLOAT", Image_Format::R32_SFLOAT},
            {"R32G32_UINT", Image_Format::R32G32_UINT},
            {"R32G32_SINT", Image_Format::R32G32_SINT},
            {"R32G32_SFLOAT", Image_Format::R32G32_SFLOAT},
            {"R32G32B32A32_UINT", Image_Format::R32G32B32A32_UINT},
            {"R32G32B32A32_SINT", Image_Format::R32G32B32A32_SINT},
            {"R32G32B32A32_SFLOAT", Image_Format::R32G32B32A32_SFLOAT},
            {"B10G11R11_UFLOAT_PACK32", Image_Format::B10G11R11_UFLOAT_PACK32},
            {"E5B9G9R9_UFLOAT_PACK32", Image_Format::E5B9G9R9_UFLOAT_PACK32},
            {"D16_UNORM", Image_Format::D16_UNORM},
            {"D32_SFLOAT", Image_Format::D32_SFLOAT},
            {"D24_UNORM_S8_UINT", Image_Format::D24_UNORM_S8_UINT},
            {"D32_SFLOAT_S8_UINT", Image_Format::D32_SFLOAT_S8_UINT},
            {"BC1_RGB_UNORM_BLOCK", Image_Format::BC1_RGB_UNORM_BLOCK},
            {"BC1_RGB_SRGB_BLOCK", Image_Format::BC1_RGB_SRGB_BLOCK},
            {"BC1_RGBA_UNORM_BLOCK", Image_Format::BC1_RGBA_UNORM_BLOCK},
            {"BC1_RGBA_SRGB_BLOCK", Image_Format::BC1_RGBA_SRGB_BLOCK},
            {"BC2_UNORM_BLOCK", Image_Format::BC2_UNORM_BLOCK},
            {"BC2_SRGB_BLOCK", Image_Format::BC2_SRGB_BLOCK},
            {"BC3_UNORM_BLOCK", Image_Format::BC3_UNORM_BLOCK},
            {"BC3_SRGB_BLOCK", Image_Format::BC3_SRGB_BLOCK},
            {"BC4_UNORM_BLOCK", Image_Format::BC4_UNORM_BLOCK},
            {"BC4_SNORM_BLOCK", Image_Format::BC4_SNORM_BLOCK},
            {"BC5_UNORM_BLOCK", Image_Format::BC5_UNORM_BLOCK},
            {"BC5_SNORM_BLOCK", Image_Format::BC5_SNORM_BLOCK},
            {"BC6H_UFLOAT_BLOCK", Image_Format::BC6H_UFLOAT_BLOCK},
            {"BC6H_SFLOAT_BLOCK", Image_Format::BC6H_SFLOAT_BLOCK},
            {"BC7_UNORM_BLOCK", Image_Format::BC7_UNORM_BLOCK},
            {"BC7_SRGB_BLOCK", Image_Format::BC7_SRGB_BLOCK}
        });
    for (const auto& [string_val, enum_val] : IMAGE_FORMATS)
    {
        if (string_val == string_format)
            return get_image_format_info(enum_val);
    }
    return get_image_format_info(Image_Format::Undefined);
}

Image_Format_Info get_image_format_info(Image_Format format) noexcept
{
    switch (format)
    {
    case Image_Format::Undefined:
        return {
            .format = Image_Format::Undefined,
            .bytes = 0
        };
    case Image_Format::R8_UNORM:
        return {
            .format = Image_Format::R8_UNORM,
            .bytes = 1
        };
    case Image_Format::R8_SNORM:
        return {
            .format = Image_Format::R8_SNORM,
            .bytes = 1
        };
    case Image_Format::R8_UINT:
        return {
            .format = Image_Format::R8_UINT,
            .bytes = 1
        };
    case Image_Format::R8_SINT:
        return {
            .format = Image_Format::R8_SINT,
            .bytes = 1
        };
    case Image_Format::R8G8_UNORM:
        return {
            .format = Image_Format::R8G8_UNORM,
            .bytes = 2
        };
    case Image_Format::R8G8_SNORM:
        return {
            .format = Image_Format::R8G8_SNORM,
            .bytes = 2
        };
    case Image_Format::R8G8_UINT:
        return {
            .format = Image_Format::R8G8_UINT,
            .bytes = 2
        };
    case Image_Format::R8G8_SINT:
        return {
            .format = Image_Format::R8G8_SINT,
            .bytes = 2
        };
    case Image_Format::R8G8B8A8_UNORM:
        return {
            .format = Image_Format::R8G8B8A8_UNORM,
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_SNORM:
        return {
            .format = Image_Format::R8G8B8A8_SNORM,
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_UINT:
        return {
            .format = Image_Format::R8G8B8A8_UINT,
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_SINT:
        return {
            .format = Image_Format::R8G8B8A8_SINT,
            .bytes = 4
        };
    case Image_Format::R8G8B8A8_SRGB:
        return {
            .format = Image_Format::R8G8B8A8_SRGB,
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_UNORM:
        return {
            .format = Image_Format::B8G8R8A8_UNORM,
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_SNORM:
        return {
            .format = Image_Format::B8G8R8A8_SNORM,
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_UINT:
        return {
            .format = Image_Format::B8G8R8A8_UINT,
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_SINT:
        return {
            .format = Image_Format::B8G8R8A8_SINT,
            .bytes = 4
        };
    case Image_Format::B8G8R8A8_SRGB:
        return {
            .format = Image_Format::B8G8R8A8_SRGB,
            .bytes = 4
        };
    case Image_Format::A2R10G10B10_UNORM_PACK32:
        return {
            .format = Image_Format::A2R10G10B10_UNORM_PACK32,
            .bytes = 4
        };
    case Image_Format::R16_UNORM:
        return {
            .format = Image_Format::R16_UNORM,
            .bytes = 2
        };
    case Image_Format::R16_SNORM:
        return {
            .format = Image_Format::R16_SNORM,
            .bytes = 2
        };
    case Image_Format::R16_UINT:
        return {
            .format = Image_Format::R16_UINT,
            .bytes = 2
        };
    case Image_Format::R16_SINT:
        return {
            .format = Image_Format::R16_SINT,
            .bytes = 2
        };
    case Image_Format::R16_SFLOAT:
        return {
            .format = Image_Format::R16_SFLOAT,
            .bytes = 2
        };
    case Image_Format::R16G16_UNORM:
        return {
            .format = Image_Format::R16G16_UNORM,
            .bytes = 4
        };
    case Image_Format::R16G16_SNORM:
        return {
            .format = Image_Format::R16G16_SNORM,
            .bytes = 4
        };
    case Image_Format::R16G16_UINT:
        return {
            .format = Image_Format::R16G16_UINT,
            .bytes = 4
        };
    case Image_Format::R16G16_SINT:
        return {
            .format = Image_Format::R16G16_SINT,
            .bytes = 4
        };
    case Image_Format::R16G16_SFLOAT:
        return {
            .format = Image_Format::R16G16_SFLOAT,
            .bytes = 4
        };
    case Image_Format::R16G16B16A16_UNORM:
        return {
            .format = Image_Format::R16G16B16A16_UNORM,
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_SNORM:
        return {
            .format = Image_Format::R16G16B16A16_SNORM,
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_UINT:
        return {
            .format = Image_Format::R16G16B16A16_UINT,
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_SINT:
        return {
            .format = Image_Format::R16G16B16A16_SINT,
            .bytes = 8
        };
    case Image_Format::R16G16B16A16_SFLOAT:
        return {
            .format = Image_Format::R16G16B16A16_SFLOAT,
            .bytes = 8
        };
    case Image_Format::R32_UINT:
        return {
            .format = Image_Format::R32_UINT,
            .bytes = 4
        };
    case Image_Format::R32_SINT:
        return {
            .format = Image_Format::R32_SINT,
            .bytes = 4
        };
    case Image_Format::R32_SFLOAT:
        return {
            .format = Image_Format::R32_SFLOAT,
            .bytes = 4
        };
    case Image_Format::R32G32_UINT:
        return {
            .format = Image_Format::R32G32_UINT,
            .bytes = 8
        };
    case Image_Format::R32G32_SINT:
        return {
            .format = Image_Format::R32G32_SINT,
            .bytes = 8
        };
    case Image_Format::R32G32_SFLOAT:
        return {
            .format = Image_Format::R32G32_SFLOAT,
            .bytes = 8
        };
    case Image_Format::R32G32B32A32_UINT:
        return {
            .format = Image_Format::R32G32B32A32_UINT,
            .bytes = 16
        };
    case Image_Format::R32G32B32A32_SINT:
        return {
            .format = Image_Format::R32G32B32A32_SINT,
            .bytes = 16
        };
    case Image_Format::R32G32B32A32_SFLOAT:
        return {
            .format = Image_Format::R32G32B32A32_SFLOAT,
            .bytes = 16
        };
    case Image_Format::B10G11R11_UFLOAT_PACK32:
        return {
            .format = Image_Format::B10G11R11_UFLOAT_PACK32,
            .bytes = 4
        };
    case Image_Format::E5B9G9R9_UFLOAT_PACK32:
        return {
            .format = Image_Format::E5B9G9R9_UFLOAT_PACK32,
            .bytes = 4
        };
    case Image_Format::D16_UNORM:
        return {
            .format = Image_Format::D16_UNORM,
            .bytes = 2
        };
    case Image_Format::D32_SFLOAT:
        return {
            .format = Image_Format::D32_SFLOAT,
            .bytes = 4
        };
    case Image_Format::D24_UNORM_S8_UINT:
        return {
            .format = Image_Format::D24_UNORM_S8_UINT,
            .bytes = 4
        };
    case Image_Format::D32_SFLOAT_S8_UINT:
        return {
            .format = Image_Format::D32_SFLOAT_S8_UINT,
            .bytes = 8
        };
    case Image_Format::BC1_RGB_UNORM_BLOCK:
        return {
            .format = Image_Format::BC1_RGB_UNORM_BLOCK,
            .bytes = 8
        };
    case Image_Format::BC1_RGB_SRGB_BLOCK:
        return {
            .format = Image_Format::BC1_RGB_SRGB_BLOCK,
            .bytes = 8
        };
    case Image_Format::BC1_RGBA_UNORM_BLOCK:
        return {
            .format = Image_Format::BC1_RGBA_UNORM_BLOCK,
            .bytes = 8
        };
    case Image_Format::BC1_RGBA_SRGB_BLOCK:
        return {
            .format = Image_Format::BC1_RGBA_SRGB_BLOCK,
            .bytes = 8
        };
    case Image_Format::BC2_UNORM_BLOCK:
        return {
            .format = Image_Format::BC2_UNORM_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC2_SRGB_BLOCK:
        return {
            .format = Image_Format::BC2_SRGB_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC3_UNORM_BLOCK:
        return {
            .format = Image_Format::BC3_UNORM_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC3_SRGB_BLOCK:
        return {
            .format = Image_Format::BC3_SRGB_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC4_UNORM_BLOCK:
        return {
            .format = Image_Format::BC4_UNORM_BLOCK,
            .bytes = 8
        };
    case Image_Format::BC4_SNORM_BLOCK:
        return {
            .format = Image_Format::BC4_SNORM_BLOCK,
            .bytes = 8
        };
    case Image_Format::BC5_UNORM_BLOCK:
        return {
            .format = Image_Format::BC5_UNORM_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC5_SNORM_BLOCK:
        return {
            .format = Image_Format::BC5_SNORM_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC6H_UFLOAT_BLOCK:
        return {
            .format = Image_Format::BC6H_UFLOAT_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC6H_SFLOAT_BLOCK:
        return {
            .format = Image_Format::BC6H_SFLOAT_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC7_UNORM_BLOCK:
        return {
            .format = Image_Format::BC7_UNORM_BLOCK,
            .bytes = 16
        };
    case Image_Format::BC7_SRGB_BLOCK:
        return {
            .format = Image_Format::BC7_SRGB_BLOCK,
            .bytes = 16
        };
    default:
        std::unreachable();
    }
}
}
