#ifndef RHI_BINDLESS_HLSLI
#define RHI_BINDLESS_HLSLI

#ifdef __spirv__
#define DECLARE_PUSH_CONSTANTS(TYPE, VARIABLE) [[vk::push_constant]] TYPE VARIABLE

#define VK_INTERNAL_TEXTURE_BINDING_A 1
#define VK_INTERNAL_TEXTURE_BINDING_B 0

#define VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, TYPE)                          \
    [[vk::binding( VK_INTERNAL_TEXTURE_BINDING_A, VK_INTERNAL_TEXTURE_BINDING_B )]] \
    TEXTYPE<TYPE> g_vk_##TEXTYPE##TYPE[];

#define VK_INTERNAL_DECLARE_TEXTURE_BINDINGS(TEXTYPE)               \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, float)             \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, float2)            \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, float3)            \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, float4)            \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, int)               \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, int2)              \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, int3)              \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, int4)              \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, uint)              \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, uint2)             \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, uint3)             \
    VK_INTERNAL_DECLARE_TEXTURE_BINDING(TEXTYPE, uint4)

#define VK_INTERNAL_GET_TEX_BINDING(TEXTYPE, TYPE)                 \
    g_vk_##TEXTYPE##TYPE

#define VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(TYPE)                                                                                \
    template<> void RW_Texture::store_1d_uniform(uint pos, TYPE value) {                                                                \
        RWTexture1D<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture1D, TYPE)[handle.uav()];                                       \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_1d_array_uniform(uint2 pos, TYPE value) {                                                         \
        RWTexture1DArray<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture1DArray, TYPE)[handle.uav()];                             \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_2d_uniform(uint2 pos, TYPE value) {                                                               \
        RWTexture2D<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture2D, TYPE)[handle.uav()];                                       \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_2d_array_uniform(uint3 pos, TYPE value) {                                                         \
        RWTexture2DArray<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture2DArray, TYPE)[handle.uav()];                             \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_3d_uniform(uint3 pos, TYPE value) {                                                               \
        RWTexture3D<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture3D, TYPE)[handle.uav()];                                       \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_1d(uint pos, TYPE value) {                                                                        \
        RWTexture1D<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture1D, TYPE)[NonUniformResourceIndex(handle.uav())];              \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_1d_array(uint2 pos, TYPE value) {                                                                 \
        RWTexture1DArray<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture1DArray, TYPE)[NonUniformResourceIndex(handle.uav())];    \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_2d(uint2 pos, TYPE value) {                                                                       \
        RWTexture2D<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture2D, TYPE)[NonUniformResourceIndex(handle.uav())];              \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_2d_array(uint3 pos, TYPE value) {                                                                 \
        RWTexture2DArray<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture2DArray, TYPE)[NonUniformResourceIndex(handle.uav())];    \
        texture[pos] = value; }                                                                                                         \
    template<> void RW_Texture::store_3d(uint3 pos, TYPE value) {                                                                       \
        RWTexture3D<TYPE> texture = VK_INTERNAL_GET_TEX_BINDING(RWTexture3D, TYPE)[NonUniformResourceIndex(handle.uav())];              \
        texture[pos] = value; }

[[vk::binding(0, 0)]] ByteAddressBuffer     g_vk_byte_address_buffers[];
[[vk::binding(0, 0)]] RWByteAddressBuffer   g_vk_rw_byte_address_buffers[];
[[vk::binding(1, 0)]] Texture1D             g_vk_texture_1d[];
[[vk::binding(1, 0)]] Texture1DArray        g_vk_texture_1d_array[];
[[vk::binding(1, 0)]] Texture2D             g_vk_texture_2d[];
[[vk::binding(1, 0)]] Texture2DArray        g_vk_texture_2d_array[];
[[vk::binding(1, 0)]] Texture3D             g_vk_texture_3d[];
VK_INTERNAL_DECLARE_TEXTURE_BINDINGS(RWTexture1D);
VK_INTERNAL_DECLARE_TEXTURE_BINDINGS(RWTexture1DArray);
VK_INTERNAL_DECLARE_TEXTURE_BINDINGS(RWTexture2D);
VK_INTERNAL_DECLARE_TEXTURE_BINDINGS(RWTexture2DArray);
VK_INTERNAL_DECLARE_TEXTURE_BINDINGS(RWTexture3D);
// [[vk::binding(1, 0)]] RWTexture1D           g_vk_rw_texture_1d[];
// [[vk::binding(1, 0)]] RWTexture1D           g_vk_rw_texture_1d_array[];
// [[vk::binding(1, 0)]] RWTexture2D           g_vk_rw_texture_2d[];
// [[vk::binding(1, 0)]] RWTexture2DArray      g_vk_rw_texture_2d_array[];
// [[vk::binding(1, 0)]] RWTexture3D           g_vk_rw_texture_3d[];
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

struct Raw_Buffer
{
    Handle handle;

    template<typename T>
    T load()
    {
        #ifdef __spirv__
        return g_vk_byte_address_buffers[handle.srv()].Load<T>(0);
        #else // D3D12
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(0);
        #endif
    }

    template<typename T>
    T load_nuri()
    {
        #ifdef __spirv__
        return g_vk_byte_address_buffers[NonUniformResourceIndex(handle.srv())].Load<T>(0);
        #else // D3D12
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(0);
        #endif
    }
};

struct RW_Raw_Buffer
{
    Handle handle;

    template<typename T>
    T load()
    {
        #ifdef __spirv__
        return g_vk_byte_address_buffers[handle.srv()].Load<T>(0);
        #else // D3D12
        ByteAddressBuffer buffer = ResourceDescriptorHeap[handle.srv()];
        return buffer.Load<T>(0);
        #endif
    }

    template<typename T>
    T load_nuri()
    {
        #ifdef __spirv__
        return g_vk_byte_address_buffers[NonUniformResourceIndex(handle.srv())].Load<T>(0);
        #else // D3D12
        ByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return buffer.Load<T>(0);
        #endif
    }
    
    template<typename T>
    void store(T value)
    {
        #ifdef __spirv__
        g_vk_rw_byte_address_buffers[handle.srv()].Store(0, value);
        #else
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[handle.uav()];
        buffer.Store(0, value);
        #endif
    }
    
    template<typename T>
    void store_nuri(T value)
    {
        #ifdef __spirv__
        g_vk_rw_byte_address_buffers[NonUniformResourceIndex(handle.srv())].Store(0, value);
        #else
        RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        buffer.Store(0, value);
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

    SamplerComparisonState get_cmp()
    {
        #ifdef __spirv__
        return g_vk_samplers[handle.srv()];
        #else
        return SamplerDescriptorHeap[handle.srv()];
        #endif
    }

    SamplerComparisonState get_cmd_nuri()
    {
        #ifdef __spirv__
        return g_vk_samplers[NonUniformResourceIndex(handle.srv())];
        #else
        return SamplerDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        #endif
    }
};

struct Texture
{
    Handle handle;

    template<typename T>
    T load_1d(uint pos)
    {
        #if __spirv__
        return g_vk_texture_1d[NonUniformResourceIndex(handle.srv())].Load(uint2(pos, 0));
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint2(pos, 0));
        #endif
    }

    template<typename T>
    T load_1d_array(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_1d_array[NonUniformResourceIndex(handle.srv())].Load(uint3(pos, 0));
        #else
        Texture1DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos.x, pos.y, 0));
        #endif
    }

    template<typename T>
    T load_2d(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_2d[NonUniformResourceIndex(handle.srv())].Load(uint3(pos, 0));
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos, 0));
        #endif
    }

    template<typename T>
    T load_2d_array(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_2d_array[NonUniformResourceIndex(handle.srv())].Load(uint4(pos, 0));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos.xyz, 0));
        #endif
    }

    template<typename T>
    T load_3d(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_3d[NonUniformResourceIndex(handle.srv())].Load(uint4(pos, 0));
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos, 0));
        #endif
    }

    template<typename T>
    T sample_1d(SamplerState s, float u)
    {
        #if __spirv__
        return g_vk_texture_1d[NonUniformResourceIndex(handle.srv())].Sample(s, u);
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, u);
        #endif
    }

    template<typename T>
    T sample_2d(SamplerState s, float2 uv)
    {
        #if __spirv__
        return g_vk_texture_2d[NonUniformResourceIndex(handle.srv())].Sample(s, uv);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uv);
        #endif
    }

    template<typename T>
    T sample_2d_array(SamplerState s, float2 uv, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[NonUniformResourceIndex(handle.srv())].Sample(s, float3(uv, index));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, float3(uv, index));
        #endif
    }

    template<typename T>
    T sample_3d(SamplerState s, float3 uvw)
    {
        #if __spirv__
        return g_vk_texture_3d[NonUniformResourceIndex(handle.srv())].Sample(s, uvw);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uvw);
        #endif
    }

    template<typename T>
    T sample_level_1d(SamplerState s, float u, float mip)
    {
        #if __spirv__
        return g_vk_texture_1d[NonUniformResourceIndex(handle.srv())].Sample(s, u, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, u, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d(SamplerState s, float2 uv, float mip)
    {
        #if __spirv__
        return g_vk_texture_2d[NonUniformResourceIndex(handle.srv())].Sample(s, uv, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uv, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d_array(SamplerState s, float2 uv, float mip, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[NonUniformResourceIndex(handle.srv())].Sample(s, float3(uv, index), mip);
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, float3(uv, index), mip);
        #endif
    }

    template<typename T>
    T sample_level_3d(SamplerState s, float3 uvw, float mip)
    {
        #if __spirv__
        return g_vk_texture_3d[NonUniformResourceIndex(handle.srv())].Sample(s, uvw, mip);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uvw, mip);
        #endif
    }

    template<typename T>
    T load_1d_uniform(uint pos)
    {
        #if __spirv__
        return g_vk_texture_1d[handle.srv()].Load(uint2(pos, 0));
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint2(pos, 0));
        #endif
    }

    template<typename T>
    T load_1d_array_uniform(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_1d_array[handle.srv()].Load(uint3(pos, 0));
        #else
        Texture1DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos.x, pos.y, 0));
        #endif
    }

    template<typename T>
    T load_2d_uniform(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].Load(uint3(pos, 0));
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos, 0));
        #endif
    }

    template<typename T>
    T load_2d_array_uniform(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_2d_array[handle.srv()].Load(uint4(pos, 0));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos.xyz, 0));
        #endif
    }

    template<typename T>
    T load_3d_uniform(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_3d[handle.srv()].Load(uint4(pos, 0));
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos, 0));
        #endif
    }

    template<typename T>
    T sample_1d_uniform(SamplerState s, float u)
    {
        #if __spirv__
        return g_vk_texture_1d[handle.srv()].Sample(s, u);
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, u);
        #endif
    }

    template<typename T>
    T sample_2d_uniform(SamplerState s, float2 uv)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].Sample(s, uv);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uv);
        #endif
    }

    template<typename T>
    T sample_cmp_2d_uniform(SamplerComparisonState s, float2 uv, float cmp)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].SampleCmp(s, uv, cmp);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleCmp(s, uv, cmp);
        #endif
    }

    template<typename T>
    T sample_2d_array_uniform(SamplerComparisonState s, float2 uv, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[handle.srv()].Sample(s, float3(uv, index));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, float3(uv, index));
        #endif
    }

    template<typename T>
    T sample_3d_uniform(SamplerState s, float3 uvw)
    {
        #if __spirv__
        return g_vk_texture_3d[handle.srv()].Sample(s, uvw);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uvw);
        #endif
    }

    template<typename T>
    T sample_level_1d_uniform(SamplerState s, float u, float mip)
    {
        #if __spirv__
        return g_vk_texture_1d[handle.srv()].Sample(s, u, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, u, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d_uniform(SamplerState s, float2 uv, float mip)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].Sample(s, uv, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uv, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d_array_uniform(SamplerState s, float2 uv, float mip, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[handle.srv()].Sample(s, float3(uv, index), mip);
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, float3(uv, index), mip);
        #endif
    }

    template<typename T>
    T sample_level_3d_uniform(SamplerState s, float3 uvw, float mip)
    {
        #if __spirv__
        return g_vk_texture_3d[handle.srv()].Sample(s, uvw, mip);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uvw, mip);
        #endif
    }
};

struct RW_Texture
{
    Handle handle;

    template<typename T>
    T load_1d(uint pos)
    {
        #if __spirv__
        return g_vk_texture_1d[NonUniformResourceIndex(handle.srv())].Load(uint2(pos, 0));
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint2(pos, 0));
        #endif
    }

    template<typename T>
    T load_1d_array(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_1d_array[NonUniformResourceIndex(handle.srv())].Load(uint3(pos, 0));
        #else
        Texture1DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos.x, pos.y, 0));
        #endif
    }

    template<typename T>
    T load_2d(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_2d[NonUniformResourceIndex(handle.srv())].Load(uint3(pos, 0));
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint3(pos, 0));
        #endif
    }

    template<typename T>
    T load_2d_array(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_2d_array[NonUniformResourceIndex(handle.srv())].Load(uint4(pos, 0));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos.xyz, 0));
        #endif
    }

    template<typename T>
    T load_3d(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_3d[NonUniformResourceIndex(handle.srv())].Load(uint4(pos, 0));
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Load(uint4(pos, 0));
        #endif
    }

    template<typename T>
    void store_1d(uint pos, T value)
    {
        #if __spirv__
        #else
        RWTexture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_1d_array(uint2 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture1DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_2d(uint2 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_2d_array(uint3 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_3d(uint3 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.uav())];
        texture[pos] = value;
        #endif
    }
    

    template<typename T>
    T sample_1d(SamplerState s, float u)
    {
        #if __spirv__
        return g_vk_texture_1d[NonUniformResourceIndex(handle.srv())].Sample(s, u);
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, u);
        #endif
    }

    template<typename T>
    T sample_2d(SamplerState s, float2 uv)
    {
        #if __spirv__
        return g_vk_texture_2d[NonUniformResourceIndex(handle.srv())].Sample(s, uv);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uv);
        #endif
    }

    template<typename T>
    T sample_2d_array(SamplerState s, float2 uv, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[NonUniformResourceIndex(handle.srv())].Sample(s, float3(uv, index));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, float3(uv, index));
        #endif
    }

    template<typename T>
    T sample_3d(SamplerState s, float3 uvw)
    {
        #if __spirv__
        return g_vk_texture_3d[NonUniformResourceIndex(handle.srv())].Sample(s, uvw);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.Sample(s, uvw);
        #endif
    }

    template<typename T>
    T sample_level_1d(SamplerState s, float u, float mip)
    {
        #if __spirv__
        return g_vk_texture_1d[NonUniformResourceIndex(handle.srv())].Sample(s, u, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, u, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d(SamplerState s, float2 uv, float mip)
    {
        #if __spirv__
        return g_vk_texture_2d[NonUniformResourceIndex(handle.srv())].Sample(s, uv, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uv, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d_array(SamplerState s, float2 uv, float mip, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[NonUniformResourceIndex(handle.srv())].Sample(s, float3(uv, index), mip);
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, float3(uv, index), mip);
        #endif
    }

    template<typename T>
    T sample_level_3d(SamplerState s, float3 uvw, float mip)
    {
        #if __spirv__
        return g_vk_texture_3d[NonUniformResourceIndex(handle.srv())].Sample(s, uvw, mip);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[NonUniformResourceIndex(handle.srv())];
        return texture.SampleLevel(s, uvw, mip);
        #endif
    }
    

    template<typename T>
    T load_1d_uniform(uint pos)
    {
        #if __spirv__
        return g_vk_texture_1d[handle.srv()].Load(uint2(pos, 0));
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint2(pos, 0));
        #endif
    }

    template<typename T>
    T load_1d_array_uniform(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_1d_array[handle.srv()].Load(uint3(pos, 0));
        #else
        Texture1DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos.x, pos.y, 0));
        #endif
    }

    template<typename T>
    T load_2d_uniform(uint2 pos)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].Load(uint3(pos, 0));
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint3(pos, 0));
        #endif
    }

    template<typename T>
    T load_2d_array_uniform(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_2d_array[handle.srv()].Load(uint4(pos, 0));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos.xyz, 0));
        #endif
    }

    template<typename T>
    T load_3d_uniform(uint3 pos)
    {
        #if __spirv__
        return g_vk_texture_3d[handle.srv()].Load(uint4(pos, 0));
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Load(uint4(pos, 0));
        #endif
    }

    template<typename T>
    void store_1d_uniform(uint pos, T value)
    {
        #if __spirv__
        #else
        RWTexture1D<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_1d_array_uniform(uint2 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture1DArray<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_2d_uniform(uint2 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture2D<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_2d_array_uniform(uint3 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture2DArray<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    void store_3d_uniform(uint3 pos, T value)
    {
        #if __spirv__
        #else
        RWTexture3D<T> texture = ResourceDescriptorHeap[handle.uav()];
        texture[pos] = value;
        #endif
    }

    template<typename T>
    T sample_1d_uniform(SamplerState s, float u)
    {
        #if __spirv__
        return g_vk_texture_1d[handle.srv()].Sample(s, u);
        #else
        Texture1D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, u);
        #endif
    }

    template<typename T>
    T sample_2d_uniform(SamplerState s, float2 uv)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].Sample(s, uv);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uv);
        #endif
    }

    template<typename T>
    T sample_2d_array_uniform(SamplerState s, float2 uv, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[handle.srv()].Sample(s, float3(uv, index));
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, float3(uv, index));
        #endif
    }

    template<typename T>
    T sample_3d_uniform(SamplerState s, float3 uvw)
    {
        #if __spirv__
        return g_vk_texture_3d[handle.srv()].Sample(s, uvw);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.Sample(s, uvw);
        #endif
    }

    template<typename T>
    T sample_level_1d_uniform(SamplerState s, float u, float mip)
    {
        #if __spirv__
        return g_vk_texture_1d[handle.srv()].Sample(s, u, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, u, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d_uniform(SamplerState s, float2 uv, float mip)
    {
        #if __spirv__
        return g_vk_texture_2d[handle.srv()].Sample(s, uv, mip);
        #else
        Texture2D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uv, mip);
        #endif
    }

    template<typename T>
    T sample_level_2d_array_uniform(SamplerState s, float2 uv, float mip, uint index)
    {
        #if __spirv__
        return g_vk_texture_2d_array[handle.srv()].Sample(s, float3(uv, index), mip);
        #else
        Texture2DArray<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, float3(uv, index), mip);
        #endif
    }

    template<typename T>
    T sample_level_3d_uniform(SamplerState s, float3 uvw, float mip)
    {
        #if __spirv__
        return g_vk_texture_3d[handle.srv()].Sample(s, uvw, mip);
        #else
        Texture3D<T> texture = ResourceDescriptorHeap[handle.srv()];
        return texture.SampleLevel(s, uvw, mip);
        #endif
    }
};

    #ifdef __spirv__
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(float);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(float2);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(float3);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(float4);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(int);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(int2);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(int3);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(int4);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(uint);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(uint2);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(uint3);
    VK_INTERNAL_RWTEXTURE_SPECIALIZE_STORE_OPS(uint4);
    #endif // __spirv__
}

#endif
