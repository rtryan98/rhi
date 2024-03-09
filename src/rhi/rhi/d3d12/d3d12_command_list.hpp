#pragma once

#include "rhi/command_list.hpp"

#include <agility_sdk/d3d12.h>

namespace rhi::d3d12
{
class D3D12_Graphics_Device;

class D3D12_Command_List final : public Command_List
{
public:
    // Meta commands
    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept override;

    // Barrier commands
    virtual void barrier(const Barrier_Info& barrier_info) noexcept override;

    // Compute commands
    virtual void dispatch(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept override;
    virtual void dispatch_indirect(Buffer* buffer, uint64_t offset) noexcept override;
    virtual void dispatch_indirect_d3d12(Buffer* buffer, uint64_t offset, uint32_t count) noexcept override;
    virtual void dispatch_indirect_count_d3d12(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept override;

    // Copy commands
    virtual void copy_buffer(Buffer* src, uint64_t src_offset, Buffer* dst, uint64_t dst_offset, uint64_t size) noexcept override;
    virtual void copy_buffer_to_image() noexcept override;
    virtual void copy_image() noexcept override;
    virtual void copy_image_to_buffer() noexcept override;
    virtual void fill_buffer(Buffer_View* dst, uint32_t value) noexcept override;

    // Debug commands
    virtual void begin_debug_region(const char* name, float r, float g, float b) noexcept override;
    virtual void add_debug_marker(const char* name, float r, float g, float b)  noexcept override;
    virtual void end_debug_region() noexcept override;

    // Draw commands
    virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) noexcept override;
    virtual void draw_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept override;
    virtual void draw_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept override;
    virtual void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t index_offset, uint32_t vertex_offset, uint32_t instance_offset) noexcept override;
    virtual void draw_indexed_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept override;
    virtual void draw_indexed_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept override;
    virtual void draw_mesh_tasks(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept override;
    virtual void draw_mesh_tasks_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept override;
    virtual void draw_mesh_tasks_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept override;

    // State commands
    virtual void begin_render_pass(const Render_Pass_Begin_Info& begin_info) noexcept override;
    virtual void end_render_pass() noexcept override;
    virtual void set_pipeline(Pipeline* pipeline) noexcept override;
    virtual void set_push_constants(void* data, uint32_t size, Pipeline_Bind_Point bind_point) noexcept override;
    virtual void set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height) noexcept override;
    virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth, [[maybe_unused]] bool vk_invert_viewport = true) noexcept override;

    [[nodiscard]] ID3D12GraphicsCommandList7* get_internal_command_list() const noexcept;

private:
    ID3D12GraphicsCommandList7* m_cmd;
    D3D12_Graphics_Device* m_device;
};
}
