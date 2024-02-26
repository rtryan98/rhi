#pragma once

#include "rhi/resource.hpp"

#include <memory>
#include <expected>

namespace rhi
{
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
    Error_Unknown
};

struct Submit_Info
{

};

struct Graphics_Device_Create_Info
{
    bool enable_validation;
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

    constexpr static [[nodiscard]] Graphics_API get_active_graphics_api() noexcept;

    virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(const Buffer_Create_Info& create_info) noexcept = 0;
    virtual void destroy_buffer(Buffer* buffer) noexcept = 0;
    virtual [[nodiscard]] std::expected<Image*, Result> create_image(const Image_Create_Info& create_info) noexcept = 0;
    virtual void destroy_image(Image* image) noexcept = 0;
    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(const Sampler_Create_Info& create_info) noexcept = 0;
    virtual void destroy_sampler(Sampler* sampler) noexcept = 0;
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
