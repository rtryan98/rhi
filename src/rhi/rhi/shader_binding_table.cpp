#include "rhi/shader_binding_table.hpp"

#include "rhi/graphics_device.hpp"

namespace rhi
{
[[nodiscard]] constexpr uint64_t pow2_align_up(uint64_t x, uint64_t a) noexcept
{
    return (x + (a - 1ull)) & ~(a - 1ull);
}

Shader_Binding_Table_Layout compute_shader_binding_table_layout(
    const Ray_Tracing_Pipeline_Properties& props,
    uint32_t ray_gen_count, uint32_t miss_count,
    uint32_t hit_count, uint32_t callable_count) noexcept
{
    Shader_Binding_Table_Layout layout = {
        .handle_size = props.shader_group_handle_size,
        .record_stride = pow2_align_up(layout.handle_size, props.shader_group_handle_alignment),
        .ray_gen_count = ray_gen_count,
        .miss_count = miss_count,
        .hit_count = hit_count,
        .callable_count = callable_count
    };

    uint64_t offset = 0ull;

    auto compute_region = [&](uint64_t& dst_offset, uint64_t& dst_size, uint32_t count) {
        dst_offset = offset;
        dst_size = pow2_align_up(static_cast<uint64_t>(count) * layout.record_stride, props.shader_group_base_alignment);
        offset += dst_size;
        };

    compute_region(layout.ray_gen_offset, layout.ray_gen_size, ray_gen_count);
    compute_region(layout.miss_offset, layout.miss_size, miss_count);
    compute_region(layout.hit_offset, layout.hit_size, hit_count);
    compute_region(layout.callable_offset, layout.callable_size, callable_count);

    layout.total_size = offset;
    return layout;
}

void write_shader_binding_table(
    const Shader_Binding_Table_Layout& layout,
    const void* group_handles,
    void* mapped_dst) noexcept
{
    const auto* src = static_cast<const uint8_t*>(group_handles);
    auto* dst = static_cast<uint8_t*>(mapped_dst);
    const auto handle_size = layout.handle_size;
    const auto stride = layout.record_stride;

    auto copy_region = [&](uint64_t region_offset, uint32_t count)
        {
            for (uint32_t i = 0; i < count; ++i)
            {
                std::memcpy(dst + region_offset + uint64_t(i) * stride, src, handle_size);
                src += handle_size;
            }
        };
    
    copy_region(layout.hit_offset, layout.hit_count);
    copy_region(layout.ray_gen_offset, layout.ray_gen_count);
    copy_region(layout.miss_offset, layout.miss_count);
    copy_region(layout.callable_offset, layout.callable_count);
}

Shader_Binding_Table make_shader_binding_table(
    const Shader_Binding_Table_Layout& layout,
    uint64_t buffer_gpu_address,
    const Shader_Binding_Table_Selection& selection) noexcept
{
    const uint64_t stride = layout.record_stride;

    auto map_region = [&](uint64_t region_offset, uint32_t count, uint32_t first) -> GPU_Address_Range
        {
            if (count == 0 || first >= count)
                return {};
            return {
                .gpu_address = buffer_gpu_address + region_offset + uint64_t(first) * stride,
                .size = uint64_t(count - first) * stride,
                .stride = stride,
            };
        };

    Shader_Binding_Table shader_binding_table = {};

    shader_binding_table.ray_gen = {
        .gpu_address = buffer_gpu_address + layout.ray_gen_offset + uint64_t(selection.ray_gen_index) * stride,
        .size = stride,
        .stride = stride,
    };
    shader_binding_table.miss = map_region(layout.miss_offset, layout.miss_count, selection.miss_offset);
    shader_binding_table.hit = map_region(layout.hit_offset, layout.hit_count, selection.hit_offset);
    shader_binding_table.callable = map_region(layout.callable_offset, layout.callable_count, selection.callable_offset);
    return shader_binding_table;
}
}
