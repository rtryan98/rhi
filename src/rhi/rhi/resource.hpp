#pragma once

#include <vector>

namespace rhi
{
struct Buffer_Create_Info
{

};

struct Buffer
{

};

struct Image_Create_Info
{

};

struct Image
{

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
