#pragma once

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
    virtual void begin_rendering() noexcept = 0;
    virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) noexcept = 0;
    virtual void draw_indirect() noexcept = 0;
    virtual void draw_indirect_count() noexcept = 0;
    virtual void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t index_offset, uint32_t vertex_offset, uint32_t instance_offset) noexcept = 0;
    virtual void draw_indexed_indirect() noexcept = 0;
    virtual void draw_indexed_indirect_count() noexcept = 0;
    virtual void draw_mesh_tasks(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept = 0;
    virtual void end_rendering() noexcept = 0;

    // State commands
    virtual void set_pipeline(Pipeline* pipeline) noexcept = 0;
    virtual void set_push_constants(void* data, uint32_t size) noexcept = 0;
    virtual void set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height) noexcept = 0;
    virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth, [[maybe_unused]] bool vk_invert_viewport = true) noexcept = 0;

protected:
    Command_List_Type m_type;
};
}
