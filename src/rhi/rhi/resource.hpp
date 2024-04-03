#pragma once

#include <array>
#include <core/common/bitmask.hpp>
#include <vector>

namespace rhi
{
constexpr static uint32_t PIPELINE_COLOR_ATTACHMENTS_MAX = 8;

struct Buffer_View;
struct Image_View;

enum class Descriptor_Type
{
    Resource = 1,
    Color_Attachment,
    Depth_Stencil_Attachment
};

enum class Image_Format
{
    Undefined = 0,
    R8_UNORM = 9,
    R8_SNORM = 10,
    R8_UINT = 13,
    R8_SINT = 14,
    R8G8_UNORM = 16,
    R8G8_SNORM = 17,
    R8G8_UINT = 20,
    R8G8_SINT = 21,
    R8G8B8A8_UNORM = 37,
    R8G8B8A8_SNORM = 38,
    R8G8B8A8_UINT = 41,
    R8G8B8A8_SINT = 42,
    R8G8B8A8_SRGB = 43,
    B8G8R8A8_UNORM = 44,
    B8G8R8A8_SNORM = 45,
    B8G8R8A8_UINT = 48,
    B8G8R8A8_SINT = 49,
    B8G8R8A8_SRGB = 50,
    A2R10G10B10_UNORM_PACK32 = 58,
    R16_UNORM = 70,
    R16_SNORM = 71,
    R16_UINT = 74,
    R16_SINT = 75,
    R16_SFLOAT = 76,
    R16G16_UNORM = 77,
    R16G16_SNORM = 78,
    R16G16_UINT = 81,
    R16G16_SINT = 82,
    R16G16_SFLOAT = 83,
    R16G16B16A16_UNORM = 91,
    R16G16B16A16_SNORM = 92,
    R16G16B16A16_UINT = 95,
    R16G16B16A16_SINT = 96,
    R16G16B16A16_SFLOAT = 97,
    R32_UINT = 98,
    R32_SINT = 99,
    R32_SFLOAT = 100,
    R32G32_UINT = 101,
    R32G32_SINT = 102,
    R32G32_SFLOAT = 103,
    R32G32B32A32_UINT = 107,
    R32G32B32A32_SINT = 108,
    R32G32B32A32_SFLOAT = 109,
    B10G11R11_UFLOAT_PACK32 = 122,
    E5B9G9R9_UFLOAT_PACK32 = 123,
    D16_UNORM = 124,
    D32_SFLOAT = 126,
    D24_UNORM_S8_UINT = 129,
    D32_SFLOAT_S8_UINT = 130,
    BC1_RGB_UNORM_BLOCK = 131,
    BC1_RGB_SRGB_BLOCK = 132,
    BC1_RGBA_UNORM_BLOCK = 133,
    BC1_RGBA_SRGB_BLOCK = 134,
    BC2_UNORM_BLOCK = 135,
    BC2_SRGB_BLOCK = 136,
    BC3_UNORM_BLOCK = 137,
    BC3_SRGB_BLOCK = 138,
    BC4_UNORM_BLOCK = 139,
    BC4_SNORM_BLOCK = 140,
    BC5_UNORM_BLOCK = 141,
    BC5_SNORM_BLOCK = 142,
    BC6H_UFLOAT_BLOCK = 143,
    BC6H_SFLOAT_BLOCK = 144,
    BC7_UNORM_BLOCK = 145,
    BC7_SRGB_BLOCK = 146,
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
    Greate,
    Not_Equal,
    Greater_Equal,
    Always
};

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

enum class Depth_Bounds_Test_Mode
{
    Disabled,
    Static,
    Dynamic
};

enum class Primitive_Topology_Type
{
    Point = 1,
    Line = 2,
    Triangle = 3,
    Patch = 4
};

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

enum class Blend_Op
{
    Add = 1,
    Sub = 2,
    Reverse_Sub = 3,
    Min = 4,
    Max = 5
};

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

enum class Color_Component
{
    R_Bit = 0x1,
    G_Bit = 0x2,
    B_Bit = 0x4,
    A_Bit = 0x8,
    Enable_All = R_Bit | G_Bit | B_Bit | A_Bit
};

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
};

struct Buffer
{
    uint64_t size;
    Memory_Heap_Type heap_type;
    Buffer_View* buffer_view;
    void* data;
};

struct Buffer_View_Create_Info
{
    uint64_t size;
    uint64_t offset;
};

struct Buffer_View
{
    uint32_t bindless_index;
    uint64_t size;
    uint64_t offset;
    Buffer* buffer;
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
};

struct Sampler
{
    uint32_t bindless_index;
};

struct Shader_Blob_Create_Info
{
    void* data;
    uint64_t data_size;
    uint32_t groups_x;
    uint32_t groups_y;
    uint32_t groups_z;
};

struct Shader_Blob
{
    std::vector<uint8_t> data;
    uint32_t groups_x;
    uint32_t groups_y;
    uint32_t groups_z;
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
};

struct Pipeline_Blend_State_Info
{
    bool independent_blend_enable;
    std::array<Pipeline_Color_Attachment_Blend_Info, PIPELINE_COLOR_ATTACHMENTS_MAX> color_attachments;
};

struct Pipeline_Depth_Stencil_Op_Info
{
    Stencil_Op fail;
    Stencil_Op depth_fail;
    Stencil_Op pass;
    Comparison_Func comparison_func;
    uint8_t stencil_read_mask;
    uint8_t stencil_write_mask;
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
};

struct Compute_Pipeline_Create_Info
{
    Shader_Blob* cs;
};

struct Mesh_Shading_Pipeline_Create_Info
{
    Shader_Blob* ts;
    Shader_Blob* ms;
    Pipeline_Blend_State_Info blend_state_info;
    Pipeline_Rasterization_State_Info rasterizer_state_info;
    Pipeline_Depth_Stencil_State_Info depth_stencil_info;
    Primitive_Topology_Type primitive_topology;
    uint32_t color_attachment_count;
    std::array<Image_Format, PIPELINE_COLOR_ATTACHMENTS_MAX> color_attachment_formats;
    Image_Format depth_stencil_format;
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

struct Image_Format_Info
{
    uint32_t bytes;
};

Image_Format_Info get_image_format_info(Image_Format format) noexcept;
}

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Image_Usage> = true;

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Color_Component> = true;
