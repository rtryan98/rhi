#pragma once

#include "rhi/resource.hpp"

#include <memory>
#include <expected>
#include <span>

namespace rhi
{
class Command_List;
class Swapchain;

enum class Graphics_API
{
    D3D12,
    Vulkan
};

enum class Result
{
    Success,
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
{};

struct Submit_Fence_Info
{
    Fence* fence;
    uint64_t value;
};

struct Submit_Info
{
    Queue_Type queue_type;
    Swapchain* swapchain; // May be nullptr.
    std::span<Submit_Fence_Info> wait_infos;
    std::span<Command_List*> command_lists;
    std::span< Submit_Fence_Info> signal_infos;
};

struct Graphics_Device_Create_Info
{
    bool enable_validation;
    bool enable_gpu_validation;
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

    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept = 0;

    virtual [[nodsicard]] std::expected<Fence*, Result> create_fence(uint64_t initial_value) noexcept = 0;
    virtual void destroy_fence(Fence* fence) noexcept = 0;

    virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(
        const Buffer_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::expected<Buffer_View*, Result> create_buffer_view(
        const Buffer_View_Create_Info& create_info) noexcept = 0;
    virtual void destroy_buffer(Buffer* buffer) noexcept = 0;

    virtual [[nodiscard]] std::expected<Image*, Result> create_image(
        const Image_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::expected<Image_View*, Result> create_image_view(
        const Image_View_Create_Info& create_info) noexcept = 0;
    virtual void destroy_image(Image* image) noexcept = 0;

    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(
        const Sampler_Create_Info& create_info) noexcept = 0;
    virtual void destroy_sampler(Sampler* sampler) noexcept = 0;

    virtual [[nodiscard]] std::expected<Shader_Blob*, Result> create_shader_blob(
        void* data, uint64_t size) noexcept = 0;
    virtual void destroy_shader_blob(Shader_Blob* shader_blob) noexcept = 0;

    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Graphics_Pipeline_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Compute_Pipeline_Create_Info& create_info) noexcept = 0;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept = 0;
    // virtual [[nodiscard]] std::expected<Pipeline*, Error> create_pipeline(const Ray_Tracing_Pipeline_Create_Info& create_info) noexcept = 0;
    virtual void destroy_pipeline(Pipeline* pipeline) noexcept = 0;

    virtual Result submit(const Submit_Info& submit_info) noexcept = 0;

protected:
    Graphics_Device() noexcept = default;
};
}
