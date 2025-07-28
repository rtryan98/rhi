#ifndef RHI_BINDLESS_HLSLI
#define RHI_BINDLESS_HLSLI

#ifdef __spirv__
#define DECLARE_PUSH_CONSTANTS(TYPE, VARIABLE) [[vk::push_constant]] TYPE VARIABLE
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

struct Raw_Buffer
{
    Handle handle;

    template<typename T>
    T load()
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(0);
    }

    template<typename T>
    T load_nuri()
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(0);
    }
};

struct RW_Raw_Buffer
{
    Handle handle;

    template<typename T>
    T load()
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(0);
    }

    template<typename T>
    T load_nuri()
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(0);
    }
    
    template<typename T>
    void store(T value)
    {
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[handle.uav()];
        buffer.Store(0, value);
    }
    
    template<typename T>
    void store_nuri(T value)
    {
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        buffer.Store(0, value);
    }
};

struct Array_Buffer
{
    Handle handle;

    template<typename T>
    T load(uint index)
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(index * sizeof(T));
    }

    template<typename T>
    T load_nuri(uint index)
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(index * sizeof(T));
    }
};

struct RW_Array_Buffer : Array_Buffer
{
    template<typename T>
    T load(uint index)
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(index * sizeof(T));
    }

    template<typename T>
    T load_nuri(uint index)
    {
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(index * sizeof(T));
    }

    template<typename T>
    void store(uint index, T value)
    {
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[handle.uav()];
        buffer.Store(index * sizeof(T), value);
    }
    
    template<typename T>
    void store_nuri(uint index, T value)
    {
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        buffer.Store(index * sizeof(T), value);
    }

    template<typename T>
    uint atomic_add_uint(uint index, uint value_offset_in_struct, uint value)
    {
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[handle.uav()];
        uint original_value;
        buffer.InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
        return original_value;
    }

    template<typename T>
    uint atomic_add_uint_nuri(uint index, uint value_offset_in_struct, uint value)
    {
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        uint original_value;
        buffer.InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
        return original_value;
    }
};

struct Sampler
{
    Handle handle;

    SamplerState get()
    {
        return SamplerDescriptorHeap[handle.srv()];
    }

    SamplerState get_nuri()
    {
        return SamplerDescriptorHeap[NonUniformResourceIndex(handle.srv())];
    }

    SamplerComparisonState get_cmp()
    {
        return SamplerDescriptorHeap[handle.srv()];
    }

    SamplerComparisonState get_cmd_nuri()
    {
        return SamplerDescriptorHeap[NonUniformResourceIndex(handle.srv())];
    }
};

struct Texture
{
    Handle handle;

    template<typename T>
    T load_1d(uint pos)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint2(pos, 0));
    }

    template<typename T>
    T load_1d_array(uint2 pos)
    {
        Texture1DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos.x, pos.y, 0));
    }

    template<typename T>
    T load_2d(uint2 pos)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos, 0));
    }

    template<typename T>
    T load_2d_array(uint3 pos)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos.xyz, 0));
    }

    template<typename T>
    T load_3d(uint3 pos)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos, 0));
    }

    template<typename T>
    T sample_1d(SamplerState s, float u)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, u);
    }

    template<typename T>
    T sample_2d(SamplerState s, float2 uv)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uv);
    }

    template<typename T>
    T sample_2d_array(SamplerState s, float2 uv, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, float3(uv, index));
    }

    template<typename T>
    T sample_3d(SamplerState s, float3 uvw)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uvw);
    }

    template<typename T>
    T sample_level_1d(SamplerState s, float u, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, u, mip);
    }

    template<typename T>
    T sample_level_2d(SamplerState s, float2 uv, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uv, mip);
    }

    template<typename T>
    T sample_level_2d_array(SamplerState s, float2 uv, float mip, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, float3(uv, index), mip);
    }

    template<typename T>
    T sample_level_3d(SamplerState s, float3 uvw, float mip)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uvw, mip);
    }

    template<typename T>
    T load_1d_uniform(uint pos)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint2(pos, 0));
    }

    template<typename T>
    T load_1d_array_uniform(uint2 pos)
    {
        Texture1DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos.x, pos.y, 0));
    }

    template<typename T>
    T load_2d_uniform(uint2 pos)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos, 0));
    }

    template<typename T>
    T load_2d_array_uniform(uint3 pos)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos.xyz, 0));
    }

    template<typename T>
    T load_3d_uniform(uint3 pos)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos, 0));
    }

    template<typename T>
    T sample_1d_uniform(SamplerState s, float u)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, u);
    }

    template<typename T>
    T sample_2d_uniform(SamplerState s, float2 uv)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uv);
    }

    template<typename T>
    T sample_cmp_2d_uniform(SamplerComparisonState s, float2 uv, float cmp)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleCmp(s, uv, cmp);
    }

    template<typename T>
    T sample_2d_array_uniform(SamplerComparisonState s, float2 uv, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, float3(uv, index));
    }

    template<typename T>
    T sample_3d_uniform(SamplerState s, float3 uvw)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uvw);
    }

    template<typename T>
    T sample_level_1d_uniform(SamplerState s, float u, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, u, mip);
    }

    template<typename T>
    T sample_level_2d_uniform(SamplerState s, float2 uv, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uv, mip);
    }

    template<typename T>
    T sample_level_2d_array_uniform(SamplerState s, float2 uv, float mip, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, float3(uv, index), mip);
    }

    template<typename T>
    T sample_level_3d_uniform(SamplerState s, float3 uvw, float mip)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uvw, mip);
    }
};

struct RW_Texture
{
    Handle handle;

    template<typename T>
    T load_1d(uint pos)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint2(pos, 0));
    }

    template<typename T>
    T load_1d_array(uint2 pos)
    {
        Texture1DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos.x, pos.y, 0));
    }

    template<typename T>
    T load_2d(uint2 pos)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos, 0));
    }

    template<typename T>
    T load_2d_array(uint3 pos)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos.xyz, 0));
    }

    template<typename T>
    T load_3d(uint3 pos)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos, 0));
    }

    template<typename T>
    void store_1d(uint pos, T value)
    {
        RWTexture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
    }

    template<typename T>
    void store_1d_array(uint2 pos, T value)
    {
        RWTexture1DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
    }

    template<typename T>
    void store_2d(uint2 pos, T value)
    {
        RWTexture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
    }

    template<typename T>
    void store_2d_array(uint3 pos, T value)
    {
        RWTexture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
    }

    template<typename T>
    void store_3d(uint3 pos, T value)
    {
        RWTexture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
    }
    

    template<typename T>
    T sample_1d(SamplerState s, float u)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, u);
    }

    template<typename T>
    T sample_2d(SamplerState s, float2 uv)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uv);
    }

    template<typename T>
    T sample_2d_array(SamplerState s, float2 uv, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, float3(uv, index));
    }

    template<typename T>
    T sample_3d(SamplerState s, float3 uvw)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uvw);
    }

    template<typename T>
    T sample_level_1d(SamplerState s, float u, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, u, mip);
    }

    template<typename T>
    T sample_level_2d(SamplerState s, float2 uv, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uv, mip);
    }

    template<typename T>
    T sample_level_2d_array(SamplerState s, float2 uv, float mip, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, float3(uv, index), mip);
    }

    template<typename T>
    T sample_level_3d(SamplerState s, float3 uvw, float mip)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uvw, mip);
    }
    

    template<typename T>
    T load_1d_uniform(uint pos)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint2(pos, 0));
    }

    template<typename T>
    T load_1d_array_uniform(uint2 pos)
    {
        Texture1DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos.x, pos.y, 0));
    }

    template<typename T>
    T load_2d_uniform(uint2 pos)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos, 0));
    }

    template<typename T>
    T load_2d_array_uniform(uint3 pos)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos.xyz, 0));
    }

    template<typename T>
    T load_3d_uniform(uint3 pos)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos, 0));
    }

    template<typename T>
    void store_1d_uniform(uint pos, T value)
    {
        RWTexture1D<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
    }

    template<typename T>
    void store_1d_array_uniform(uint2 pos, T value)
    {
        RWTexture1DArray<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
    }

    template<typename T>
    void store_2d_uniform(uint2 pos, T value)
    {
        RWTexture2D<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
    }

    template<typename T>
    void store_2d_array_uniform(uint3 pos, T value)
    {
        RWTexture2DArray<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
    }

    template<typename T>
    void store_3d_uniform(uint3 pos, T value)
    {
        RWTexture3D<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
    }

    template<typename T>
    T sample_1d_uniform(SamplerState s, float u)
    {
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, u);
    }

    template<typename T>
    T sample_2d_uniform(SamplerState s, float2 uv)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uv);
    }

    template<typename T>
    T sample_2d_array_uniform(SamplerState s, float2 uv, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, float3(uv, index));
    }

    template<typename T>
    T sample_3d_uniform(SamplerState s, float3 uvw)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uvw);
    }

    template<typename T>
    T sample_level_1d_uniform(SamplerState s, float u, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, u, mip);
    }

    template<typename T>
    T sample_level_2d_uniform(SamplerState s, float2 uv, float mip)
    {
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uv, mip);
    }

    template<typename T>
    T sample_level_2d_array_uniform(SamplerState s, float2 uv, float mip, uint index)
    {
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, float3(uv, index), mip);
    }

    template<typename T>
    T sample_level_3d_uniform(SamplerState s, float3 uvw, float mip)
    {
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uvw, mip);
    }
};
}

#endif
