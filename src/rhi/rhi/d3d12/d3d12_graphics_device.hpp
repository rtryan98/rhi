#pragma once

#include "rhi/graphics_device.hpp"
#include "rhi/d3d12/d3d12_resource.hpp"

#include <core/d3d12/d3d12_device.hpp>
#include <mutex>
#include <plf_colony.h>

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

struct D3D12_Features
{
    // D3D12_FEATURE_DATA_ARCHITECTURE architecture; // Superseeded by architecture1
    D3D12_FEATURE_DATA_FEATURE_LEVELS levels;
    D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpu_virtual_address_support;
    D3D12_FEATURE_DATA_SHADER_MODEL shader_model;
    D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_SUPPORT protected_resource_session_support;
    D3D12_FEATURE_DATA_ROOT_SIGNATURE root_signature;
    D3D12_FEATURE_DATA_ARCHITECTURE1 architecture1;
    D3D12_FEATURE_DATA_SHADER_CACHE shader_cache;
    D3D12_FEATURE_DATA_EXISTING_HEAPS existing_heaps;
    D3D12_FEATURE_DATA_SERIALIZATION serialization;
    D3D12_FEATURE_DATA_DISPLAYABLE displayable;
    D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_TYPE_COUNT protected_resource_session_type_count;
    D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_TYPES protected_resource_session_types;
    D3D12_FEATURE_DATA_D3D12_OPTIONS   options;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1  options1;
    D3D12_FEATURE_DATA_D3D12_OPTIONS2  options2;
    D3D12_FEATURE_DATA_D3D12_OPTIONS3  options3;
    D3D12_FEATURE_DATA_D3D12_OPTIONS4  options4;
    D3D12_FEATURE_DATA_D3D12_OPTIONS5  options5;
    D3D12_FEATURE_DATA_D3D12_OPTIONS6  options6;
    D3D12_FEATURE_DATA_D3D12_OPTIONS7  options7;
    D3D12_FEATURE_DATA_D3D12_OPTIONS8  options8;
    D3D12_FEATURE_DATA_D3D12_OPTIONS9  options9;
    D3D12_FEATURE_DATA_D3D12_OPTIONS10 options10;
    D3D12_FEATURE_DATA_D3D12_OPTIONS11 options11;
    D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12;
    D3D12_FEATURE_DATA_D3D12_OPTIONS13 options13;
    D3D12_FEATURE_DATA_D3D12_OPTIONS14 options14;
    D3D12_FEATURE_DATA_D3D12_OPTIONS15 options15;
    D3D12_FEATURE_DATA_D3D12_OPTIONS16 options16;
    D3D12_FEATURE_DATA_D3D12_OPTIONS17 options17;
    D3D12_FEATURE_DATA_D3D12_OPTIONS18 options18;
    D3D12_FEATURE_DATA_D3D12_OPTIONS19 options19;
    D3D12_FEATURE_DATA_D3D12_OPTIONS20 options20;
};

struct D3D12_Context_Create_Info
{
    bool enable_validation;
    bool enable_gpu_validation;
    bool disable_tdr;
    D3D_FEATURE_LEVEL feature_level;
    uint32_t resource_descriptor_heap_size;
    uint32_t sampler_descriptor_heap_size;
    uint32_t rtv_descriptor_heap_size;
    uint32_t dsv_descriptor_heap_size;
    uint32_t push_constant_size;
    std::span<D3D12_STATIC_SAMPLER_DESC> static_samplers;
};

struct D3D12_Context
{
    IDXGIFactory7* factory;
    IDXGIAdapter4* adapter;
    ID3D12Device10* device;
    ID3D12InfoQueue1* info_queue;
    ID3D12CommandQueue* direct_queue;
    ID3D12CommandQueue* compute_queue;
    ID3D12CommandQueue* copy_queue;
    D3D12_Features features;
    ID3D12DescriptorHeap* resource_descriptor_heap;
    ID3D12DescriptorHeap* sampler_descriptor_heap;
    ID3D12DescriptorHeap* rtv_descriptor_heap;
    ID3D12DescriptorHeap* dsv_descriptor_heap;
    ID3D12RootSignature* bindless_root_signature;
};

struct D3D12_Fence : public Fence
{
    ID3D12Fence1* fence;

    virtual [[nodiscard]] Result get_status(uint64_t value) noexcept override;
    virtual Result wait_for_value(uint64_t value) noexcept override;
};

class D3D12_Graphics_Device final : public Graphics_Device
{
public:
    D3D12_Graphics_Device(const Graphics_Device_Create_Info& create_info) noexcept;
    virtual ~D3D12_Graphics_Device() noexcept override;

    virtual Result wait_idle() noexcept override;
    virtual Result queue_wait_idle(Queue_Type queue, uint64_t timeout) noexcept override;

    virtual [[nodiscard]] Graphics_API get_graphics_api() const noexcept override;

    virtual [[nodiscard]] std::unique_ptr<Swapchain> create_swapchain(
        const Swapchain_Win32_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::unique_ptr<Command_Pool> create_command_pool(
        const Command_Pool_Create_Info& create_info) noexcept override;

    virtual [[nodsicard]] std::expected<Fence*, Result> create_fence(uint64_t initial_value) noexcept override;
    virtual void destroy_fence(Fence* fence) noexcept override;

        virtual [[nodiscard]] std::expected<Buffer*, Result> create_buffer(
        const Buffer_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual [[nodiscard]] std::expected<Buffer_View*, Result> create_buffer_view(
        Buffer* buffer, const Buffer_View_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual void destroy_buffer(Buffer* buffer) noexcept override;

        virtual [[nodiscard]] std::expected<Image*, Result> create_image(
        const Image_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual [[nodiscard]] std::expected<Image_View*, Result> create_image_view(
        Image* image, const Image_View_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual void destroy_image(Image* image) noexcept override;

    virtual [[nodiscard]] std::expected<Sampler*, Result> create_sampler(
        const Sampler_Create_Info& create_info, uint32_t index = NO_RESOURCE_INDEX) noexcept override;
    virtual void destroy_sampler(Sampler* sampler) noexcept override;

    virtual [[nodiscard]] std::expected<Acceleration_Structure*, Result> create_acceleration_structure(
        const Acceleration_Structure_Create_Info& create_info) noexcept override;
    virtual void destroy_acceleration_structure(Acceleration_Structure* acceleration_structure) noexcept override;

    virtual [[nodiscard]] std::expected<Shader_Blob*, Result> create_shader_blob(
        const Shader_Blob_Create_Info& create_info) noexcept override;
    virtual Result recreate_shader_blob(Shader_Blob* shader_blob, const Shader_Blob_Create_Info& create_info) noexcept override;
    virtual Result recreate_shader_blob_deserialize_memory(Shader_Blob* shader_blob, void* memory) noexcept override;
    virtual void destroy_shader_blob(Shader_Blob* shader_blob) noexcept override;

    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Graphics_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Compute_Pipeline_Create_Info& create_info) noexcept override;
    virtual [[nodiscard]] std::expected<Pipeline*, Result> create_pipeline(
        const Mesh_Shading_Pipeline_Create_Info& create_info) noexcept override;
    virtual void destroy_pipeline(Pipeline* pipeline) noexcept override;

    virtual Result submit(const Submit_Info& submit_info) noexcept override;

    virtual void name_resource(Buffer* buffer, const char* name) noexcept override;
    virtual void name_resource(Image* image, const char* name) noexcept override;

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_descriptor_handle(
        uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_descriptor_handle(
        uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept;

    [[nodiscard]] uint32_t get_uav_from_bindless_index(uint32_t bindless_index) const noexcept;
    [[nodiscard]] const Indirect_Signatures& get_indirect_signatures() const noexcept;

    [[nodiscard]] D3D12_Context* get_context() noexcept;

    [[nodiscard]] uint32_t create_descriptor_index_blocking(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
    void release_descriptor_index_blocking(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;

    [[nodiscard]] D3D12_Image* acquire_custom_allocated_image() noexcept;
    void release_custom_allocated_image(D3D12_Image* image) noexcept;

private:
    void create_initial_buffer_descriptors(D3D12_Buffer* buffer, bool create_srv, bool create_uav) noexcept;
    void create_initial_image_descriptors(D3D12_Image* image) noexcept;

    void create_srv_and_uav(
        ID3D12Resource* resource,
        uint32_t bindless_index,
        const D3D12_SHADER_RESOURCE_VIEW_DESC* srv_desc,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav_desc) noexcept;

    void create_srv_uav_rtv_dsv(
        ID3D12Resource* resource,
        uint32_t bindless_index,
        const D3D12_SHADER_RESOURCE_VIEW_DESC* srv_desc,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav_desc,
        uint32_t rtv_dsv_index,
        const D3D12_RENDER_TARGET_VIEW_DESC* rtv_desc,
        const D3D12_DEPTH_STENCIL_VIEW_DESC* dsv_desc) noexcept;

    // Only use inside resource creation and destruction. Not guarded by mutex.
    [[nodiscard]] uint32_t create_descriptor_index(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
    void release_descriptor_index(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;

    [[nodiscard]] Descriptor_Increment_Sizes acquire_descriptor_increment_sizes() noexcept;
    [[nodiscard]] Indirect_Signatures create_execute_indirect_signatures() noexcept;

    [[nodiscard]] Acceleration_Structure_Build_Sizes get_acceleration_structure_build_sizes(
        const Acceleration_Structure_Build_Geometry_Info& build_info) noexcept override;

private:
    constexpr static std::size_t ARRAY_VECTOR_SIZE = 512;

    D3D12_Context m_context;
    D3D12MA::Allocator* m_allocator;

    Descriptor_Increment_Sizes m_descriptor_increment_sizes;
    Indirect_Signatures m_indirect_signatures;

    bool m_use_mutex;
    std::mutex m_resource_mutex;
    std::mutex m_direct_queue_mutex;
    std::mutex m_compute_queue_mutex;
    std::mutex m_copy_queue_mutex;

    plf::colony<D3D12_Fence> m_fences;
    plf::colony<D3D12_Buffer> m_buffers;
    plf::colony<D3D12_Buffer_View> m_buffer_views;
    plf::colony<D3D12_Image> m_images;
    plf::colony<D3D12_Image_View> m_image_views;
    plf::colony<D3D12_Sampler> m_samplers;
    plf::colony<Acceleration_Structure> m_acceleration_structures; // D3D12 doesn't need a special type for AS
    plf::colony<Shader_Blob> m_shader_blobs;
    plf::colony<D3D12_Pipeline> m_pipelines;

    uint32_t m_max_dynamic_resource_index;
    uint32_t m_max_dynamic_sampler_index;
    std::vector<uint32_t> m_resource_descriptor_indices;
    std::vector<uint32_t> m_sampler_descriptor_indices;
    std::vector<uint32_t> m_rtv_descriptor_indices;
    std::vector<uint32_t> m_dsv_descriptor_indices;
};
}
