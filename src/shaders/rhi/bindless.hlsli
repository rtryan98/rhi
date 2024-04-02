#ifndef RHI_BINDLESS_HLSLI
#define RHI_BINDLESS_HLSLI

#ifdef __spirv__
#define DECLARE_PUSH_CONSTANTS(TYPE, VARIABLE) [[vk::push_constant]] TYPE VARIABLE
[[vk::binding(0, 0)]] ByteAddressBuffer     g_vk_byte_address_buffers[];
[[vk::binding(0, 0)]] RWByteAddressBuffer   g_vk_rw_byte_address_buffers[];
[[vk::binding(2, 0)]] SamplerState          g_vk_samplers[];
#else
#define DECLARE_PUSH_CONSTANTS(TYPE, VARIABLE) ConstantBuffer< TYPE > VARIABLE : register(b0, space0)
#endif

namespace rhi
{
struct Handle
{
    uint index;

    uint srv()
    {
        return index;
    }

    uint uav()
    {
        #ifdef __spirv__
        return index;
        #else // D3D12
        return index + 1; // D3D12 requires separate descriptors for SRV and UAV
        #endif
    }
};

struct Array_Buffer
{
    Handle handle;

    template<typename T>
    T load(uint index)
    {
        #ifdef __spirv__
        return g_vk_byte_address_buffers[handle.srv()].Load<T>(index * sizeof(T));
        #else // D3D12
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(index * sizeof(T));
        #endif
    }

    template<typename T>
    T load_nuri(uint index)
    {
        #ifdef __spirv__
        return g_vk_byte_address_buffers[NonUniformResourceIndex(handle.srv())].Load<T>(index * sizeof(T));
        #else // D3D12
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(index * sizeof(T));
        #endif
    }
};

struct RW_Array_Buffer : Array_Buffer
{
    template<typename T>
    void store(uint index, T value)
    {
        #ifdef __spirv__
        g_vk_rw_byte_address_buffers[handle.srv()].Store(index * sizeof(T), value);
        #else
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[handle.uav()];
        buffer.Store(index * sizeof(T), value);
        #endif
    }
    
    template<typename T>
    void store_nuri(uint index, T value)
    {
        #ifdef __spirv__
        g_vk_rw_byte_address_buffers[NonUniformResourceIndex(handle.srv())].Store(index * sizeof(T), value);
        #else
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        buffer.Store(index * sizeof(T), value);
        #endif
    }

    template<typename T>
    uint atomic_add_uint(uint index, uint value_offset_in_struct, uint value)
    {
        #ifdef __spirv__
        uint original_value;
        g_vk_rw_byte_address_buffers[handle.srv()].InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
        return original_value;
        #else
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[handle.uav()];
        uint original_value;
        buffer.InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
        return original_value;
        #endif
    }

    template<typename T>
    uint atomic_add_uint_nuri(uint index, uint value_offset_in_struct, uint value)
    {
        #ifdef __spirv__
        uint original_value;
        g_vk_rw_byte_address_buffers[NonUniformResourceIndex(handle.srv())].InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
        return original_value;
        #else
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        uint original_value;
        buffer.InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
        return original_value;
        #endif
    }
};

struct Sampler
{
    Handle handle;

    SamplerState get()
    {
        #ifdef __spirv__
        return g_vk_samplers[handle.srv()];
        #else
        return SamplerDescriptorHeap[handle.srv()];
        #endif
    }

    SamplerState get_nuri()
    {
        #ifdef __spirv__
        return g_vk_samplers[NonUniformResourceIndex(handle.srv())];
        #else
        return SamplerDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        #endif
    }
};

}

#endif
