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

enum class Barrier_Layout : uint32_t
{

};

enum class Barrier_Pipeline_Stage : uint32_t
{

};

enum class Barrier_Access : uint32_t
{
    Common = 0x0,
    Vertex_Buffer = 0x1,
    Constant_Buffer = 0x2,
    Index_Buffer = 0x4,
    Render_Target = 0x8,
    Unordered_Access = 0x10,
    Depth_Stencil_Write = 0x20,
    Depth_Stencil_Read = 0x40,
    Shader_Resource = 0x80,
    Stream_Output = 0x100,
    Indirect_Argument = 0x200,
    Copy_Dest = 0x400,
    Copy_Source = 0x800,
    Resolve_Dest = 0x1000,
    Resolve_Source = 0x2000,
    Ray_Tracing_Acceleration_Structure_Read = 0x4000,
    Ray_Tracing_Acceleration_Structure_Write = 0x8000,
    Shading_Rate_Source = 0x10000,
    Video_Decode_Read = 0x20000,
    Video_Decode_Write = 0x40000,
    Video_Process_Read = 0x80000,
    Video_Process_Write = 0x100000,
    Video_Encode_Read = 0x200000,
    Video_Encode_Write = 0x400000,
    No_Access = 0x80000000
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
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Barrier_Layout> = true;

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Barrier_Pipeline_Stage> = true;

template<>
constexpr static bool CORE_ENABLE_BIT_OPERATORS<rhi::Barrier_Access> = true;
