#pragma once
#ifdef RHI_GRAPHICS_API_D3D12

#include "rhi/graphics_device.hpp"

#include <core/d3d12/d3d12_device.hpp>

namespace rhi::d3d12
{
class D3D12_Graphics_Device final : public Graphics_Device
{
public:
    D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~D3D12_Graphics_Device() noexcept override;

    virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(const Buffer_Create_Info& create_info) noexcept override;
    virtual void destroy_buffer(Buffer* buffer) noexcept override;
    virtual [[nodiscard]] std::expected<Image*, Result> create_image(const Image_Create_Info& create_info) noexcept override;
    virtual void destroy_image(Image* image) noexcept override;
    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(const Sampler_Create_Info& create_info) noexcept override;
    virtual void destroy_sampler(Sampler* sampler) noexcept override;
    virtual [[nodiscard]] std::expected<Shader_Blob*, Result> create_shader_blob(void* data, uint64_t size) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Graphics_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Compute_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept override;
    virtual void destroy_pipeline(Pipeline* pipeline) noexcept override;

    virtual Result submit(const Submit_Info& submit_info) noexcept override;

private:
    core::d3d12::D3D12_Context m_context;
};
}

#endif // RHI_GRAPHICS_API_D3D12
