#pragma once

#include "rhi/d3d12/d3d12_command_list.hpp"
#include "rhi/d3d12/d3d12_graphics_device.hpp"
#include "rhi/d3d12/d3d12_resource.hpp"

#include <array>
#include <vector>
#include <WinPixEventRuntime/pix3.h>

namespace rhi::d3d12
{
auto translate_barrier_Image_layout(
    Barrier_Image_Layout layout,
    Queue_Type queue_type)
{
    switch (layout)
    {
    case Barrier_Image_Layout::Undefined:
        return D3D12_BARRIER_LAYOUT_UNDEFINED;
    case Barrier_Image_Layout::Present:
        return D3D12_BARRIER_LAYOUT_PRESENT;
    case Barrier_Image_Layout::General:
        switch (queue_type)
        {
        case Queue_Type::Graphics:
            return D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COMMON;
        case Queue_Type::Compute:
            return D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_COMMON;
        case Queue_Type::Video_Decode:
            return D3D12_BARRIER_LAYOUT_VIDEO_QUEUE_COMMON;
        case Queue_Type::Video_Encode:
            return D3D12_BARRIER_LAYOUT_VIDEO_QUEUE_COMMON;
        default:
            return D3D12_BARRIER_LAYOUT_COMMON;
        }
    case Barrier_Image_Layout::Color_Attachment:
        return D3D12_BARRIER_LAYOUT_RENDER_TARGET;
    case Barrier_Image_Layout::Depth_Stencil_Read_Only:
        return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
    case Barrier_Image_Layout::Depth_Stencil_Write:
        return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
    case Barrier_Image_Layout::Shader_Read_Only:
        switch (queue_type)
        {
        case Queue_Type::Graphics:
            return D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE;
        case Queue_Type::Compute:
            return D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_SHADER_RESOURCE;
        default:
            return D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
        };
    case Barrier_Image_Layout::Copy_Src:
        switch (queue_type)
        {
        case Queue_Type::Graphics:
            return D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COPY_SOURCE;
        case Queue_Type::Compute:
            return D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_COPY_SOURCE;
        default:
            return D3D12_BARRIER_LAYOUT_COPY_SOURCE;
        }
    case Barrier_Image_Layout::Copy_Dst:
        switch (queue_type)
        {
        case Queue_Type::Graphics:
            return D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COPY_DEST;
        case Queue_Type::Compute:
            return D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_COPY_DEST;
        default:
            return D3D12_BARRIER_LAYOUT_COPY_DEST;
        }
    case Barrier_Image_Layout::Resolve_Src:
        return D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE;
    case Barrier_Image_Layout::Resolve_Dst:
        return D3D12_BARRIER_LAYOUT_RESOLVE_DEST;
    case Barrier_Image_Layout::Shading_Rate_Attachment:
        return D3D12_BARRIER_LAYOUT_SHADING_RATE_SOURCE;
    case Barrier_Image_Layout::Video_Read:
        switch (queue_type)
        {
        case Queue_Type::Video_Decode:
            return D3D12_BARRIER_LAYOUT_VIDEO_DECODE_READ;
        case Queue_Type::Video_Encode:
            return D3D12_BARRIER_LAYOUT_VIDEO_ENCODE_READ;
        default:
            return D3D12_BARRIER_LAYOUT_UNDEFINED;
        }
    case Barrier_Image_Layout::Video_Write:
        switch (queue_type)
        {
        case Queue_Type::Video_Decode:
            return D3D12_BARRIER_LAYOUT_VIDEO_DECODE_WRITE;
        case Queue_Type::Video_Encode:
            return D3D12_BARRIER_LAYOUT_VIDEO_ENCODE_WRITE;
        default:
            return D3D12_BARRIER_LAYOUT_UNDEFINED;
        }
    default:
        return D3D12_BARRIER_LAYOUT_UNDEFINED;
    }
}

auto translate_barrier_pipeline_stage_flags(Barrier_Pipeline_Stage stage)
{
    auto check_stage = [stage](auto flag)
    {
        return ((flag & stage) != Barrier_Pipeline_Stage::None);
    };
    auto apply_stages = [&](auto flag, auto d3d12_flag)
    {
        return check_stage(flag) ? d3d12_flag : D3D12_BARRIER_SYNC_NONE;
    };
    auto result = D3D12_BARRIER_SYNC_NONE;
    result |= apply_stages(Barrier_Pipeline_Stage::Draw_Indirect,
        D3D12_BARRIER_SYNC_EXECUTE_INDIRECT);
    result |= apply_stages(Barrier_Pipeline_Stage::Vertex_Input,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Vertex_Shader,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Hull_Shader,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Domain_Shader,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Geometry_Shader,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Pixel_Shader,
        D3D12_BARRIER_SYNC_PIXEL_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Early_Fragment_Tests,
        D3D12_BARRIER_SYNC_RENDER_TARGET);
    result |= apply_stages(Barrier_Pipeline_Stage::Late_Fragment_Tests,
        D3D12_BARRIER_SYNC_RENDER_TARGET);
    result |= apply_stages(Barrier_Pipeline_Stage::Color_Attachment_Output,
        D3D12_BARRIER_SYNC_RENDER_TARGET);
    result |= apply_stages(Barrier_Pipeline_Stage::Compute_Shader,
        D3D12_BARRIER_SYNC_COMPUTE_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::All_Transfer,
        D3D12_BARRIER_SYNC_COPY |
        D3D12_BARRIER_SYNC_RESOLVE |
        D3D12_BARRIER_SYNC_COPY_RAYTRACING_ACCELERATION_STRUCTURE |
        D3D12_BARRIER_SYNC_RENDER_TARGET);
    result |= apply_stages(Barrier_Pipeline_Stage::Host,
        D3D12_BARRIER_SYNC_NONE);
    result |= apply_stages(Barrier_Pipeline_Stage::All_Graphics,
        D3D12_BARRIER_SYNC_DRAW |
        D3D12_BARRIER_SYNC_EXECUTE_INDIRECT);
    result |= apply_stages(Barrier_Pipeline_Stage::All_Commands,
        D3D12_BARRIER_SYNC_ALL);
    result |= apply_stages(Barrier_Pipeline_Stage::Copy,
        D3D12_BARRIER_SYNC_COPY);
    result |= apply_stages(Barrier_Pipeline_Stage::Resolve,
        D3D12_BARRIER_SYNC_RESOLVE);
    result |= apply_stages(Barrier_Pipeline_Stage::Blit,
        D3D12_BARRIER_SYNC_RENDER_TARGET);
    result |= apply_stages(Barrier_Pipeline_Stage::Clear,
        D3D12_BARRIER_SYNC_RENDER_TARGET);
    result |= apply_stages(Barrier_Pipeline_Stage::Index_Input,
        D3D12_BARRIER_SYNC_INDEX_INPUT);
    result |= apply_stages(Barrier_Pipeline_Stage::Vertex_Attribute_Input,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Pre_Rasterization_Stages,
        D3D12_BARRIER_SYNC_NON_PIXEL_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Video_Decode,
        D3D12_BARRIER_SYNC_VIDEO_DECODE);
    result |= apply_stages(Barrier_Pipeline_Stage::Video_Encode,
        D3D12_BARRIER_SYNC_VIDEO_ENCODE);
    result |= apply_stages(Barrier_Pipeline_Stage::Acceleration_Structure_Build,
        D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE);
    result |= apply_stages(Barrier_Pipeline_Stage::Ray_Tracing_Shader,
        D3D12_BARRIER_SYNC_RAYTRACING);
    result |= apply_stages(Barrier_Pipeline_Stage::Amplification_Shader,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Mesh_Shader,
        D3D12_BARRIER_SYNC_VERTEX_SHADING);
    result |= apply_stages(Barrier_Pipeline_Stage::Acceleration_Structure_Copy,
        D3D12_BARRIER_SYNC_COPY_RAYTRACING_ACCELERATION_STRUCTURE);
    return result;
}

auto translate_barrier_access_flags(Barrier_Access access)
{
    auto check_access = [access](auto flag)
    {
        return ((flag & access) != Barrier_Access::None);
    };
    auto apply_access = [&](auto flag, auto d3d12_flag)
    {
        return check_access(flag) ? d3d12_flag : D3D12_BARRIER_ACCESS_COMMON;
    };

    if (access == Barrier_Access::None)
    {
        return D3D12_BARRIER_ACCESS_NO_ACCESS;
    }

    auto result = D3D12_BARRIER_ACCESS_COMMON;
    result |= apply_access(Barrier_Access::Indirect_Command_Read,
        D3D12_BARRIER_ACCESS_INDIRECT_ARGUMENT);
    result |= apply_access(Barrier_Access::Index_Read,
        D3D12_BARRIER_ACCESS_INDEX_BUFFER);
    result |= apply_access(Barrier_Access::Vertex_Attribute_Read,
        D3D12_BARRIER_ACCESS_VERTEX_BUFFER);
    result |= apply_access(Barrier_Access::Constant_Buffer_View,
        D3D12_BARRIER_ACCESS_CONSTANT_BUFFER);
    result |= apply_access(Barrier_Access::Shader_Read,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE);
    result |= apply_access(Barrier_Access::Shader_Write,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE);
    result |= apply_access(Barrier_Access::Color_Attachment_Read,
        D3D12_BARRIER_ACCESS_RENDER_TARGET);
    result |= apply_access(Barrier_Access::Color_Attachment_Write,
        D3D12_BARRIER_ACCESS_RENDER_TARGET);
    result |= apply_access(Barrier_Access::Depth_Stencil_Attachment_Read,
        D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ);
    result |= apply_access(Barrier_Access::Depth_Stencil_Attachment_Write,
        D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE);
    result |= apply_access(Barrier_Access::Transfer_Read,
        D3D12_BARRIER_ACCESS_COPY_SOURCE);
    result |= apply_access(Barrier_Access::Transfer_Write,
        D3D12_BARRIER_ACCESS_COPY_DEST);
    result |= apply_access(Barrier_Access::Shader_Sampled_Read,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE);
    result |= apply_access(Barrier_Access::Unordered_Access_Read,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE |
        D3D12_BARRIER_ACCESS_UNORDERED_ACCESS);
    result |= apply_access(Barrier_Access::Unordered_Access_Write,
        D3D12_BARRIER_ACCESS_UNORDERED_ACCESS);
    result |= apply_access(Barrier_Access::Video_Decode_Read,
        D3D12_BARRIER_ACCESS_VIDEO_DECODE_READ);
    result |= apply_access(Barrier_Access::Video_Decode_Write,
        D3D12_BARRIER_ACCESS_VIDEO_DECODE_WRITE);
    result |= apply_access(Barrier_Access::Video_Encode_Read,
        D3D12_BARRIER_ACCESS_VIDEO_ENCODE_READ);
    result |= apply_access(Barrier_Access::Video_Encode_Write,
        D3D12_BARRIER_ACCESS_VIDEO_ENCODE_WRITE);
    result |= apply_access(Barrier_Access::Shading_Rate_Attachment,
        D3D12_BARRIER_ACCESS_SHADING_RATE_SOURCE);
    result |= apply_access(Barrier_Access::Acceleration_Structure_Read,
        D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ);
    result |= apply_access(Barrier_Access::Acceleration_Structure_Write,
        D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE);
    return result;
}

Graphics_API D3D12_Command_List::get_graphics_api() const noexcept
{
    return m_device->get_graphics_api();
}

void D3D12_Command_List::barrier(const Barrier_Info& barrier_info) noexcept
{
    uint32_t num_barrier_groups = 0;
    std::array<D3D12_BARRIER_GROUP, 3> barrier_groups = {};
    std::vector<D3D12_BUFFER_BARRIER> buffer_barriers = {};
    std::vector<D3D12_TEXTURE_BARRIER> texture_barriers = {};
    std::vector<D3D12_GLOBAL_BARRIER> global_barriers = {};

    if (barrier_info.buffer_barriers.size() > 0)
    {
        auto& barrier_group = barrier_groups[num_barrier_groups];
        barrier_group.Type = D3D12_BARRIER_TYPE_BUFFER;
        barrier_group.NumBarriers = uint32_t(barrier_info.buffer_barriers.size());
        buffer_barriers.reserve(barrier_group.NumBarriers);
        barrier_group.pBufferBarriers = buffer_barriers.data();
        for (const auto& buffer_barrier : barrier_info.buffer_barriers)
        {
            buffer_barriers.push_back( D3D12_BUFFER_BARRIER {
                .SyncBefore = translate_barrier_pipeline_stage_flags(buffer_barrier.stage_before),
                .SyncAfter = translate_barrier_pipeline_stage_flags(buffer_barrier.stage_after),
                .AccessBefore = translate_barrier_access_flags(buffer_barrier.access_before),
                .AccessAfter = translate_barrier_access_flags(buffer_barrier.access_after),
                .pResource = static_cast<D3D12_Buffer*>(buffer_barrier.buffer)->resource,
                .Offset = 0ull,
                .Size = ~0ull
                });
        }

        num_barrier_groups += 1;
    }

    if (barrier_info.image_barriers.size() > 0)
    {
        auto& barrier_group = barrier_groups[num_barrier_groups];
        barrier_group.Type = D3D12_BARRIER_TYPE_TEXTURE;
        barrier_group.NumBarriers = uint32_t(barrier_info.image_barriers.size());
        texture_barriers.reserve(barrier_group.NumBarriers);
        barrier_group.pTextureBarriers = texture_barriers.data();

        for (const auto& texture_barrier : barrier_info.image_barriers)
        {
            auto queue_type_before = m_queue_type;
            auto queue_type_after = m_queue_type;

            switch (texture_barrier.queue_type_ownership_transfer_mode)
            {
            case Queue_Type_Ownership_Transfer_Mode::Acquire:
                queue_type_after = texture_barrier.queue_type_ownership_transfer_target_queue;
                break;
            case Queue_Type_Ownership_Transfer_Mode::Release:
                queue_type_before = texture_barrier.queue_type_ownership_transfer_target_queue;
                break;
            default:
                break;
            }

            auto layout_before = translate_barrier_Image_layout(
                texture_barrier.layout_before, queue_type_before);
            auto layout_after = translate_barrier_Image_layout(
                texture_barrier.layout_after, queue_type_after);

            if (m_queue_type != queue_type_after)
            {
                layout_after = D3D12_BARRIER_LAYOUT_COMMON;
            }
            if (m_queue_type != queue_type_before)
            {
                layout_before = D3D12_BARRIER_LAYOUT_COMMON;
            }

            texture_barriers.push_back(D3D12_TEXTURE_BARRIER{
                .SyncBefore = translate_barrier_pipeline_stage_flags(texture_barrier.stage_before),
                .SyncAfter = translate_barrier_pipeline_stage_flags(texture_barrier.stage_after),
                .AccessBefore = translate_barrier_access_flags(texture_barrier.access_before),
                .AccessAfter = translate_barrier_access_flags(texture_barrier.access_after),
                .LayoutBefore = layout_before,
                .LayoutAfter = layout_after,
                .pResource = static_cast<D3D12_Image*>(texture_barrier.image)->resource,
                .Subresources = {
                    .IndexOrFirstMipLevel = texture_barrier.subresource_range.first_mip_level,
                    .NumMipLevels = texture_barrier.subresource_range.mip_count,
                    .FirstArraySlice = texture_barrier.subresource_range.first_array_index,
                    .NumArraySlices = texture_barrier.subresource_range.array_size,
                    .FirstPlane = texture_barrier.subresource_range.first_plane,
                    .NumPlanes = texture_barrier.subresource_range.plane_count
                },
                .Flags = texture_barrier.discard
                    ? D3D12_TEXTURE_BARRIER_FLAG_DISCARD
                    : D3D12_TEXTURE_BARRIER_FLAG_NONE
                });
        }

        num_barrier_groups += 1;
    }

    if (barrier_info.memory_barriers.size() > 0)
    {
        auto& barrier_group = barrier_groups[num_barrier_groups];
        barrier_group.Type = D3D12_BARRIER_TYPE_GLOBAL;
        barrier_group.NumBarriers = uint32_t(barrier_info.memory_barriers.size());
        global_barriers.reserve(barrier_group.NumBarriers);
        barrier_group.pGlobalBarriers = global_barriers.data();
        for (const auto& global_barrier : barrier_info.memory_barriers)
        {
            global_barriers.push_back(D3D12_GLOBAL_BARRIER{
                .SyncBefore = translate_barrier_pipeline_stage_flags(global_barrier.stage_before),
                .SyncAfter = translate_barrier_pipeline_stage_flags(global_barrier.stage_after),
                .AccessBefore = translate_barrier_access_flags(global_barrier.access_before),
                .AccessAfter = translate_barrier_access_flags(global_barrier.access_after),
                });
        }

        num_barrier_groups += 1;
    }

    m_cmd->Barrier(num_barrier_groups, barrier_groups.data());
}

void D3D12_Command_List::dispatch(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept
{
    m_cmd->Dispatch(groups_x, groups_y, groups_z);
}

void D3D12_Command_List::dispatch_indirect(Buffer* buffer, uint64_t offset) noexcept
{
    if (!buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().dispatch_indirect,
        1, d3d12_buffer->resource, offset, nullptr, 0);
}

void D3D12_Command_List::dispatch_indirect_d3d12(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    if (!buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().dispatch_indirect,
        count, d3d12_buffer->resource, offset, nullptr, 0);
}

void D3D12_Command_List::dispatch_indirect_count_d3d12(
    Buffer* buffer,
    uint64_t offset,
    uint32_t max_dispatch_count,
    Buffer* count_buffer,
    uint64_t count_offset) noexcept
{
    if (!buffer || !count_buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    auto d3d12_count_buffer = static_cast<D3D12_Buffer*>(count_buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().dispatch_indirect,
        max_dispatch_count, d3d12_buffer->resource, offset, d3d12_count_buffer->resource, count_offset);
}

void D3D12_Command_List::copy_buffer(
    Buffer* src,
    uint64_t src_offset,
    Buffer* dst,
    uint64_t dst_offset,
    uint64_t size) noexcept
{
    if (!src || !dst) return;

    auto d3d12_src = static_cast<D3D12_Buffer*>(src);
    auto d3d12_dst = static_cast<D3D12_Buffer*>(dst);
    m_cmd->CopyBufferRegion(d3d12_dst->resource, dst_offset, d3d12_src->resource, src_offset, size);
}

void D3D12_Command_List::fill_buffer(Buffer* dst, uint32_t value) noexcept
{
    if (!dst) return;

    auto d3d12_dst = static_cast<D3D12_Buffer*>(dst);
    uint32_t values[4] = { value, value, value, value };
    m_cmd->ClearUnorderedAccessViewUint(
        m_device->get_gpu_descriptor_handle(
            m_device->get_uav_from_bindless_index(d3d12_dst->bindless_index),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        m_device->get_cpu_descriptor_handle(
            m_device->get_uav_from_bindless_index(d3d12_dst->bindless_index),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        d3d12_dst->resource,
        values, 0, nullptr);
}

void D3D12_Command_List::begin_debug_region(
    [[maybe_unused]] const char* name,
    [[maybe_unused]] float r,
    [[maybe_unused]] float g,
    [[maybe_unused]] float b) noexcept
{
#ifdef USE_PIX
    PIXBeginEvent(m_cmd, PIX_COLOR(uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255)), name);
#endif
}

void D3D12_Command_List::add_debug_marker(
    [[maybe_unused]] const char* name,
    [[maybe_unused]] float r,
    [[maybe_unused]] float g,
    [[maybe_unused]] float b) noexcept
{
#ifdef USE_PIX
    PIXSetMarker(m_cmd, PIX_COLOR(uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255)), name);
#endif
}

void D3D12_Command_List::end_debug_region() noexcept
{
#ifdef USE_PIX
    PIXEndEvent(m_cmd);
#endif
}

void D3D12_Command_List::draw(
    uint32_t vertex_count,
    uint32_t instance_count,
    uint32_t vertex_offset,
    uint32_t instance_offset) noexcept
{
    m_cmd->DrawInstanced(vertex_count, instance_count, vertex_offset, instance_offset);
}

void D3D12_Command_List::draw_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    if (!buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().draw_indirect,
        count, d3d12_buffer->resource, offset, nullptr, 0);
}

void D3D12_Command_List::draw_indirect_count(
    Buffer* buffer,
    uint64_t offset,
    uint32_t max_draw_count,
    Buffer* count_buffer,
    uint64_t count_offset) noexcept
{
    if (!buffer || !count_buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    auto d3d12_count_buffer = static_cast<D3D12_Buffer*>(count_buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().draw_indirect,
        max_draw_count, d3d12_buffer->resource, offset, d3d12_count_buffer->resource, count_offset);
}

void D3D12_Command_List::draw_indexed(
    uint32_t index_count,
    uint32_t instance_count,
    uint32_t index_offset,
    uint32_t vertex_offset,
    uint32_t instance_offset) noexcept
{
    m_cmd->DrawIndexedInstanced(index_count, instance_count, index_offset, vertex_offset, instance_offset);
}

void D3D12_Command_List::draw_indexed_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    if (!buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().draw_indexed_indirect,
        count, d3d12_buffer->resource, offset, nullptr, 0);
}

void D3D12_Command_List::draw_indexed_indirect_count(
    Buffer* buffer,
    uint64_t offset,
    uint32_t max_draw_count,
    Buffer* count_buffer,
    uint64_t count_offset) noexcept
{
    if (!buffer || !count_buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    auto d3d12_count_buffer = static_cast<D3D12_Buffer*>(count_buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().draw_indexed_indirect,
        max_draw_count, d3d12_buffer->resource, offset, d3d12_count_buffer->resource, count_offset);
}

void D3D12_Command_List::draw_mesh_tasks(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept
{
    m_cmd->DispatchMesh(groups_x, groups_y, groups_z);
}

void D3D12_Command_List::draw_mesh_tasks_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    if (!buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().draw_mesh_tasks_indirect,
        count, d3d12_buffer->resource, offset, nullptr, 0);
}

void D3D12_Command_List::draw_mesh_tasks_indirect_count(
    Buffer* buffer,
    uint64_t offset,
    uint32_t max_draw_count,
    Buffer* count_buffer,
    uint64_t count_offset) noexcept
{
    if (!buffer || !count_buffer) return;

    auto d3d12_buffer = static_cast<D3D12_Buffer*>(buffer);
    auto d3d12_count_buffer = static_cast<D3D12_Buffer*>(count_buffer);
    m_cmd->ExecuteIndirect(m_device->get_indirect_signatures().draw_mesh_tasks_indirect,
        max_draw_count, d3d12_buffer->resource, offset, d3d12_count_buffer->resource, count_offset);
}

void D3D12_Command_List::end_render_pass() noexcept
{
    m_cmd->EndRenderPass();
}

void D3D12_Command_List::set_pipeline(Pipeline* pipeline) noexcept
{
    if (!pipeline) return;

    auto d3d12_pipeline = static_cast<D3D12_Pipeline*>(pipeline);
    if (pipeline->type == Pipeline_Type::Ray_Tracing)
    {
        m_cmd->SetPipelineState1(d3d12_pipeline->rtpso);
    }
    else
    {
        m_cmd->SetPipelineState(d3d12_pipeline->pso);
    }
}

void D3D12_Command_List::set_push_constants(
    void* data, uint32_t size, Pipeline_Bind_Point bind_point) noexcept
{
    switch (bind_point)
    {
    case Pipeline_Bind_Point::Graphics:
        m_cmd->SetGraphicsRoot32BitConstants(0, sizeof(size) / sizeof(uint32_t), data, 0);
        break;
    case Pipeline_Bind_Point::Compute:
        m_cmd->SetComputeRoot32BitConstants(0, sizeof(size) / sizeof(uint32_t), data, 0);
        break;
    case Pipeline_Bind_Point::Ray_Tracing: // TODO: correct command for constants?
        m_cmd->SetComputeRoot32BitConstants(0, sizeof(size) / sizeof(uint32_t), data, 0);
        break;
    default:
        break;
    }
}

void D3D12_Command_List::set_scissor(
    int32_t x, int32_t y,
    uint32_t width, uint32_t height) noexcept
{
    D3D12_RECT scissor = {
        .left = LONG(x),
        .top = LONG(y),
        .right = LONG(x) + LONG(width),
        .bottom = LONG(y) + LONG(height)
    };
    m_cmd->RSSetScissorRects(1, &scissor);
}

void D3D12_Command_List::set_viewport(
    float x, float y,
    float width, float height,
    float min_depth, float max_depth,
    [[maybe_unused]] bool vk_invert_viewport) noexcept
{
    D3D12_VIEWPORT viewport = {
        .TopLeftX = x,
        .TopLeftY = y,
        .Width = float(width),
        .Height = float(height),
        .MinDepth = min_depth,
        .MaxDepth = max_depth,
    };
    m_cmd->RSSetViewports(1, &viewport);
}

ID3D12GraphicsCommandList7* D3D12_Command_List::get_internal_command_list() const noexcept
{
    return m_cmd;
}
}
