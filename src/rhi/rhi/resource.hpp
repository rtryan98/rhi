#pragma once

#include <vector>

namespace rhi
{
struct Buffer_View;
struct Image_View;

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
    Buffer_View* next_buffer_view;
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
    Image_View* next_image_view;
};

struct Image
{
    Image_View* next_image_view;
};

struct Image_View_Create_Info
{

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
