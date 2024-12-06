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
    case Barrier_Image_Layout::Unordered_Access:
        switch (queue_type)
        {
        case rhi::Queue_Type::Graphics:
            return D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_UNORDERED_ACCESS;
        case rhi::Queue_Type::Compute:
            return D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_UNORDERED_ACCESS;
        default:
            return D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS;
        }
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
    // case Barrier_Image_Layout::Resolve_Src:
    //     return D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE;
    // case Barrier_Image_Layout::Resolve_Dst:
    //     return D3D12_BARRIER_LAYOUT_RESOLVE_DEST;
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
        D3D12_BARRIER_SYNC_DEPTH_STENCIL);
    result |= apply_stages(Barrier_Pipeline_Stage::Late_Fragment_Tests,
        D3D12_BARRIER_SYNC_DEPTH_STENCIL);
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
        // D3D12_BARRIER_ACCESS_SHADER_RESOURCE | // Invalid
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

D3D12_Command_List::D3D12_Command_List(D3D12_Command_List_Underlying_Type cmd, D3D12_Graphics_Device* device) noexcept
    : m_cmd(cmd)
    , m_device(device)
{
    auto type = cmd->GetType();
    switch (type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        m_queue_type = Queue_Type::Graphics;
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        m_queue_type = Queue_Type::Compute;
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        m_queue_type = Queue_Type::Copy;
        break;
    case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:
        m_queue_type = Queue_Type::Video_Decode;
        break;
    case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:
        m_queue_type = Queue_Type::Video_Encode;
        break;
    default:
        break;
    }
}

Graphics_API D3D12_Command_List::get_graphics_api() const noexcept
{
    return Graphics_API::D3D12;
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

uint32_t calculate_subresource(
    uint32_t mip_level,
    uint32_t first_array_index,
    uint32_t plane_slice,
    uint32_t mip_levels,
    uint32_t array_layer_count)
{
    return mip_level + (first_array_index * mip_levels) + (plane_slice * mip_levels * array_layer_count);
}

void D3D12_Command_List::copy_buffer_to_image(
    Buffer* src, uint64_t src_offset,
    Image* dst, const Offset_3D& dst_offset, const Extent_3D& dst_extent,
    uint32_t dst_mip_level, uint32_t dst_array_index) noexcept
{
    if (!src || !dst) return;

    D3D12_TEXTURE_COPY_LOCATION copy_src = {
        .pResource = static_cast<D3D12_Buffer*>(src)->resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = {
            .Offset = src_offset,
            .Footprint = {
                .Format = translate_format(dst->format),
                .Width = dst_extent.x,
                .Height = dst_extent.y,
                .Depth = dst_extent.z,
                .RowPitch = dst_extent.x * get_image_format_info(dst->format).bytes
            }
        }
    };
    D3D12_TEXTURE_COPY_LOCATION copy_dst = {
        .pResource = static_cast<D3D12_Image*>(dst)->resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = calculate_subresource(
            dst_mip_level, dst_array_index, 0, dst->mip_levels, dst->array_size)
    }; // TODO: handle plane count when multi-plane textures are added
    m_cmd->CopyTextureRegion(
        &copy_dst,
        dst_offset.x, dst_offset.y, dst_offset.z,
        &copy_src,
        nullptr);
}

void D3D12_Command_List::copy_image(
    Image* src, const Offset_3D& src_offset, uint32_t src_mip_level, uint32_t src_array_index,
    Image* dst, const Offset_3D& dst_offset, uint32_t dst_mip_level, uint32_t dst_array_index,
    const Extent_3D& extent) noexcept
{
    if (!src || !dst) return;

    D3D12_TEXTURE_COPY_LOCATION copy_src = {
        .pResource = static_cast<D3D12_Image*>(src)->resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = calculate_subresource(
            src_mip_level, src_array_index, 0, src->mip_levels, src->array_size)
    };
    D3D12_TEXTURE_COPY_LOCATION copy_dst = {
        .pResource = static_cast<D3D12_Image*>(dst)->resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = calculate_subresource(
            dst_mip_level, dst_array_index, 0, dst->mip_levels, dst->array_size)
    };
    D3D12_BOX copy_box = {
        .left = uint32_t(src_offset.x),
        .top = uint32_t(src_offset.y),
        .front = uint32_t(src_offset.z),
        .right = uint32_t(src_offset.x + extent.x),
        .bottom = uint32_t(src_offset.y + extent.y),
        .back = uint32_t(src_offset.z + extent.z)
    };
    m_cmd->CopyTextureRegion(
        &copy_dst,
        dst_offset.x, dst_offset.y, dst_offset.z,
        &copy_src,
        &copy_box);
}

void D3D12_Command_List::copy_image_to_buffer(
    Image* src, const Offset_3D& src_offset, const Extent_3D& src_extent,
    uint32_t src_mip_level, uint32_t src_array_index,
    Buffer* dst, uint64_t dst_offset) noexcept
{
    if (!src || !dst) return;

    D3D12_TEXTURE_COPY_LOCATION copy_src = {
        .pResource = static_cast<D3D12_Image*>(src)->resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = calculate_subresource(
            src_mip_level, src_array_index, 0, src->mip_levels, src->array_size)
    }; // TODO: handle plane count when multi-plane textures are added
    D3D12_TEXTURE_COPY_LOCATION copy_dst = {
        .pResource = static_cast<D3D12_Buffer*>(dst)->resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = {
            .Offset = dst_offset,
            .Footprint = {
                .Format = translate_format(src->format),
                .Width = src_extent.x,
                .Height = src_extent.y,
                .Depth = src_extent.z,
                .RowPitch = src_extent.x
            }
        }
    };
    D3D12_BOX copy_box = {
        .left = uint32_t(src_offset.x),
        .top = uint32_t(src_offset.y),
        .front = uint32_t(src_offset.z),
        .right = uint32_t(src_offset.x + src_extent.x),
        .bottom = uint32_t(src_offset.y + src_extent.y),
        .back = uint32_t(src_offset.z + src_extent.z)
    };
    m_cmd->CopyTextureRegion(
        &copy_dst,
        0, 0, 0,
        &copy_src,
        &copy_box);
}

void D3D12_Command_List::fill_buffer(Buffer_View* dst, uint32_t value) noexcept
{
    if (!dst || !dst->buffer) return;

    auto d3d12_dst = static_cast<D3D12_Buffer*>(dst->buffer);
    uint32_t values[4] = { value, value, value, value };
    m_cmd->ClearUnorderedAccessViewUint(
        m_device->get_gpu_descriptor_handle(
            m_device->get_uav_from_bindless_index(dst->bindless_index),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        m_device->get_cpu_descriptor_handle(
            m_device->get_uav_from_bindless_index(dst->bindless_index),
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

void D3D12_Command_List::clear_color_attachment(Image_View* image, float r, float g, float b, float a) noexcept
{
    float rgba[] = {r,g,b,a};
    m_cmd->ClearRenderTargetView(
        m_device->get_cpu_descriptor_handle(
            static_cast<D3D12_Image_View*>(image)->rtv_dsv_index,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
        rgba, 0, nullptr);
}

void D3D12_Command_List::clear_depth_stencil_attachment(Image_View* image, float d, uint8_t s) noexcept
{
    m_cmd->ClearDepthStencilView(
        m_device->get_cpu_descriptor_handle(
            static_cast<D3D12_Image_View*>(image)->rtv_dsv_index,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        d, s, 0, nullptr);
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

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE translate_beginning_access_type(
    Render_Pass_Attachment_Load_Op load_op) noexcept
{
    switch (load_op)
    {
    case Render_Pass_Attachment_Load_Op::Discard:
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    case Render_Pass_Attachment_Load_Op::Load:
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    case Render_Pass_Attachment_Load_Op::Clear:
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
    case Render_Pass_Attachment_Load_Op::No_Access:
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
    default:
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE translate_ending_access_type(
    Render_Pass_Attachment_Store_Op store_op) noexcept
{
    switch (store_op)
    {
    case Render_Pass_Attachment_Store_Op::Discard:
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    case Render_Pass_Attachment_Store_Op::Store:
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    case Render_Pass_Attachment_Store_Op::No_Access:
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
    default:
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
    }
}

D3D12_RENDER_PASS_BEGINNING_ACCESS translate_beginning_access(
    Render_Pass_Attachment_Load_Op
    load_op,
    bool ds,
    Image_Format format,
    const Clear_Value& cv)
{
    D3D12_RENDER_PASS_BEGINNING_ACCESS result = {
        .Type = translate_beginning_access_type(load_op)
    };
    if (load_op == Render_Pass_Attachment_Load_Op::Clear)
    {
        if (!ds)
        {
            result.Clear = {
                .ClearValue = {
                    .Format = translate_format(format),
                    .Color = { cv.color.r, cv.color.g, cv.color.b, cv.color.a }
                }
            };
        }
        else
        {
            result.Clear = {
                .ClearValue = {
                    .Format = translate_format(format),
                    .DepthStencil = {
                        .Depth = cv.depth_stencil.depth,
                        .Stencil = cv.depth_stencil.stencil
                    }
                }
            };
        }
    }
    return result;
}

D3D12_RENDER_PASS_ENDING_ACCESS translate_ending_access(
    Render_Pass_Attachment_Store_Op store_op)
{
    return {
        .Type = translate_ending_access_type(store_op)
    };
}

D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_dsv_cpu_descriptor_handle_safe(
    D3D12_Graphics_Device* device, Image_View* image_view)
{
    if (!image_view)
        return {};

    auto d3d12_image_view = static_cast<D3D12_Image_View*>(image_view);
    auto type = d3d12_image_view->descriptor_type == Descriptor_Type::Color_Attachment
        ? D3D12_DESCRIPTOR_HEAP_TYPE_RTV
        : D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    auto descriptor = device->get_cpu_descriptor_handle(
        d3d12_image_view->rtv_dsv_index, type);
    return descriptor;
}

void D3D12_Command_List::begin_render_pass(const Render_Pass_Begin_Info& begin_info) noexcept
{
    uint32_t render_target_count = uint32_t(begin_info.color_attachments.size());
    std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC,
        PIPELINE_COLOR_ATTACHMENTS_MAX> render_pass_render_targets = {};
    for (auto i = 0; i < render_target_count; ++i)
    {
        const auto& attachment = begin_info.color_attachments[i];
        auto& render_pass_attachment = render_pass_render_targets[i];
        render_pass_attachment.cpuDescriptor = get_rtv_dsv_cpu_descriptor_handle_safe(
            m_device, attachment.attachment);
        render_pass_attachment.BeginningAccess = translate_beginning_access(
            attachment.load_op,
            false,
            attachment.attachment->image->format,
            attachment.clear_value);
        render_pass_attachment.EndingAccess = translate_ending_access(attachment.store_op);
    }
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC render_pass_depth_stencil_attachment = {
        .cpuDescriptor = get_rtv_dsv_cpu_descriptor_handle_safe(
            m_device, begin_info.depth_stencil_attachment.attachment),
        .DepthBeginningAccess = translate_beginning_access(
            begin_info.depth_stencil_attachment.depth_load_op,
            true,
            begin_info.depth_stencil_attachment.attachment != nullptr
                ? begin_info.depth_stencil_attachment.attachment->image->format
                : Image_Format::Undefined,
            begin_info.depth_stencil_attachment.clear_value),
        .StencilBeginningAccess = translate_beginning_access(
            begin_info.depth_stencil_attachment.stencil_load_op,
            true,
            begin_info.depth_stencil_attachment.attachment != nullptr
                ? begin_info.depth_stencil_attachment.attachment->image->format
                : Image_Format::Undefined,
            begin_info.depth_stencil_attachment.clear_value),
        .DepthEndingAccess = translate_ending_access(begin_info.depth_stencil_attachment.depth_store_op),
        .StencilEndingAccess = translate_ending_access(begin_info.depth_stencil_attachment.stencil_store_op),
    };
    m_cmd->BeginRenderPass(
        render_target_count,
        render_pass_render_targets.data(),
        begin_info.depth_stencil_attachment.attachment != nullptr
            ? &render_pass_depth_stencil_attachment : nullptr,
        D3D12_RENDER_PASS_FLAG_NONE);
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
        switch (pipeline->type)
        {
        case Pipeline_Type::Vertex_Shading:
        {
            if (pipeline->vertex_shading_info.depth_stencil_info.depth_bounds_test_mode == Depth_Bounds_Test_Mode::Static)
            {
                set_depth_bounds(
                    pipeline->vertex_shading_info.depth_stencil_info.depth_bounds_min,
                    pipeline->vertex_shading_info.depth_stencil_info.depth_bounds_max
                );
            }
            D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
            switch (pipeline->vertex_shading_info.primitive_topology)
            {
            case Primitive_Topology_Type::Point:
                topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                break;
            case Primitive_Topology_Type::Line:
                topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                break;
            case Primitive_Topology_Type::Triangle:
                topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                break;
            case Primitive_Topology_Type::Patch:
                // TODO: implement tesselation?
                break;
            default:
                break;
            }
            m_cmd->IASetPrimitiveTopology(topology);
            break;
        }
        case Pipeline_Type::Mesh_Shading:
        {
            if (pipeline->mesh_shading_info.depth_stencil_info.depth_bounds_test_mode == Depth_Bounds_Test_Mode::Static)
            {
                set_depth_bounds(
                    pipeline->mesh_shading_info.depth_stencil_info.depth_bounds_min,
                    pipeline->mesh_shading_info.depth_stencil_info.depth_bounds_max
                );
            }
            D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
            switch (pipeline->mesh_shading_info.primitive_topology)
            {
            case Primitive_Topology_Type::Point:
                topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                break;
            case Primitive_Topology_Type::Line:
                topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                break;
            case Primitive_Topology_Type::Triangle:
                topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                break;
            default:
                break;
            }
            m_cmd->IASetPrimitiveTopology(topology);
            break;
        }
        default:
            break;
        }
    }
}

void D3D12_Command_List::set_depth_bounds(float min, float max) noexcept
{
    m_cmd->OMSetDepthBounds(min, max);
}

void D3D12_Command_List::set_index_buffer(Buffer* buffer, Index_Type index_type) noexcept
{
    D3D12_INDEX_BUFFER_VIEW index_buffer_view = {
        .BufferLocation = static_cast<D3D12_Buffer*>(buffer)->resource->GetGPUVirtualAddress(),
        .SizeInBytes = uint32_t(buffer->size),
        .Format = index_type == Index_Type::U16
            ? DXGI_FORMAT_R16_UINT
            : DXGI_FORMAT_R32_UINT
    };
    m_cmd->IASetIndexBuffer(&index_buffer_view);
}

void D3D12_Command_List::set_index_buffer(Buffer* buffer, Index_Type index_type, uint64_t offset, uint64_t size) noexcept
{
    D3D12_INDEX_BUFFER_VIEW index_buffer_view = {
        .BufferLocation = static_cast<D3D12_Buffer*>(buffer)->resource->GetGPUVirtualAddress(),
        .SizeInBytes = uint32_t(size),
        .Format = index_type == Index_Type::U16
            ? DXGI_FORMAT_R16_UINT
            : DXGI_FORMAT_R32_UINT
    };
    auto ptr_offset = offset * ((index_type == Index_Type::U16)
        ? sizeof(uint16_t)
        : sizeof(uint32_t));
    index_buffer_view.BufferLocation += ptr_offset;
    m_cmd->IASetIndexBuffer(&index_buffer_view);
}

void D3D12_Command_List::set_push_constants(
    const void* data, uint32_t size, Pipeline_Bind_Point bind_point) noexcept
{
    switch (bind_point)
    {
    case Pipeline_Bind_Point::Graphics:
        m_cmd->SetGraphicsRoot32BitConstants(0, size / sizeof(uint32_t), data, 0);
        break;
    case Pipeline_Bind_Point::Compute:
        m_cmd->SetComputeRoot32BitConstants(0, size / sizeof(uint32_t), data, 0);
        break;
    case Pipeline_Bind_Point::Ray_Tracing: // TODO: correct command for constants?
        m_cmd->SetComputeRoot32BitConstants(0, size / sizeof(uint32_t), data, 0);
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
    float min_depth, float max_depth) noexcept
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

D3D12_COMMAND_LIST_TYPE translate_command_list_type(Queue_Type queue_type)
{
    switch (queue_type)
    {
    case Queue_Type::Graphics:
        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    case Queue_Type::Compute:
        return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    case Queue_Type::Copy:
        return D3D12_COMMAND_LIST_TYPE_COPY;
    case Queue_Type::Video_Decode:
        return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
    case Queue_Type::Video_Encode:
        return D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE;
    default:
        return D3D12_COMMAND_LIST_TYPE_NONE;
    }
}

D3D12_Command_Pool::D3D12_Command_Pool(
    D3D12_Graphics_Device* device,
    const Command_Pool_Create_Info& create_info) noexcept
    : m_type(translate_command_list_type(create_info.queue_type))
    , m_device(device)
    , m_unused()
    , m_used()
{}

D3D12_Command_Pool::~D3D12_Command_Pool() noexcept
{
    reset();
    for (auto cmd_alloc : m_unused)
    {
        cmd_alloc.cmd->Release();
        cmd_alloc.alloc->Release();
    }
    for (auto cmd_alloc : m_used)
    {
        cmd_alloc.cmd->Release();
        cmd_alloc.alloc->Release();
    }
    m_command_lists.clear();
}

void D3D12_Command_Pool::reset() noexcept
{
    m_unused.insert(m_unused.end(), m_used.begin(), m_used.end());
    m_used.clear();
    m_command_lists.clear();
}

Command_List* D3D12_Command_Pool::acquire_command_list() noexcept
{
    auto cmd_alloc = D3D12_Command_List_Allocator{};
    if (m_unused.empty())
    {
        m_device->get_context()->device->CreateCommandList1(
            0,
            m_type,
            D3D12_COMMAND_LIST_FLAG_NONE,
            IID_PPV_ARGS(&cmd_alloc.cmd));
        m_device->get_context()->device->CreateCommandAllocator(
            m_type, IID_PPV_ARGS(&cmd_alloc.alloc));
    }
    else
    {
        cmd_alloc = m_unused.back();
        m_unused.pop_back();
    }
    m_used.push_back(cmd_alloc);
    cmd_alloc.alloc->Reset();
    cmd_alloc.cmd->Reset(cmd_alloc.alloc, nullptr);
    auto context = m_device->get_context();
    auto descriptor_heaps = std::to_array({
        context->resource_descriptor_heap,
        context->sampler_descriptor_heap
        });
    if (m_type == D3D12_COMMAND_LIST_TYPE_DIRECT || m_type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
    {
        cmd_alloc.cmd->SetDescriptorHeaps(uint32_t(descriptor_heaps.size()), descriptor_heaps.data());
        cmd_alloc.cmd->SetComputeRootSignature(context->bindless_root_signature);
        if (m_type == D3D12_COMMAND_LIST_TYPE_DIRECT)
        {
            cmd_alloc.cmd->SetGraphicsRootSignature(context->bindless_root_signature);
        }
    }
    return m_command_lists.emplace_back(std::make_unique<D3D12_Command_List>(cmd_alloc.cmd, m_device)).get();
}
}
