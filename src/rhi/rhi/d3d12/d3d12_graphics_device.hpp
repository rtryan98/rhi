#pragma once
#ifdef RHI_GRAPHICS_API_D3D12

#include "rhi/graphics_device.hpp"
#include "rhi/common/array_vector.hpp"
#include "rhi/d3d12/d3d12_resource.hpp"

#include <core/d3d12/d3d12_device.hpp>

namespace D3D12MA
{
class Allocator;
}

namespace rhi::d3d12
{
class D3D12_Graphics_Device final : public Graphics_Device
{
public:
    D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~D3D12_Graphics_Device() noexcept override;

    virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(
        const Buffer_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Buffer_View*, Result> create_buffer_view(
        const Buffer_View_Create_Info& create_info) noexcept override;
    virtual void destroy_buffer(Buffer* buffer) noexcept override;

    virtual [[nodiscard]] std::expected<Image*, Result> create_image(
        const Image_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Image_View*, Result> create_image_view(
        const Image_View_Create_Info& create_info) noexcept override;
    virtual void destroy_image(Image* image) noexcept override;

    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(
        const Sampler_Create_Info& create_info) noexcept override;
    virtual void destroy_sampler(Sampler* sampler) noexcept override;

    virtual [[nodiscard]] std::expected<Shader_Blob*, Result> create_shader_blob(
        void* data, uint64_t size) noexcept override;
    virtual void destroy_shader_blob(Shader_Blob* shader_blob) noexcept override;

    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Graphics_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Compute_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept override;
    virtual void destroy_pipeline(Pipeline* pipeline) noexcept override;

    virtual Result submit(const Submit_Info& submit_info) noexcept override;

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_descriptor_handle(
        uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_descriptor_handle(
        uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept;

    [[nodiscard]] uint32_t get_uav_from_bindless_index(uint32_t bindless_index) const noexcept;

private:
    void release_bindless_index(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
    [[nodiscard]] uint32_t create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;

private:
    constexpr static std::size_t ARRAY_VECTOR_SIZE = 512;

    core::d3d12::D3D12_Context m_context;
    D3D12MA::Allocator* m_allocator;

    uint64_t m_resource_descriptor_increment_size;
    uint64_t m_sampler_descriptor_increment_size;
    uint64_t m_rtv_descriptor_increment_size;
    uint64_t m_dsv_descriptor_increment_size;

    Array_Vector<D3D12_Buffer, ARRAY_VECTOR_SIZE> m_buffers;
    Array_Vector<D3D12_Image, ARRAY_VECTOR_SIZE> m_images;
    Array_Vector<D3D12_Sampler, ARRAY_VECTOR_SIZE> m_samplers;
    Array_Vector<Shader_Blob, ARRAY_VECTOR_SIZE> m_shader_blobs;
    Array_Vector<D3D12_Pipeline, ARRAY_VECTOR_SIZE> m_pipelines;
};
}

#endif // RHI_GRAPHICS_API_D3D12
