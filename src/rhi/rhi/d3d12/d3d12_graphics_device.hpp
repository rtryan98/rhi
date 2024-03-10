#pragma once

#include "rhi/graphics_device.hpp"
#include "rhi/common/array_vector.hpp"
#include "rhi/d3d12/d3d12_resource.hpp"

#include <core/d3d12/d3d12_device.hpp>
#include <mutex>

namespace D3D12MA
{
class Allocator;
}

namespace rhi::d3d12
{
struct Descriptor_Increment_Sizes
{
    uint64_t resource;
    uint64_t sampler;
    uint64_t rtv;
    uint64_t dsv;
};

struct Indirect_Signatures
{
    ID3D12CommandSignature* draw_indirect;
    ID3D12CommandSignature* draw_indexed_indirect;
    ID3D12CommandSignature* draw_mesh_tasks_indirect;
    ID3D12CommandSignature* dispatch_indirect;
};

struct D3D12_Fence : public Fence
{
    ID3D12Fence1* fence;
};

class D3D12_Graphics_Device final : public Graphics_Device
{
public:
    D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~D3D12_Graphics_Device() noexcept override;

    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept override;

    virtual [[nodsicard]] std::expected<Fence*, Result> create_fence(uint64_t initial_value) noexcept override;
    virtual void destroy_fence(Fence* fence) noexcept override;

    virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(
        const Buffer_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Buffer_View*, Result> create_buffer_view(
        Buffer* buffer, const Buffer_View_Create_Info& create_info) noexcept override;
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
    [[nodiscard]] const Indirect_Signatures& get_indirect_signatures() const noexcept;

private:
    void create_initial_buffer_descriptors(D3D12_Buffer* buffer) noexcept;
    void create_initial_image_descriptors(D3D12_Image* image) noexcept;

    void create_srv_and_uav(
        ID3D12Resource* resource,
        uint32_t bindless_index,
        const D3D12_SHADER_RESOURCE_VIEW_DESC* srv_desc,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav_desc) noexcept;

    // Only use inside resource creation and destruction. Not guarded by mutex.
    [[nodiscard]] uint32_t create_bindless_index(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
    void release_bindless_index(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;

    [[nodiscard]] Descriptor_Increment_Sizes acquire_descriptor_increment_sizes() noexcept;
    [[nodiscard]] Indirect_Signatures create_execute_indirect_signatures() noexcept;

private:
    constexpr static std::size_t ARRAY_VECTOR_SIZE = 512;

    core::d3d12::D3D12_Context m_context;
    D3D12MA::Allocator* m_allocator;

    Descriptor_Increment_Sizes m_descriptor_increment_sizes;
    Indirect_Signatures m_indirect_signatures;

    bool m_use_mutex;
    std::mutex m_resource_mutex;
    std::mutex m_direct_queue_mutex;
    std::mutex m_compute_queue_mutex;
    std::mutex m_copy_queue_mutex;

    Array_Vector<D3D12_Fence, ARRAY_VECTOR_SIZE> m_fences;
    Array_Vector<D3D12_Buffer, ARRAY_VECTOR_SIZE> m_buffers;
    Array_Vector<Buffer_View, ARRAY_VECTOR_SIZE> m_buffer_views;
    Array_Vector<D3D12_Image, ARRAY_VECTOR_SIZE> m_images;
    Array_Vector<Image_View, ARRAY_VECTOR_SIZE> m_image_views;
    Array_Vector<D3D12_Sampler, ARRAY_VECTOR_SIZE> m_samplers;
    Array_Vector<Shader_Blob, ARRAY_VECTOR_SIZE> m_shader_blobs;
    Array_Vector<D3D12_Pipeline, ARRAY_VECTOR_SIZE> m_pipelines;
};
}
