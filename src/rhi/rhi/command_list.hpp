#pragma once

#include <core/common/bitmask.hpp>
#include <cstdint>

namespace rhi
{
struct Buffer;
struct Image;
struct Pipeline;

enum class Command_List_Type
{
    Graphics,
    Compute,
    Copy,
    Video_Decode,
    Video_Encode
};

enum class Barrier_Layout : uint64_t
{

};

enum class Barrier_Pipeline_Stage : uint64_t
{
    // Use Vulkan flags, translate to D3D12 flags (more conservative)
    None                            = 0x0000000000ull,
    Draw_Indirect                   = 0x0000000002ull,
    Vertex_Input                    = 0x0000000004ull,
    Vertex_Shader                   = 0x0000000008ull,
    Hull_Shader                     = 0x0000000010ull,  // Maps to Vertex Shader in D3D12
    Domain_Shader                   = 0x0000000020ull,  // Maps to Vertex Shader in D3D12
    Geometry_Shader                 = 0x0000000040ull,  // Maps to Vertex Shader in D3D12
    Pixel_Shader                    = 0x0000000080ull,
    Early_Fragment_Tests            = 0x0000000100ull,  // Maps to Render Target in D3D12
    Late_Fragment_Tests             = 0x0000000200ull,  // Maps to Render Target in D3D12
    Color_Attachment_Output         = 0x0000000400ull,  // Maps to Render Target in D3D12
    Compute_Shader                  = 0x0000000800ull,
    All_Transfer                    = 0x0000001000ull,  // Maps to Copy, Resolve, Ray Tracing Acceleration Structure Copy and Render Target in D3D12
    Host                            = 0x0000004000ull,  // No equivalent D3D12 mapping
    All_Graphics                    = 0x0000008000ull,  // Maps to Draw and Execute Indirect on D3D12
    All_Commands                    = 0x0000010000ull,
    Copy                            = 0x0100000000ull,
    Resolve                         = 0x0200000000ull,
    Blit                            = 0x0400000000ull,  // Maps to Render Target in D3D12
    Clear                           = 0x0800000000ull,  // Maps to Render Target in D3D12
    Index_Input                     = 0x1000000000ull,
    Vertex_Attribute_Input          = 0x2000000000ull,
    Pre_Rasterization_Stages        = 0x4000000000ull,
    Video_Decode                    = 0x0004000000ull,
    Video_Encode                    = 0x0008000000ull,

};

enum class Barrier_Access : uint64_t
{

};

class Command_List
{
public:
    // Barrier commands
    virtual void barrier() noexcept = 0;

    // Compute commands
    virtual void dispatch(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept = 0;

    // Copy commands
    virtual void copy_buffer(Buffer* src, Buffer* dst, uint32_t size, uint32_t offset) noexcept = 0;
    virtual void copy_buffer_to_image() noexcept = 0;
    virtual void copy_image() noexcept = 0;
    virtual void copy_image_to_buffer() noexcept = 0;
    virtual void fill_buffer() noexcept = 0;

    // Debug commands
    virtual void begin_debug_region() noexcept = 0;
    virtual void add_debug_marker()  noexcept = 0;
    virtual void end_debug_region() noexcept = 0;

    // Draw commands
    virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) noexcept = 0;
    virtual void draw_indirect() noexcept = 0;
    virtual void draw_indirect_count() noexcept = 0;
    virtual void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t index_offset, uint32_t vertex_offset, uint32_t instance_offset) noexcept = 0;
    virtual void draw_indexed_indirect() noexcept = 0;
    virtual void draw_indexed_indirect_count() noexcept = 0;
    virtual void draw_mesh_tasks(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept = 0;

    // State commands
    virtual void begin_render_pass() noexcept = 0;
    virtual void end_render_pass() noexcept = 0;
    virtual void set_pipeline(Pipeline* pipeline) noexcept = 0;
    virtual void set_push_constants(void* data, uint32_t size) noexcept = 0;
    virtual void set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height) noexcept = 0;
    virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth, [[maybe_unused]] bool vk_invert_viewport = true) noexcept = 0;

protected:
    Command_List_Type m_type;
};
}

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Barrier_Pipeline_Stage> = true;

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Barrier_Access> = true;
