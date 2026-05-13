#pragma once

#include <array>
#include <vector>
#include <string_view>

#include "rhi/acceleration_structure.hpp"
#include "rhi/image_format.hpp"
#include "rhi/common/bitmask.hpp"

namespace rhi
{
constexpr static uint32_t NO_RESOURCE_INDEX = ~0u;
constexpr static uint32_t MAX_RESOURCE_INDEX = 500000;
constexpr static uint32_t MAX_SAMPLER_INDEX = 2048;

constexpr static uint32_t PIPELINE_COLOR_ATTACHMENTS_MAX = 8;

struct Buffer_View;
struct Image_View;

enum class Descriptor_Type
{
    Resource = 1,
    Color_Attachment,
    Depth_Stencil_Attachment
};

enum class Image_Component_Swizzle
{
    Identity = 0,
    Zero,
    One,
    R,
    G,
    B,
    A
};

enum class Image_Usage : uint32_t
{
    Sampled = 0x00000004,
    Unordered_Access = 0x00000008,
    Color_Attachment = 0x00000010,
    Depth_Stencil_Attachment = 0x00000020,
    Shading_Rate_Attachment = 0x00000100
};

enum class Image_View_Type
{
    Texture_1D,
    Texture_1D_Array,
    Texture_2D,
    Texture_2D_Array,
    Texture_2D_MS,
    Texture_2D_MS_Array,
    Texture_3D,
    Texture_Cube,
    Texture_Cube_Array
};

enum class Memory_Heap_Type
{
    GPU,
    CPU_Upload,
    CPU_Readback
};

enum class Fill_Mode
{
    Solid,
    Wireframe
};

enum class Cull_Mode
{
    None,
    Front,
    Back
};
Cull_Mode cull_mode_from_string(const std::string_view str);

enum class Winding_Order
{
    Front_Face_CCW,
    Front_Face_CW
};

enum class Comparison_Func
{
    None,
    Never,
    Less,
    Equal,
    Less_Equal,
    Greater,
    Not_Equal,
    Greater_Equal,
    Always
};
Comparison_Func comparison_func_from_string(std::string_view str);

enum class Stencil_Op
{
    Keep = 1,
    Zero = 2,
    Replace = 3,
    Incr_Sat = 4,
    Decr_Sat = 5,
    Invert = 6,
    Incr = 7,
    Decr = 8
};
Stencil_Op stencil_op_from_string(std::string_view str);

enum class Depth_Bounds_Test_Mode
{
    Disabled,
    Static,
    Dynamic
};
Depth_Bounds_Test_Mode depth_bounds_test_mode_from_string(std::string_view str);

enum class Primitive_Topology_Type
{
    Point = 1,
    Line = 2,
    Triangle = 3,
    Patch = 4
};
Primitive_Topology_Type primitive_topology_from_string(std::string_view str);

enum class Blend_Factor
{
    Zero,
    One,
    Src_Color,
    One_Minus_Src_Color,
    Dst_Color,
    One_Minus_Dst_Color,
    Src_Alpha,
    One_Minus_Src_Alpha,
    Dst_Alpha,
    One_Minus_Dst_Alpha,
    Constant_Color,
    One_Minus_Constant_Color,
    Constant_Alpha,
    One_Minus_Constant_Alpha,
    Src1_Color,
    One_Minus_Src1_Color,
    Src1_Alpha,
    One_Minus_Src1_Alpha
};
Blend_Factor blend_factor_from_string(std::string_view str);

enum class Blend_Op
{
    Add = 1,
    Sub = 2,
    Reverse_Sub = 3,
    Min = 4,
    Max = 5
};
Blend_Op blend_op_from_string(std::string_view str);

enum class Logic_Op
{
    Clear,
    Set,
    Copy,
    Copy_Inverted,
    Noop,
    Invert,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    Equiv,
    AND_Reverse,
    AND_Inverted,
    OR_Reverse,
    OR_Inverted
};
Logic_Op logic_op_from_string(std::string_view str);

enum class Color_Component
{
    R_Bit = 0x1,
    G_Bit = 0x2,
    B_Bit = 0x4,
    A_Bit = 0x8,
    Enable_All = R_Bit | G_Bit | B_Bit | A_Bit
};
Color_Component color_component_from_string(std::string_view str);

enum class Sampler_Filter
{
    Nearest = 0,
    Linear = 1
};

enum class Sampler_Reduction_Type
{
    Standard,
    Comparison
};

enum class Image_Sample_Address_Mode
{
    Wrap = 1,
    Mirror = 2,
    Clamp = 3,
    Border = 4
};

struct Buffer_Create_Info
{
    uint64_t size;
    Memory_Heap_Type heap;
    bool acceleration_structure_memory;

    auto operator<=>(const Buffer_Create_Info&) const = default;
};

struct Buffer
{
    uint64_t size;
    Memory_Heap_Type heap_type;
    Buffer_View* buffer_view;
    void* data;
    uint64_t gpu_address;
    Buffer_View* buffer_view_linked_list_head;

    auto operator<=>(const Buffer&) const = default;
};

struct Buffer_View_Create_Info
{
    uint64_t size;
    uint64_t offset;

    auto operator<=>(const Buffer_View_Create_Info&) const = default;
};

struct Buffer_View
{
    uint32_t bindless_index;
    uint64_t size;
    uint64_t offset;
    Buffer* buffer;
    Buffer_View* next_buffer_view;

    auto operator<=>(const Buffer_View&) const = default;
};

struct Image_Create_Info
{
    Image_Format format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint16_t array_size;
    uint16_t mip_levels;
    Image_Usage usage;
    Image_View_Type primary_view_type;

    auto operator<=>(const Image_Create_Info&) const = default;
};

struct Image
{
    Image_Format format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint16_t array_size;
    uint16_t mip_levels;
    Image_Usage usage;
    Image_View_Type primary_view_type;
    Image_View* image_view;
    Image_View* image_view_linked_list_head;

    auto operator<=>(const Image&) const = default;
};

struct Image_Component_Mapping
{
    Image_Component_Swizzle r;
    Image_Component_Swizzle g;
    Image_Component_Swizzle b;
    Image_Component_Swizzle a;
};

struct Image_View_Create_Info
{
    uint16_t first_array_level;
    uint16_t array_levels;
    uint16_t first_mip_level;
    uint16_t mip_levels;
    Image_Component_Mapping component_mapping;
    Image_View_Type view_type;
    Descriptor_Type descriptor_type;
};

struct Image_View
{
    uint32_t bindless_index;
    Image* image;
    Descriptor_Type descriptor_type;
    Image_View* next_image_view;

    auto operator<=>(const Image_View&) const = default;
};

struct Sampler_Create_Info
{
    Sampler_Filter filter_min;
    Sampler_Filter filter_mag;
    Sampler_Filter filter_mip;
    Image_Sample_Address_Mode address_mode_u;
    Image_Sample_Address_Mode address_mode_v;
    Image_Sample_Address_Mode address_mode_w;
    float mip_lod_bias;
    uint32_t max_anisotropy;
    Comparison_Func comparison_func;
    Sampler_Reduction_Type reduction;
    std::array<float, 4> border_color;
    float min_lod;
    float max_lod;
    bool anisotropy_enable; // Note: forces min/mag/mip filter to linear

    auto operator<=>(const Sampler_Create_Info&) const = default;
};

struct Sampler
{
    uint32_t bindless_index;

    auto operator<=>(const Sampler&) const = default;
};

struct Shader_Blob_Create_Info
{
    void* data;
    uint64_t data_size;
    uint32_t groups_x;
    uint32_t groups_y;
    uint32_t groups_z;

    auto operator<=>(const Shader_Blob_Create_Info&) const = default;
};

struct Shader_Blob
{
    std::vector<uint8_t> data;
    uint32_t groups_x;
    uint32_t groups_y;
    uint32_t groups_z;

    auto operator<=>(const Shader_Blob&) const = default;
};

struct Pipeline_Rasterization_State_Info
{
    Fill_Mode fill_mode;
    Cull_Mode cull_mode;
    Winding_Order winding_order;
    float depth_bias;
    float depth_bias_clamp;
    float depth_bias_slope_scale;
    bool depth_clip_enable;

    auto operator<=>(const Pipeline_Rasterization_State_Info&) const = default;
};

struct Pipeline_Color_Attachment_Blend_Info
{
    bool blend_enable;
    bool logic_op_enable;
    Blend_Factor color_src_blend;
    Blend_Factor color_dst_blend;
    Blend_Op color_blend_op;
    Blend_Factor alpha_src_blend;
    Blend_Factor alpha_dst_blend;
    Blend_Op alpha_blend_op;
    Logic_Op logic_op;
    Color_Component color_write_mask;

    auto operator<=>(const Pipeline_Color_Attachment_Blend_Info&) const = default;
};

struct Pipeline_Blend_State_Info
{
    bool independent_blend_enable;
    std::array<Pipeline_Color_Attachment_Blend_Info, PIPELINE_COLOR_ATTACHMENTS_MAX> color_attachments;

    auto operator<=>(const Pipeline_Blend_State_Info&) const = default;
};

struct Pipeline_Depth_Stencil_Op_Info
{
    Stencil_Op fail;
    Stencil_Op depth_fail;
    Stencil_Op pass;
    Comparison_Func comparison_func;
    uint8_t stencil_read_mask;
    uint8_t stencil_write_mask;

    auto operator<=>(const Pipeline_Depth_Stencil_Op_Info&) const = default;
};

struct Pipeline_Depth_Stencil_State_Info
{
    bool depth_enable;
    bool depth_write_enable;
    Comparison_Func comparison_func;
    bool stencil_enable;
    Pipeline_Depth_Stencil_Op_Info stencil_front_face;
    Pipeline_Depth_Stencil_Op_Info stencil_back_face;
    Depth_Bounds_Test_Mode depth_bounds_test_mode;
    float depth_bounds_min;
    float depth_bounds_max;

    auto operator<=>(const Pipeline_Depth_Stencil_State_Info&) const = default;
};

struct Graphics_Pipeline_Create_Info
{
    Shader_Blob* vs;
    Shader_Blob* hs;
    Shader_Blob* ds;
    Shader_Blob* gs;
    Shader_Blob* ps;
    Pipeline_Blend_State_Info blend_state_info;
    Pipeline_Rasterization_State_Info rasterizer_state_info;
    Pipeline_Depth_Stencil_State_Info depth_stencil_info;
    Primitive_Topology_Type primitive_topology;
    uint32_t color_attachment_count;
    std::array<Image_Format, PIPELINE_COLOR_ATTACHMENTS_MAX> color_attachment_formats;
    Image_Format depth_stencil_format;

    auto operator<=>(const Graphics_Pipeline_Create_Info&) const = default;
};

struct Compute_Pipeline_Create_Info
{
    Shader_Blob* cs;

    auto operator<=>(const Compute_Pipeline_Create_Info&) const = default;
};

struct Mesh_Shading_Pipeline_Create_Info
{
    Shader_Blob* ts;
    Shader_Blob* ms;
    Shader_Blob* ps;
    Pipeline_Blend_State_Info blend_state_info;
    Pipeline_Rasterization_State_Info rasterizer_state_info;
    Pipeline_Depth_Stencil_State_Info depth_stencil_info;
    Primitive_Topology_Type primitive_topology;
    uint32_t color_attachment_count;
    std::array<Image_Format, PIPELINE_COLOR_ATTACHMENTS_MAX> color_attachment_formats;
    Image_Format depth_stencil_format;

    auto operator<=>(const Mesh_Shading_Pipeline_Create_Info&) const = default;
};

enum class Pipeline_Type
{
    Vertex_Shading,
    Compute,
    Mesh_Shading,
    Ray_Tracing
};

struct Pipeline
{
    Pipeline_Type type;
    union
    {
        Graphics_Pipeline_Create_Info vertex_shading_info;
        Compute_Pipeline_Create_Info compute_shading_info;
        Mesh_Shading_Pipeline_Create_Info mesh_shading_info;
    };
};
}

template<>
constexpr static bool RHI_ENABLE_BIT_OPERATORS<rhi::Image_Usage> = true;

template<>
constexpr static bool RHI_ENABLE_BIT_OPERATORS<rhi::Color_Component> = true;
