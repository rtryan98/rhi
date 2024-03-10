#pragma once

#include <core/common/bitmask.hpp>
#include <vector>

namespace rhi
{
struct Buffer_View;
struct Image_View;

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

enum class Image_Usage
{
    Sampled = 0x00000004,
    Unordered_Access = 0x00000008,
    Color_Attachment = 0x00000010,
    Depth_Stencil_Attachment = 0x00000020,
    Shading_Rate_Attachment = 0x00000100
};

enum class Memory_Heap_Type
{
    GPU,
    CPU_Upload,
    CPU_Readback
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
    Buffer_View* next_buffer_view;
};

struct Image_Create_Info
{
    Image_Format format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint16_t array_layers;
    uint16_t mip_levels;
    Image_Usage usage;
};

struct Image
{
    Image_Format format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint16_t array_layers;
    uint16_t mip_levels;
    Image_Usage usage;
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
};

struct Image_View
{
    uint32_t bindless_index;
    Image* image;
    Image_View* next_image_view;
};

struct Sampler_Create_Info
{

};

struct Sampler
{
    uint32_t bindless_index;
};

struct Shader_Blob
{
    std::vector<uint8_t> data;
};

struct Graphics_Pipeline_Create_Info
{

};

struct Compute_Pipeline_Create_Info
{

};

struct Mesh_Shading_Pipeline_Create_Info
{

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
};
}

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Image_Usage> = true;
