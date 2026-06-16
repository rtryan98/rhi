#pragma once

#include <cstdint>

namespace rhi
{
struct Ray_Tracing_Pipeline_Properties;

struct GPU_Address_Range
{
    uint64_t gpu_address;
    uint64_t size;
    uint64_t stride;
};

struct Shader_Binding_Table
{
    GPU_Address_Range ray_gen;
    GPU_Address_Range miss;
    GPU_Address_Range hit;
    GPU_Address_Range callable;
};

struct Shader_Binding_Table_Layout
{
    uint64_t handle_size;
    uint64_t record_stride;

    uint64_t ray_gen_offset;
    uint64_t ray_gen_size;
    uint64_t miss_offset;
    uint64_t miss_size;
    uint64_t hit_offset;
    uint64_t hit_size;
    uint64_t callable_offset;
    uint64_t callable_size;

    uint32_t ray_gen_count;
    uint32_t miss_count;
    uint32_t hit_count;
    uint32_t callable_count;

    uint64_t total_size;
};

struct Shader_Binding_Table_Selection
{
    uint32_t ray_gen_index;
    uint32_t miss_offset;
    uint32_t hit_offset;
    uint32_t callable_offset;
};

[[nodiscard]] constexpr Shader_Binding_Table_Layout compute_shader_binding_table_layout(
    const Ray_Tracing_Pipeline_Properties& props,
    uint32_t ray_gen_count, uint32_t miss_count,
    uint32_t hit_count, uint32_t callable_count) noexcept;

void write_shader_binding_table(
    const Shader_Binding_Table_Layout& layout,
    const void* group_handles,
    void* mapped_dst) noexcept;

[[nodiscard]] Shader_Binding_Table make_shader_binding_table(
    const Shader_Binding_Table_Layout& layout,
    uint64_t buffer_gpu_address,
    const Shader_Binding_Table_Selection& selection = {}) noexcept;
}
