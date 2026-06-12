#include "rhi/vulkan/vulkan_command_list.hpp"

#include "rhi/vulkan/vulkan_graphics_device.hpp"
#include "rhi/vulkan/vulkan_cast.hpp"

namespace rhi::vulkan
{
Vulkan_Command_List::Vulkan_Command_List(VkCommandBuffer cmd, Vulkan_Graphics_Device* device, Queue_Type queue_type) noexcept
    : m_cmd(cmd)
    , m_device(device)
    , m_bound_image_views()
{
    m_queue_type = queue_type;
}

Graphics_API Vulkan_Command_List::get_graphics_api() const noexcept
{
    return Graphics_API::Vulkan;
}

auto translate_barrier_Image_layout(Barrier_Image_Layout layout)
{
    switch (layout)
    {
    case Barrier_Image_Layout::Undefined:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    case Barrier_Image_Layout::Present:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case Barrier_Image_Layout::General:
        return VK_IMAGE_LAYOUT_GENERAL;
    case Barrier_Image_Layout::Color_Attachment:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case Barrier_Image_Layout::Depth_Stencil_Read_Only:
        return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
    case Barrier_Image_Layout::Depth_Stencil_Write:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case Barrier_Image_Layout::Shader_Read_Only:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case Barrier_Image_Layout::Unordered_Access:
        return VK_IMAGE_LAYOUT_GENERAL;
    case Barrier_Image_Layout::Copy_Src:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case Barrier_Image_Layout::Copy_Dst:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case Barrier_Image_Layout::Shading_Rate_Attachment:
        return VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
    case Barrier_Image_Layout::Video_Read:
        return VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR;
    case Barrier_Image_Layout::Video_Write:
        return VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR;
    default:
        return VK_IMAGE_LAYOUT_UNDEFINED;
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
            return check_stage(flag) ? d3d12_flag : VK_PIPELINE_STAGE_2_NONE;
        };
    auto result = VK_PIPELINE_STAGE_2_NONE;
    result |= apply_stages(Barrier_Pipeline_Stage::Draw_Indirect,
        VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Vertex_Input,
        VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Vertex_Shader,
        VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Hull_Shader,
        VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Domain_Shader,
        VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Geometry_Shader,
        VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Pixel_Shader,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Early_Fragment_Tests,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Late_Fragment_Tests,
        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Color_Attachment_Output,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Compute_Shader,
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::All_Transfer,
        VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Host,
        VK_PIPELINE_STAGE_2_HOST_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::All_Graphics,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::All_Commands,
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Copy,
        VK_PIPELINE_STAGE_2_COPY_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Resolve,
        VK_PIPELINE_STAGE_2_RESOLVE_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Blit,
        VK_PIPELINE_STAGE_2_BLIT_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Clear,
        VK_PIPELINE_STAGE_2_CLEAR_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Index_Input,
        VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Vertex_Attribute_Input,
        VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Pre_Rasterization_Stages,
        VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT);
    result |= apply_stages(Barrier_Pipeline_Stage::Video_Decode,
        VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR);
    result |= apply_stages(Barrier_Pipeline_Stage::Video_Encode,
        VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR);
    result |= apply_stages(Barrier_Pipeline_Stage::Acceleration_Structure_Build,
        VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR);
    result |= apply_stages(Barrier_Pipeline_Stage::Ray_Tracing_Shader,
        VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR);
    result |= apply_stages(Barrier_Pipeline_Stage::Amplification_Shader,
        VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT);
    result |= apply_stages(Barrier_Pipeline_Stage::Mesh_Shader,
        VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT);
    result |= apply_stages(Barrier_Pipeline_Stage::Acceleration_Structure_Copy,
        VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR);
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
            return check_access(flag) ? d3d12_flag : VK_ACCESS_2_NONE;
        };

    if (access == Barrier_Access::None)
    {
        return VK_ACCESS_2_NONE;
    }

    auto result = VK_ACCESS_2_NONE;
    result |= apply_access(Barrier_Access::Indirect_Command_Read,
        VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT);
    result |= apply_access(Barrier_Access::Index_Read,
        VK_ACCESS_2_INDEX_READ_BIT);
    result |= apply_access(Barrier_Access::Vertex_Attribute_Read,
        VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT);
    result |= apply_access(Barrier_Access::Constant_Buffer_View,
        VK_ACCESS_2_UNIFORM_READ_BIT);
    result |= apply_access(Barrier_Access::Shader_Read,
        VK_ACCESS_2_SHADER_READ_BIT);
    result |= apply_access(Barrier_Access::Shader_Write,
        VK_ACCESS_2_SHADER_WRITE_BIT);
    result |= apply_access(Barrier_Access::Color_Attachment_Read,
        VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT);
    result |= apply_access(Barrier_Access::Color_Attachment_Write,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
    result |= apply_access(Barrier_Access::Depth_Stencil_Attachment_Read,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
    result |= apply_access(Barrier_Access::Depth_Stencil_Attachment_Write,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    result |= apply_access(Barrier_Access::Transfer_Read,
        VK_ACCESS_2_TRANSFER_READ_BIT);
    result |= apply_access(Barrier_Access::Transfer_Write,
        VK_ACCESS_2_TRANSFER_WRITE_BIT);
    result |= apply_access(Barrier_Access::Shader_Sampled_Read,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
    result |= apply_access(Barrier_Access::Unordered_Access_Read,
        VK_ACCESS_2_SHADER_STORAGE_READ_BIT);
    result |= apply_access(Barrier_Access::Unordered_Access_Write,
        VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT);
    result |= apply_access(Barrier_Access::Video_Decode_Read,
        VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR);
    result |= apply_access(Barrier_Access::Video_Decode_Write,
        VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR);
    result |= apply_access(Barrier_Access::Video_Encode_Read,
        VK_ACCESS_2_VIDEO_ENCODE_READ_BIT_KHR);
    result |= apply_access(Barrier_Access::Video_Encode_Write,
        VK_ACCESS_2_VIDEO_ENCODE_WRITE_BIT_KHR);
    result |= apply_access(Barrier_Access::Shading_Rate_Attachment,
        VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR);
    result |= apply_access(Barrier_Access::Acceleration_Structure_Read,
        VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR);
    result |= apply_access(Barrier_Access::Acceleration_Structure_Write,
        VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR);
    return result;
}

void Vulkan_Command_List::barrier(const Barrier_Info& barrier_info) noexcept
{
    std::vector<VkMemoryBarrier2> memory_barriers;
    memory_barriers.reserve(barrier_info.memory_barriers.size());
    for (const auto& memory_barrier : barrier_info.memory_barriers)
    {
        memory_barriers.push_back({
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = translate_barrier_pipeline_stage_flags(memory_barrier.stage_before),
            .srcAccessMask = translate_barrier_access_flags(memory_barrier.access_before),
            .dstStageMask = translate_barrier_pipeline_stage_flags(memory_barrier.stage_after),
            .dstAccessMask = translate_barrier_access_flags(memory_barrier.access_after)
            });
    }

    std::vector<VkBufferMemoryBarrier2> buffer_memory_barriers;
    buffer_memory_barriers.reserve(barrier_info.buffer_barriers.size());
    for (const auto& buffer_memory_barrier : barrier_info.buffer_barriers)
    {
        buffer_memory_barriers.push_back({
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = translate_barrier_pipeline_stage_flags(buffer_memory_barrier.stage_before),
            .srcAccessMask = translate_barrier_access_flags(buffer_memory_barrier.access_before),
            .dstStageMask = translate_barrier_pipeline_stage_flags(buffer_memory_barrier.stage_after),
            .dstAccessMask = translate_barrier_access_flags(buffer_memory_barrier.access_after),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = static_cast<Vulkan_Buffer*>(buffer_memory_barrier.buffer)->buffer,
            .offset = 0,
            .size = VK_WHOLE_SIZE
            });
    }

    std::vector<VkImageMemoryBarrier2> image_memory_barriers;
    image_memory_barriers.reserve(barrier_info.image_barriers.size());
    for (const auto& image_memory_barrier : barrier_info.image_barriers)
    {
        auto queue_type_before = m_queue_type;
        auto queue_type_after = m_queue_type;

        switch (image_memory_barrier.queue_type_ownership_transfer_mode)
        {
        case Queue_Type_Ownership_Transfer_Mode::Acquire:
            queue_type_after = image_memory_barrier.queue_type_ownership_transfer_target_queue;
            break;
        case Queue_Type_Ownership_Transfer_Mode::Release:
            queue_type_before = image_memory_barrier.queue_type_ownership_transfer_target_queue;
            break;
        default:
            break;
        }

        image_memory_barriers.push_back({
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = translate_barrier_pipeline_stage_flags(image_memory_barrier.stage_before),
            .srcAccessMask = translate_barrier_access_flags(image_memory_barrier.access_before),
            .dstStageMask = translate_barrier_pipeline_stage_flags(image_memory_barrier.stage_after),
            .dstAccessMask = translate_barrier_access_flags(image_memory_barrier.access_after),
            .oldLayout = translate_barrier_Image_layout(image_memory_barrier.layout_before),
            .newLayout = translate_barrier_Image_layout(image_memory_barrier.layout_after),
            .srcQueueFamilyIndex = m_device->get_queue_family_index(vulkan_cast<VkQueueFlagBits>(queue_type_before)),
            .dstQueueFamilyIndex = m_device->get_queue_family_index(vulkan_cast<VkQueueFlagBits>(queue_type_after)),
            .image = static_cast<Vulkan_Image*>(image_memory_barrier.image)->image,
            .subresourceRange = {
                .aspectMask = get_aspect_mask(image_memory_barrier.image),
                .baseMipLevel = image_memory_barrier.subresource_range.first_mip_level,
                .levelCount = image_memory_barrier.subresource_range.mip_count > 0
                    ? image_memory_barrier.subresource_range.mip_count
                    : VK_REMAINING_MIP_LEVELS,
                .baseArrayLayer = image_memory_barrier.subresource_range.first_array_index,
                .layerCount = image_memory_barrier.subresource_range.array_size > 0
                    ? image_memory_barrier.subresource_range.array_size
                    : VK_REMAINING_ARRAY_LAYERS
            }
            });
    }

    VkDependencyInfo dependency = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = nullptr,
        .dependencyFlags = 0,
        .memoryBarrierCount = static_cast<uint32_t>(memory_barriers.size()),
        .pMemoryBarriers = memory_barriers.data(),
        .bufferMemoryBarrierCount = static_cast<uint32_t>(buffer_memory_barriers.size()),
        .pBufferMemoryBarriers = buffer_memory_barriers.data(),
        .imageMemoryBarrierCount = static_cast<uint32_t>(image_memory_barriers.size()),
        .pImageMemoryBarriers = image_memory_barriers.data()
    };
    vkCmdPipelineBarrier2(m_cmd, &dependency);
}

void Vulkan_Command_List::dispatch(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept
{
    vkCmdDispatch(m_cmd, groups_x, groups_y, groups_z);
}

void Vulkan_Command_List::dispatch_indirect(Buffer* buffer, uint64_t offset) noexcept
{
    vkCmdDispatchIndirect(m_cmd, static_cast<Vulkan_Buffer*>(buffer)->buffer, offset);
}

void Vulkan_Command_List::copy_buffer(Buffer* src, uint64_t src_offset, Buffer* dst, uint64_t dst_offset, uint64_t size) noexcept
{
    VkBufferCopy region = {
        .srcOffset = src_offset,
        .dstOffset = dst_offset,
        .size = size
    };
    vkCmdCopyBuffer(m_cmd,
        static_cast<Vulkan_Buffer*>(src)->buffer,
        static_cast<Vulkan_Buffer*>(dst)->buffer,
        1,
        &region);
}

void Vulkan_Command_List::copy_buffer_to_image(
    Buffer* src, uint64_t src_offset,
    Image* dst, const Offset_3D& dst_offset, const Extent_3D& dst_extent,
    uint32_t dst_mip_level, uint32_t dst_array_index) noexcept
{
    VkBufferImageCopy region = {
        .bufferOffset = src_offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = get_aspect_mask(dst),
            .mipLevel = dst_mip_level,
            .baseArrayLayer = dst_array_index,
            .layerCount = 1
        },
        .imageOffset = {
            .x = dst_offset.x,
            .y = dst_offset.y,
            .z = dst_offset.z,
        },
        .imageExtent = {
            .width = dst_extent.x,
            .height = dst_extent.y,
            .depth = dst_extent.z,
        }
    };
    vkCmdCopyBufferToImage(m_cmd,
        static_cast<Vulkan_Buffer*>(src)->buffer,
        static_cast<Vulkan_Image*>(dst)->image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

void Vulkan_Command_List::copy_image(
    Image* src, const Offset_3D& src_offset, uint32_t src_mip_level, uint32_t src_array_index,
    Image* dst, const Offset_3D& dst_offset, uint32_t dst_mip_level, uint32_t dst_array_index,
    const Extent_3D& extent) noexcept
{
    VkImageCopy region = {
        .srcSubresource = {
            .aspectMask = get_aspect_mask(src),
            .mipLevel = src_mip_level,
            .baseArrayLayer = src_array_index,
            .layerCount = 1
        },
        .srcOffset = {
            .x = src_offset.x,
            .y = src_offset.y,
            .z = src_offset.z,
        },
        .dstSubresource = {
            .aspectMask = get_aspect_mask(dst),
            .mipLevel = dst_mip_level,
            .baseArrayLayer = dst_array_index,
            .layerCount = 1
        },
        .dstOffset = {
            .x = dst_offset.x,
            .y = dst_offset.y,
            .z = dst_offset.z,
        },
        .extent = {
            .width = extent.x,
            .height = extent.y,
            .depth = extent.z,
        }
    };
    vkCmdCopyImage(m_cmd,
        static_cast<Vulkan_Image*>(src)->image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        static_cast<Vulkan_Image*>(dst)->image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

void Vulkan_Command_List::copy_image_to_buffer(
    Image* src, const Offset_3D& src_offset, const Extent_3D& src_extent,
    uint32_t src_mip_level, uint32_t src_array_index,
    Buffer* dst, uint64_t dst_offset) noexcept
{
    VkBufferImageCopy region = {
        .bufferOffset = dst_offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = get_aspect_mask(src),
            .mipLevel = src_mip_level,
            .baseArrayLayer = src_array_index,
            .layerCount = 1
        },
        .imageOffset = {
            .x = src_offset.x,
            .y = src_offset.y,
            .z = src_offset.z,
        },
        .imageExtent = {
            .width = src_extent.x,
            .height = src_extent.y,
            .depth = src_extent.z,
        }
    };
    vkCmdCopyImageToBuffer(m_cmd,
        static_cast<Vulkan_Image*>(src)->image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        static_cast<Vulkan_Buffer*>(dst)->buffer,
        1,
        &region);
}

void Vulkan_Command_List::fill_buffer(Buffer_View* dst, uint32_t value) noexcept
{
    vkCmdFillBuffer(m_cmd, static_cast<Vulkan_Buffer*>(dst->buffer)->buffer, dst->offset, dst->size, value);
}

void Vulkan_Command_List::begin_debug_region(const char* name, float r, float g, float b) noexcept
{
    VkDebugMarkerMarkerInfoEXT marker = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
        .pNext = nullptr,
        .pMarkerName = name,
        .color = { r, g, b, 1.f }
    };
    vkCmdDebugMarkerBeginEXT(m_cmd, &marker);
}

void Vulkan_Command_List::add_debug_marker(const char* name, float r, float g, float b)  noexcept
{
    VkDebugMarkerMarkerInfoEXT marker = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
        .pNext = nullptr,
        .pMarkerName = name,
        .color = { r, g, b, 1.f }
    };
    vkCmdDebugMarkerInsertEXT(m_cmd, &marker);
}

void Vulkan_Command_List::end_debug_region() noexcept
{
    vkCmdDebugMarkerEndEXT(m_cmd);
}

void Vulkan_Command_List::clear_color_attachment(Image_View* image, float r, float g, float b, float a) noexcept
{
    // TODO: change how indexing works?
    uint32_t color_attachment_index = 0;
    for (const auto [index, bound_view] : std::views::enumerate(m_bound_image_views))
    {
        if (bound_view == image)
        {
            color_attachment_index = static_cast<uint32_t>(index);
        }
    }

    VkClearAttachment attachment = {
        .aspectMask = get_aspect_mask(image->image),
        .colorAttachment = color_attachment_index,
        .clearValue = {
            .color = {
                .float32 = { r, g, b, a }
            }
        }
    };

    // TODO: add options?
    VkClearRect rect = {
        .rect = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = {
                .width = image->image->width,
                .height = image->image->height
            }
        },
        .baseArrayLayer = 0,
        .layerCount = 1
    };
    vkCmdClearAttachments(m_cmd, 1, &attachment, 1, &rect);
}

void Vulkan_Command_List::clear_depth_stencil_attachment(Image_View* image, float d, uint8_t s) noexcept
{
    // TODO: rework clearing of images
    VkClearAttachment attachment = {
        .aspectMask = get_aspect_mask(image->image),
        .colorAttachment = 0,
        .clearValue = {
            .depthStencil = {
                .depth = d,
                .stencil = s
            }
        }
    };

    VkClearRect rect = {
        .rect = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = {
                .width = image->image->width,
                .height = image->image->height
            }
        },
        .baseArrayLayer = 0,
        .layerCount = 1
    };
    vkCmdClearAttachments(m_cmd, 1, &attachment, 1, &rect);
}

void Vulkan_Command_List::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) noexcept
{
    vkCmdDraw(m_cmd, vertex_count, instance_count, vertex_offset, instance_offset);
}

void Vulkan_Command_List::draw_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    vkCmdDrawIndirect(m_cmd, static_cast<Vulkan_Buffer*>(buffer)->buffer, offset, count, INDIRECT_ARGUMENT_STRIDE);
}

void Vulkan_Command_List::draw_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept
{
    vkCmdDrawIndirectCount(m_cmd,
        static_cast<Vulkan_Buffer*>(buffer)->buffer,
        offset,
        static_cast<Vulkan_Buffer*>(count_buffer)->buffer,
        count_offset,
        max_draw_count,
        INDIRECT_ARGUMENT_STRIDE);
}

void Vulkan_Command_List::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t index_offset, uint32_t vertex_offset, uint32_t instance_offset) noexcept
{
    vkCmdDrawIndexed(m_cmd, index_count, instance_count, index_offset, vertex_offset, instance_offset);
}

void Vulkan_Command_List::draw_indexed_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    vkCmdDrawIndexedIndirect(m_cmd, static_cast<Vulkan_Buffer*>(buffer)->buffer, offset, count, INDIRECT_ARGUMENT_STRIDE);
}

void Vulkan_Command_List::draw_indexed_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept
{
    vkCmdDrawIndexedIndirectCount(m_cmd,
        static_cast<Vulkan_Buffer*>(buffer)->buffer,
        offset,
        static_cast<Vulkan_Buffer*>(count_buffer)->buffer,
        count_offset,
        max_draw_count,
        INDIRECT_ARGUMENT_STRIDE);
}

void Vulkan_Command_List::draw_mesh_tasks(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept
{
    vkCmdDrawMeshTasksEXT(m_cmd, groups_x, groups_y, groups_z);
}

void Vulkan_Command_List::draw_mesh_tasks_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept
{
    vkCmdDrawMeshTasksIndirectEXT(m_cmd, static_cast<Vulkan_Buffer*>(buffer)->buffer, offset, count, INDIRECT_ARGUMENT_STRIDE);
}

void Vulkan_Command_List::draw_mesh_tasks_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept
{
    vkCmdDrawMeshTasksIndirectCountEXT(m_cmd,
        static_cast<Vulkan_Buffer*>(buffer)->buffer,
        offset,
        static_cast<Vulkan_Buffer*>(count_buffer)->buffer,
        count_offset,
        max_draw_count,
        INDIRECT_ARGUMENT_STRIDE);
}

void Vulkan_Command_List::begin_render_pass(const Render_Pass_Begin_Info& begin_info) noexcept
{
    std::vector<VkRenderingAttachmentInfo> attachments;
    attachments.reserve(begin_info.color_attachments.size());
    auto attachment_index = 0;
    for (const auto& attachment : begin_info.color_attachments)
    {
        attachments.push_back({
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = static_cast<Vulkan_Image_View*>(attachment.attachment)->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = vulkan_cast<VkAttachmentLoadOp>(attachment.load_op),
            .storeOp = vulkan_cast<VkAttachmentStoreOp>(attachment.store_op),
            .clearValue = {
                .color = {
                    .float32 = { 
                        attachment.clear_value.color.r,
                        attachment.clear_value.color.g,
                        attachment.clear_value.color.b,
                        attachment.clear_value.color.a
                    }
                }
            }
        });

        m_bound_image_views[attachment_index++] = attachment.attachment;
    }

    VkRenderingAttachmentInfo depth_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = begin_info.depth_stencil_attachment.attachment != nullptr
            ? static_cast<Vulkan_Image_View*>(begin_info.depth_stencil_attachment.attachment)->image_view
            : VK_NULL_HANDLE,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = vulkan_cast<VkAttachmentLoadOp>(begin_info.depth_stencil_attachment.depth_load_op),
        .storeOp = vulkan_cast<VkAttachmentStoreOp>(begin_info.depth_stencil_attachment.depth_store_op),
        .clearValue = {
            .depthStencil = {
                .depth = begin_info.depth_stencil_attachment.clear_value.depth_stencil.depth,
                .stencil = begin_info.depth_stencil_attachment.clear_value.depth_stencil.stencil
            }
        }
    };

    VkRect2D render_area = {};
    if (begin_info.color_attachments.size())
    {
        render_area = {
            .offset = {},
            .extent = {
                begin_info.color_attachments[0].attachment->image->width,
                begin_info.color_attachments[0].attachment->image->height
            }
        };
    }
    else if (begin_info.depth_stencil_attachment.attachment != nullptr)
    {
        render_area = {
            .offset = {},
            .extent = {
                begin_info.depth_stencil_attachment.attachment->image->width,
                begin_info.depth_stencil_attachment.attachment->image->height
            }
        };
    }

    auto depth_stencil_format_info = get_image_format_info(begin_info.depth_stencil_attachment.attachment
        ? begin_info.depth_stencil_attachment.attachment->image->format
        : Image_Format::Undefined);
    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = render_area,
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(attachments.size()),
        .pColorAttachments = attachments.data(),
        .pDepthAttachment = depth_stencil_format_info.is_depth ? &depth_attachment : nullptr,
        .pStencilAttachment = depth_stencil_format_info.is_stencil ? &depth_attachment : nullptr
    };
    vkCmdBeginRendering(m_cmd, &rendering_info);
}

void Vulkan_Command_List::end_render_pass() noexcept
{
    vkCmdEndRendering(m_cmd);
    m_bound_image_views = {};
}

void Vulkan_Command_List::set_pipeline(Pipeline* pipeline) noexcept
{
    auto bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    switch (pipeline->type)
    {
    case Pipeline_Type::Compute:
        bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
        break;
    case Pipeline_Type::Ray_Tracing:
        bind_point = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
        break;
    default:
        break;
    }
    vkCmdBindPipeline(m_cmd, bind_point, static_cast<Vulkan_Pipeline*>(pipeline)->pipeline);
}

void Vulkan_Command_List::set_depth_bounds(float min, float max) noexcept
{
    vkCmdSetDepthBounds(m_cmd, min, max);
}

void Vulkan_Command_List::set_index_buffer(Buffer* buffer, Index_Type index_type) noexcept
{
    vkCmdBindIndexBuffer(m_cmd, static_cast<Vulkan_Buffer*>(buffer)->buffer, 0,
        index_type == Index_Type::U16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
}

void Vulkan_Command_List::set_index_buffer(Buffer* buffer, Index_Type index_type, uint64_t offset, uint64_t size) noexcept
{
    auto vulkan_index_type = (index_type == Index_Type::U16) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer2(m_cmd, static_cast<Vulkan_Buffer*>(buffer)->buffer, offset, size, vulkan_index_type);
}

void Vulkan_Command_List::set_push_constants(const void* data, uint32_t size, [[maybe_unused]] Pipeline_Bind_Point bind_point) noexcept
{
    // `bind_point` is not required in Vulkan. Pipeline layout dictates push constants.
    vkCmdPushConstants(m_cmd, m_device->get_pipeline_layout(), VK_SHADER_STAGE_ALL, 0, size, data);
}

void Vulkan_Command_List::set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height) noexcept
{
    VkRect2D rect = {
        .offset = {
            .x = x,
            .y = y
        },
        .extent = {
            .width = width,
            .height = height
        }
    };
    vkCmdSetScissor(m_cmd, 0, 1, &rect);
}

void Vulkan_Command_List::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) noexcept
{
    VkViewport viewport = {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
        .minDepth = min_depth,
        .maxDepth = max_depth
    };
    vkCmdSetViewport(m_cmd, 0, 1, &viewport);
}

void Vulkan_Command_List::set_stencil_reference(uint8_t reference) noexcept
{
    vkCmdSetStencilReference(m_cmd, VK_STENCIL_FACE_FRONT_AND_BACK, reference);
}

void Vulkan_Command_List::build_acceleration_structure(
    const Acceleration_Structure_Build_Geometry_Info& build_info, uint64_t scratch_memory_address) noexcept
{
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> build_ranges;
    std::vector<VkAccelerationStructureGeometryKHR> geometries;

    if (build_info.type == Acceleration_Structure_Type::Bottom_Level)
    {
        geometries.reserve(build_info.geometry_or_instance_count);
        build_ranges.reserve(build_info.geometry_or_instance_count);

        for (auto i = 0; i < build_info.geometry_or_instance_count; ++i)
        {
            auto& geometry = build_info.geometry[i];
            auto& vulkan_geometry = geometries.emplace_back();
            auto& build_range = build_ranges.emplace_back();

            vulkan_geometry = {
                .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
                .pNext = nullptr,
                .flags = std::bit_cast<VkGeometryFlagsKHR>(geometry.flags)
            };

            if (geometry.type == Acceleration_Structure_Geometry_Type::Triangles)
            {
                const auto& triangles = geometry.geometry.triangles;

                vulkan_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
                vulkan_geometry.geometry = {
                    .triangles = {
                        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
                        .pNext = nullptr,
                        .vertexFormat = vulkan_cast<VkFormat>(triangles.vertex_format),
                        .vertexData = triangles.vertex_gpu_address,
                        .vertexStride = triangles.vertex_stride,
                        .maxVertex = triangles.vertex_count - 1, // TODO: spec mentions amount of vertices in vertexData - 1
                        .indexType = triangles.index_type == Index_Type::U16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32,
                        .indexData = triangles.index_gpu_address,
                        .transformData = triangles.transform_gpu_address
                    }
                };
                build_range = {
                    .primitiveCount = triangles.index_count / 3,
                    .primitiveOffset = 0,
                    .firstVertex = 0,
                    .transformOffset = 0
                };
            }
            else
            {
                const auto& aabbs = geometry.geometry.aabbs;

                vulkan_geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
                vulkan_geometry.geometry = {
                    .aabbs = {
                        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR,
                        .pNext = nullptr,
                        .data = aabbs.aabb_gpu_address,
                        .stride = aabbs.aabb_stride
                    }
                };
                build_range = {
                    .primitiveCount = static_cast<uint32_t>(aabbs.aabb_count),
                    .primitiveOffset = 0,
                    .firstVertex = 0,
                    .transformOffset = 0
                };
            }
        }
    }
    else // TLAS
    {
        auto& vulkan_geometry = geometries.emplace_back();
        auto& build_range = build_ranges.emplace_back();

        vulkan_geometry = {
            .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
            .pNext = nullptr,
            .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
            .geometry = {
                .instances = {
                    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
                    .pNext = nullptr,
                    .arrayOfPointers = build_info.instances.array_of_pointers,
                    .data = build_info.instances.instance_gpu_address
                }
            },
            .flags = 0
        };

        build_range = {
            .primitiveCount = build_info.geometry_or_instance_count,
            .primitiveOffset = 0,
            .firstVertex = 0,
            .transformOffset = 0
        };
    }

    VkBuildAccelerationStructureFlagsKHR flags = 0;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Allow_Update) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Allow_Compaction) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Fast_Trace) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    if (static_cast<uint32_t>(build_info.flags & Acceleration_Structure_Flags::Fast_Build) > 0u)
        flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;

    VkAccelerationStructureBuildGeometryInfoKHR build_geometry_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
        .pNext = nullptr,
        .type = vulkan_cast<VkAccelerationStructureTypeKHR>(build_info.type),
        .flags = flags,
        .mode = build_info.src != nullptr
            ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
            : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
        .srcAccelerationStructure = build_info.src != nullptr
            ? static_cast<Vulkan_Acceleration_Structure*>(build_info.src)->acceleration_structure
            : VK_NULL_HANDLE,
        .dstAccelerationStructure = build_info.dst != nullptr
            ? static_cast<Vulkan_Acceleration_Structure*>(build_info.dst)->acceleration_structure
            : VK_NULL_HANDLE,
        .geometryCount = build_info.type == Acceleration_Structure_Type::Bottom_Level
            ? build_info.geometry_or_instance_count
            : 1,
        .pGeometries = geometries.data(),
        .ppGeometries = nullptr,
        .scratchData = scratch_memory_address
    };

    auto* build_ranges_ptrs = build_ranges.data();
    vkCmdBuildAccelerationStructuresKHR(
        m_cmd, 1, &build_geometry_info, &build_ranges_ptrs);
}

void Vulkan_Command_List::dispatch_rays(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z, const Shader_Binding_Table& sbt) noexcept
{
    VkStridedDeviceAddressRegionKHR sbt_ray_gen = {
        .deviceAddress = sbt.ray_gen.gpu_address,
        .stride = sbt.ray_gen.stride,
        .size = sbt.ray_gen.size
    };
    VkStridedDeviceAddressRegionKHR sbt_miss = {
        .deviceAddress = sbt.miss.gpu_address,
        .stride = sbt.miss.stride,
        .size = sbt.miss.size
    };
    VkStridedDeviceAddressRegionKHR sbt_hit = {
        .deviceAddress = sbt.hit.gpu_address,
        .stride = sbt.hit.stride,
        .size = sbt.hit.size
    };
    VkStridedDeviceAddressRegionKHR sbt_callable = {
        .deviceAddress = sbt.callable.gpu_address,
        .stride = sbt.callable.stride,
        .size = sbt.callable.size
    };
    vkCmdTraceRaysKHR(m_cmd, &sbt_ray_gen, &sbt_miss, &sbt_hit, &sbt_callable, groups_x, groups_y, groups_z);
}

VkCommandBuffer Vulkan_Command_List::get_internal_command_list() const noexcept
{
    return m_cmd;
}

Vulkan_Command_Pool::Vulkan_Command_Pool(
    Vulkan_Graphics_Device* device,
    const Command_Pool_Create_Info& create_info) noexcept
    : m_queue_type(create_info.queue_type)
    , m_device(device)
    , m_unused()
    , m_used()
{}

Vulkan_Command_Pool::~Vulkan_Command_Pool() noexcept
{
    reset();
    for (auto cmd_alloc : m_unused)
    {
        vkDestroyCommandPool(*m_device, cmd_alloc.pool, nullptr);
    }
    for (auto cmd_alloc : m_used)
    {
        vkDestroyCommandPool(*m_device, cmd_alloc.pool, nullptr);
    }
    m_command_lists.clear();
}

void Vulkan_Command_Pool::reset() noexcept
{
    m_unused.insert(m_unused.end(), m_used.begin(), m_used.end());
    m_used.clear();
    m_command_lists.clear();
}

Command_List* Vulkan_Command_Pool::acquire_command_list() noexcept
{
    auto cmd_alloc = Vulkan_Command_List_Allocator{};
    if (m_unused.empty())
    {
        auto queue_type = VK_QUEUE_GRAPHICS_BIT;
        switch (m_queue_type)
        {
        case Queue_Type::Graphics: queue_type = VK_QUEUE_GRAPHICS_BIT; break;
        case Queue_Type::Compute: queue_type = VK_QUEUE_COMPUTE_BIT; break;
        case Queue_Type::Copy: queue_type = VK_QUEUE_TRANSFER_BIT; break;
        case Queue_Type::Video_Decode: queue_type = VK_QUEUE_VIDEO_DECODE_BIT_KHR; break;
        case Queue_Type::Video_Encode: queue_type = VK_QUEUE_VIDEO_ENCODE_BIT_KHR; break;
        default: std::unreachable();
        }

        VkCommandPoolCreateInfo command_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = m_device->get_queue_family_index(queue_type)
        };
        vkCreateCommandPool(*m_device, &command_pool_create_info, nullptr, &cmd_alloc.pool);

        VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmd_alloc.pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        vkAllocateCommandBuffers(*m_device, &command_buffer_allocate_info, &cmd_alloc.cmd);
    }
    else
    {
        cmd_alloc = m_unused.back();
        m_unused.pop_back();
    }
    m_used.push_back(cmd_alloc);

    vkResetCommandPool(*m_device, cmd_alloc.pool, 0);

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };
    vkBeginCommandBuffer(cmd_alloc.cmd, &command_buffer_begin_info);

    if (m_queue_type == Queue_Type::Graphics || m_queue_type == Queue_Type::Compute)
    {
        VkPipelineLayout pipeline_layout = m_device->get_pipeline_layout();
        VkDescriptorSet descriptor_set = m_device->get_descriptor_set();
        vkCmdBindDescriptorSets(cmd_alloc.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
        vkCmdBindDescriptorSets(cmd_alloc.cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
    }
    return m_command_lists.emplace_back(std::make_unique<Vulkan_Command_List>(cmd_alloc.cmd, m_device, m_queue_type)).get();
}
}
