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
}
