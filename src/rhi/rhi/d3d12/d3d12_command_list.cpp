#pragma once

#include "rhi/d3d12/d3d12_command_list.hpp"
#include "rhi/d3d12/d3d12_resource.hpp"

namespace rhi::d3d12
{
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
    int32_t result = D3D12_BARRIER_SYNC_NONE;
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
        return int32_t(D3D12_BARRIER_ACCESS_NO_ACCESS);
    }

    int32_t result = D3D12_BARRIER_ACCESS_COMMON;
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
}
