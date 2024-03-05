#pragma once

#include <vector>

namespace rhi
{
struct Buffer_View;
struct Image_View;

struct Buffer_Create_Info
{

};

struct Buffer
{
    uint32_t bindless_index;
    Buffer_View* next_buffer_view;
};

struct Buffer_View_Create_Info
{

};

struct Buffer_View
{
    uint32_t bindless_index;
    Buffer* buffer;
    Buffer_View* next_buffer_view;
};

struct Image_View_Create_Info
{

};

struct Image_Create_Info
{
    Image_View* next_image_view;
};

struct Image
{
    uint32_t bindless_index;
    Image_View* next_image_view;
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
