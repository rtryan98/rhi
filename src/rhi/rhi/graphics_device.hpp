#pragma once

#include "rhi/resource.hpp"
#include "rhi/command_list.hpp"

#include <memory>
#include <expected>
#include <span>

namespace rhi
{
class Command_List;
class Swapchain;
struct Swapchain_Win32_Create_Info;

enum class Graphics_API
{
#ifdef RHI_GRAPHICS_API_D3D12
    D3D12 = 0,
#endif
#ifdef RHI_GRAPHICS_API_VULKAN
    Vulkan = 1,
#endif
};

enum class Result
{
    Success,
    Wait_Timeout,
    Error_Wait_Failed,
    Error_Out_Of_Memory,
    Error_Invalid_Parameters,
    Error_Device_Lost,
    Error_No_Resource, // Create a view of a destroyed resource
    Error_Unknown
};

enum class Queue_Type
{
    Graphics,
    Compute,
    Copy,
    Video_Decode,
    Video_Encode
};

struct Fence
{
    virtual [[nodiscard]] Result get_status(uint64_t value) noexcept = 0;
    virtual Result wait_for_value(uint64_t value) noexcept = 0;
};

struct Submit_Fence_Info
{
    Fence* fence;
    uint64_t value;
};

struct Submit_Info
{
    Queue_Type queue_type;
    Swapchain* wait_swapchain; // May be nullptr.
    Swapchain* present_swapchain; // May be nullptr.
    std::span<Submit_Fence_Info> wait_infos;
    std::span<Command_List*> command_lists;
    std::span<Submit_Fence_Info> signal_infos;
};

struct Graphics_Device_Create_Info
{
    Graphics_API graphics_api;
    bool enable_validation;
    bool enable_gpu_validation;
    bool enable_locking;
    uint32_t reserved_bindless_resource_index_count;
};

class Graphics_Device
{
public:
    static [[nodiscard]] std::unique_ptr<Graphics_Device> create(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~Graphics_Device() noexcept = default;
    Graphics_Device(const Graphics_Device& other) = delete;
    Graphics_Device(Graphics_Device&& other) = delete;
    Graphics_Device& operator=(const Graphics_Device& other) = delete;
    Graphics_Device& operator=(Graphics_Device&& other) = delete;

    virtual Result wait_idle() noexcept = 0;
    virtual Result queue_wait_idle(Queue_Type queue, uint64_t timeout) noexcept = 0;

    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept = 0;

    virtual [[nodiscard]] std::unique_ptr<Swapchain> create_swapchain(
        const Swapchain_Win32_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::unique_ptr<Command_Pool> create_command_pool(
        const Command_Pool_Create_Info& create_info) noexcept = 0;

    virtual [[nodsicard]] std::expected<Fence*, Result> create_fence(uint64_t initial_value) noexcept = 0;
    virtual void destroy_fence(Fence* fence) noexcept = 0;

        virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(
        const Buffer_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept = 0;
    virtual [[nodiscard]] std::expected<Buffer_View*, Result> create_buffer_view(
        Buffer* buffer, const Buffer_View_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept = 0;
    virtual void destroy_buffer(Buffer* buffer) noexcept = 0;

    virtual [[nodiscard]] std::expected<Image*, Result> create_image(
        const Image_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept = 0;
    virtual [[nodiscard]] std::expected<Image_View*, Result> create_image_view(
        Image* image, const Image_View_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept = 0;
    virtual void destroy_image(Image* image) noexcept = 0;

    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(
        const Sampler_Create_Info& create_info) noexcept = 0;
    virtual void destroy_sampler(Sampler* sampler) noexcept = 0;

    virtual [[nodiscard]] std::expected<Shader_Blob*, Result> create_shader_blob(
        const Shader_Blob_Create_Info& create_info) noexcept = 0;
    virtual Result recreate_shader_blob(Shader_Blob* shader_blob, const Shader_Blob_Create_Info& create_info) noexcept = 0;
    virtual Result recreate_shader_blob_deserialize_memory(Shader_Blob* shader_blob, void* memory) noexcept = 0;
    virtual void destroy_shader_blob(Shader_Blob* shader_blob) noexcept = 0;

    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Graphics_Pipeline_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Compute_Pipeline_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept = 0;
    // virtual [[nodiscard]] std::expected<Pipeline*, Error> create_pipeline(const Ray_Tracing_Pipeline_Create_Info& create_info) noexcept = 0;
    virtual void destroy_pipeline(Pipeline* pipeline) noexcept = 0;

    virtual Result submit(const Submit_Info& submit_info) noexcept = 0;

    virtual void name_resource(Buffer* buffer, const char* name) noexcept = 0;
    virtual void name_resource(Image* image, const char* name) noexcept = 0;

protected:
    Graphics_Device() noexcept = default;
};
}
