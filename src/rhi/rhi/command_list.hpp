#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "rhi/common/bitmask.hpp"

namespace rhi
{
constexpr static uint32_t INDIRECT_ARGUMENT_STRIDE = sizeof(uint32_t) * 8;
constexpr static uint32_t PUSH_CONSTANT_MAX_SIZE = 64;

struct Buffer;
struct Buffer_View;
struct Image;
struct Image_View;
struct Pipeline;
enum class Queue_Type;
enum class Graphics_API;

enum class Render_Pass_Attachment_Load_Op
{
    Discard,
    Load,
    Clear,
    No_Access
};

enum class Render_Pass_Attachment_Store_Op
{
    Discard,
    Store,
    No_Access
};

struct Clear_Value
{
    struct Color
    {
        float r, g, b, a;
    } color;
    struct Depth_Stencil
    {
        float depth;
        uint8_t stencil;
    } depth_stencil;
};

struct Render_Pass_Color_Attachment_Info
{
    Image_View* attachment;
    Render_Pass_Attachment_Load_Op load_op;
    Render_Pass_Attachment_Store_Op store_op;
    Clear_Value clear_value;
};

struct Render_Pass_Depth_Attachment_Info
{
    Image_View* attachment;
    Render_Pass_Attachment_Load_Op depth_load_op;
    Render_Pass_Attachment_Store_Op depth_store_op;
    Render_Pass_Attachment_Load_Op stencil_load_op;
    Render_Pass_Attachment_Store_Op stencil_store_op;
    Clear_Value clear_value;
};

struct Render_Pass_Begin_Info
{
    std::span<Render_Pass_Color_Attachment_Info> color_attachments;
    Render_Pass_Depth_Attachment_Info depth_stencil_attachment;
};

enum class Pipeline_Bind_Point
{
    Graphics,
    Compute,
    Ray_Tracing
};

enum class Queue_Type_Ownership_Transfer_Mode
{
    None,
    Release,
    Acquire
};

enum class Barrier_Image_Layout : uint64_t
{
    Undefined,
    Present,
    General,
    Color_Attachment,
    Depth_Stencil_Read_Only,
    Depth_Stencil_Write,
    Shader_Read_Only,
    Unordered_Access,
    Copy_Src,
    Copy_Dst,
    // Resolve_Src,
    // Resolve_Dst,
    Shading_Rate_Attachment,
    Video_Read,
    Video_Write
    // TODO: add clear layout due to mismatch in vulkan and d3d12?
};

enum class Barrier_Pipeline_Stage : uint64_t
{
    // Use Vulkan flags, translate to D3D12 flags (more conservative)
    None                            = 0x0000000000ull,
    Draw_Indirect                   = 0x0000000002ull,
    Vertex_Input                    = 0x0000000004ull,
    Vertex_Shader                   = 0x0000000008ull,
    Hull_Shader                     = 0x0000000010ull,
    Domain_Shader                   = 0x0000000020ull,
    Geometry_Shader                 = 0x0000000040ull,
    Pixel_Shader                    = 0x0000000080ull,
    Early_Fragment_Tests            = 0x0000000100ull,
    Late_Fragment_Tests             = 0x0000000200ull,
    Color_Attachment_Output         = 0x0000000400ull,
    Compute_Shader                  = 0x0000000800ull,
    All_Transfer                    = 0x0000001000ull,
    Host                            = 0x0000004000ull,
    All_Graphics                    = 0x0000008000ull,
    All_Commands                    = 0x0000010000ull,
    Copy                            = 0x0100000000ull,
    Resolve                         = 0x0200000000ull,
    Blit                            = 0x0400000000ull,
    Clear                           = 0x0800000000ull,
    Index_Input                     = 0x1000000000ull,
    Vertex_Attribute_Input          = 0x2000000000ull,
    Pre_Rasterization_Stages        = 0x4000000000ull,
    Video_Decode                    = 0x0004000000ull,
    Video_Encode                    = 0x0008000000ull,
    Acceleration_Structure_Build    = 0x0002000000ull,
    Ray_Tracing_Shader              = 0x0000200000ull,
    Amplification_Shader            = 0x0000080000ull,
    Mesh_Shader                     = 0x0000100000ull,
    Acceleration_Structure_Copy     = 0x0010000000ull,
};

enum class Barrier_Access : uint64_t
{
    // Use Vulkan flags, translate to D3D12 flags (more conservative)
    None                            = 0x0000000000ull,
    Indirect_Command_Read           = 0x0000000001ull,
    Index_Read                      = 0x0000000002ull,
    Vertex_Attribute_Read           = 0x0000000004ull,
    Constant_Buffer_View            = 0x0000000008ull,
    Shader_Read                     = 0x0000000020ull,
    Shader_Write                    = 0x0000000040ull,
    Color_Attachment_Read           = 0x0000000080ull,
    Color_Attachment_Write          = 0x0000000100ull,
    Depth_Stencil_Attachment_Read   = 0x0000000200ull,
    Depth_Stencil_Attachment_Write  = 0x0000000400ull,
    Transfer_Read                   = 0x0000000800ull,
    Transfer_Write                  = 0x0000001000ull,
    Shader_Sampled_Read             = 0x0100000000ull,
    Unordered_Access_Read           = 0x0200000000ull,
    Unordered_Access_Write          = 0x0400000000ull,
    Video_Decode_Read               = 0x0800000000ull,
    Video_Decode_Write              = 0x1000000000ull,
    Video_Encode_Read               = 0x2000000000ull,
    Video_Encode_Write              = 0x4000000000ull,
    Shading_Rate_Attachment         = 0x0000800000ull,
    Acceleration_Structure_Read     = 0x0000200000ull,
    Acceleration_Structure_Write    = 0x0000400000ull,
};

enum class Index_Type
{
    U16,
    U32
};

struct Buffer_Barrier_Info
{
    Barrier_Pipeline_Stage stage_before;
    Barrier_Pipeline_Stage stage_after;
    Barrier_Access access_before;
    Barrier_Access access_after;
    Buffer* buffer;
};

struct Image_Barrier_Subresource_Range
{
    uint32_t first_mip_level;
    uint32_t mip_count;
    uint32_t first_array_index;
    uint32_t array_size;
    uint32_t first_plane;
    uint32_t plane_count;
};

struct Image_Barrier_Info
{
    Barrier_Pipeline_Stage stage_before;
    Barrier_Pipeline_Stage stage_after;
    Barrier_Access access_before;
    Barrier_Access access_after;
    Barrier_Image_Layout layout_before;
    Barrier_Image_Layout layout_after;
    // Only relevant if `queue_type_ownership_transfer_mode` is not `None`.
    Queue_Type queue_type_ownership_transfer_target_queue;
    Queue_Type_Ownership_Transfer_Mode queue_type_ownership_transfer_mode;
    Image* image;
    Image_Barrier_Subresource_Range subresource_range;
    bool discard; // D3D12 only - only usable if `layout_before` is `Undefined`.
};

struct Memory_Barrier_Info
{
    Barrier_Pipeline_Stage stage_before;
    Barrier_Pipeline_Stage stage_after;
    Barrier_Access access_before;
    Barrier_Access access_after;
};

struct Barrier_Info
{
    std::span<Buffer_Barrier_Info> buffer_barriers;
    std::span<Image_Barrier_Info> image_barriers;
    std::span<Memory_Barrier_Info> memory_barriers;
};

struct Offset_3D
{
    int32_t x;
    int32_t y;
    int32_t z;
};

struct Extent_3D
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

class Command_List
{
public:
    // Meta commands
    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept = 0;

    // Barrier commands
    virtual void barrier(const Barrier_Info& barrier_info) noexcept = 0;

    // Compute commands
    virtual void dispatch(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept = 0;
    virtual void dispatch_indirect(Buffer* buffer, uint64_t offset) noexcept = 0;
    virtual void dispatch_indirect_d3d12(Buffer* buffer, uint64_t offset, uint32_t count) noexcept = 0;
    virtual void dispatch_indirect_count_d3d12(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept = 0;

    // Copy commands
    virtual void copy_buffer(Buffer* src, uint64_t src_offset, Buffer* dst, uint64_t dst_offset, uint64_t size) noexcept = 0;
    virtual void copy_buffer_to_image(
        Buffer* src, uint64_t src_offset,
        Image* dst, const Offset_3D& dst_offset, const Extent_3D& dst_extent,
        uint32_t dst_mip_level, uint32_t dst_array_index) noexcept = 0;
    virtual void copy_image(
        Image* src, const Offset_3D& src_offset, uint32_t src_mip_level, uint32_t src_array_index,
        Image* dst, const Offset_3D& dst_offset, uint32_t dst_mip_level, uint32_t dst_array_index,
        const Extent_3D& extent) noexcept = 0; // No src_offset because D3D12 does not support it
    virtual void copy_image_to_buffer(
        Image* src, const Offset_3D& src_offset, const Extent_3D& src_extent,
        uint32_t src_mip_level, uint32_t src_array_index,
        Buffer* dst, uint64_t dst_offset) noexcept = 0;
    virtual void fill_buffer(Buffer_View* dst, uint32_t value) noexcept = 0;

    // Debug commands
    virtual void begin_debug_region(const char* name, float r, float g, float b) noexcept = 0;
    virtual void add_debug_marker(const char* name, float r, float g, float b)  noexcept = 0;
    virtual void end_debug_region() noexcept = 0;

    // Draw commands
    virtual void clear_color_attachment(Image_View* image, float r, float g, float b, float a) noexcept = 0;
    virtual void clear_depth_stencil_attachment(Image_View* image, float d, uint8_t s) noexcept = 0;
    virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) noexcept = 0;
    virtual void draw_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept = 0;
    virtual void draw_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept = 0;
    virtual void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t index_offset, uint32_t vertex_offset, uint32_t instance_offset) noexcept = 0;
    virtual void draw_indexed_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept = 0;
    virtual void draw_indexed_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept = 0;
    virtual void draw_mesh_tasks(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) noexcept = 0;
    virtual void draw_mesh_tasks_indirect(Buffer* buffer, uint64_t offset, uint32_t count) noexcept = 0;
    virtual void draw_mesh_tasks_indirect_count(Buffer* buffer, uint64_t offset, uint32_t max_draw_count, Buffer* count_buffer, uint64_t count_offset) noexcept = 0;

    // State commands
    virtual void begin_render_pass(const Render_Pass_Begin_Info& begin_info) noexcept = 0;
    virtual void end_render_pass() noexcept = 0;
    virtual void set_pipeline(Pipeline* pipeline) noexcept = 0;
    virtual void set_depth_bounds(float min, float max) noexcept = 0;
    virtual void set_index_buffer(Buffer* buffer, Index_Type index_type) noexcept = 0;
    virtual void set_index_buffer(Buffer* buffer, Index_Type index_type, uint64_t offset, uint64_t size) noexcept = 0;
    template<typename T>
    void set_push_constants(const T& data, Pipeline_Bind_Point bind_point) noexcept
    {
        static_assert(sizeof(T) <= PUSH_CONSTANT_MAX_SIZE);
        set_push_constants(&data, uint32_t(sizeof(data)), bind_point);
    }
    virtual void set_push_constants(const void* data, uint32_t size, Pipeline_Bind_Point bind_point) noexcept = 0;
    virtual void set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height) noexcept = 0;
    virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) noexcept = 0;

protected:
    Queue_Type m_queue_type;
};

struct Command_Pool_Create_Info
{
    Queue_Type queue_type;
};

class Command_Pool
{
public:
    virtual ~Command_Pool() noexcept = default;

    virtual void reset() noexcept = 0;
    virtual Command_List* acquire_command_list() noexcept = 0;

protected:
    Command_Pool() noexcept = default;
};

}

template<>
constexpr static bool RHI_ENABLE_BIT_OPERATORS<rhi::Barrier_Pipeline_Stage> = true;

template<>
constexpr static bool RHI_ENABLE_BIT_OPERATORS<rhi::Barrier_Access> = true;
