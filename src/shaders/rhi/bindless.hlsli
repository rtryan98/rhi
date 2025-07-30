#ifndef RHI_BINDLESS_HLSLI
#define RHI_BINDLESS_HLSLI

#ifdef __spirv__
#define DECLARE_PUSH_CONSTANTS(TYPE, VARIABLE) [[vk::push_constant]] TYPE VARIABLE
#else
#define DECLARE_PUSH_CONSTANTS(TYPE, VARIABLE) ConstantBuffer< TYPE > VARIABLE : register(b0, space0)
#endif

namespace rhi
{
uint id_uav(uint id)
{
#ifdef __spirv__
    return id;
#else
    return id + 1;
#endif
}

// BUFFER LOAD METHODS

template<typename T>
T buf_load(uint id)
{
    ByteAddressBuffer buf = ResourceDescriptorHeap[NonUniformResourceIndex(id)];
    return buf.Load<T>(0);
}

template<typename T>
T buf_load_arr(uint id, uint index)
{
    ByteAddressBuffer buf = ResourceDescriptorHeap[NonUniformResourceIndex(id)];
    return buf.Load<T>(index * sizeof(T));
}

template<typename T, uint Align>
T buf_load_arr_aligned(uint id, uint index)
{
    ByteAddressBuffer buf = ResourceDescriptorHeap[NonUniformResourceIndex(id)];
    return buf.Load<T>(index * Align);
}

// BUFFER STORE METHODS

template<typename T>
void buf_store(uint id, T value)
{
    RWByteAddressBuffer buf = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    buf.Store(0, value);
}

template<typename T>
void buf_store_arr(uint id, uint index, T value)
{
    RWByteAddressBuffer buf = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    buf.Store(index * sizeof(T), value);
}

template<typename T, uint Align>
void buf_store_arr_aligned(uint id, uint index, T value)
{
    RWByteAddressBuffer buf = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    buf.Store(index * Align, value);
}

// TEXTURE GATHER METHODS

template<typename T>
float4 tex_gather(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Gather(s, uv);
}

template<typename T>
float4 tex_gather_red(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherRed(s, uv);
}

template<typename T>
float4 tex_gather_green(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherGreen(s, uv);
}

template<typename T>
float4 tex_gather_blue(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherBlue(s, uv);
}

template<typename T>
float4 tex_gather_alpha(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherAlpha(s, uv);
}

template<typename T>
float4 tex_gather_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Gather(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_red_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherRed(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_green_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherGreen(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_blue_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherBlue(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_alpha_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherAlpha(s, float3(uv, index));
}

// TEXTURE GATHER CMP METHODS

template<typename T>
float4 tex_gather_cmp(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmp(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_red(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpRed(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_green(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpGreen(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_blue(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpBlue(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_alpha(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpAlpha(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmp(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_red_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpRed(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_green_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpGreen(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_blue_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpBlue(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_alpha_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.GatherCmpAlpha(s, float3(uv, index), cmp);
}

// TEXTURE LOAD METHODS

template<typename T>
T tex_load(uint id_texture, uint pos)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint2(pos, 0));
}

template<typename T>
T tex_load(uint id_texture, uint pos, uint mip)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint2(pos, mip));
}

template<typename T>
T tex_load(uint id_texture, uint2 pos)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint3(pos, 0));
}

template<typename T>
T tex_load(uint id_texture, uint2 pos, uint mip)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint3(pos, mip));
}

template<typename T>
T tex_load(uint id_texture, uint3 pos)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint4(pos, 0));
}

template<typename T>
T tex_load(uint id_texture, uint3 pos, uint mip)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint4(pos, mip));
}

template<typename T>
T tex_load_arr(uint id_texture, uint pos, uint index)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint3(pos, index, 0));
}

template<typename T>
T tex_load_arr(uint id_texture, uint pos, uint index, uint mip)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint3(pos, index, mip));
}

template<typename T>
T tex_load_arr(uint id_texture, uint2 pos, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint4(pos, index, 0));
}

template<typename T>
T tex_load_arr(uint id_texture, uint2 pos, uint index, uint mip)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    return tex.Load(uint4(pos, index, mip));
}

// TEXTURE SAMPLE METHODS

template<typename T>
T tex_sample(uint id_texture, uint id_sampler, float u)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, u);
}

template<typename T>
T tex_sample(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, uv);
}

template<typename T>
T tex_sample(uint id_texture, uint id_sampler, float3 uvw)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, uvw);
}

template<typename T>
T tex_sample_arr(uint id_texture, uint id_sampler, float u, uint index)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, float2(u, index));
}

template<typename T>
T tex_sample_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, float3(uv, index));
}

template<typename T>
T tex_sample_cube(uint id_texture, uint id_sampler, float3 uvw)
{
    TextureCube<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, uvw);
}

template<typename T>
T tex_sample_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.Sample(s, float4(uvw, index));
}

// TEXTURE SAMPLE BIAS METHODS

template<typename T>
T tex_sample_bias(uint id_texture, uint id_sampler, float u, float bias)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, u, bias);
}

template<typename T>
T tex_sample_bias(uint id_texture, uint id_sampler, float2 uv, float bias)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, uv, bias);
}

template<typename T>
T tex_sample_bias(uint id_texture, uint id_sampler, float3 uvw, float bias)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, uvw, bias);
}

template<typename T>
T tex_sample_bias_arr(uint id_texture, uint id_sampler, float u, uint index, float bias)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, float2(u, index), bias);
}

template<typename T>
T tex_sample_bias_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float bias)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, float3(uv, index), bias);
}

template<typename T>
T tex_sample_bias_cube(uint id_texture, uint id_sampler, float3 uvw, float bias)
{
    TextureCube<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, uvw, bias);
}

template<typename T>
T tex_sample_bias_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float bias)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleBias(s, float4(uvw, index), bias);
}

// TEXTURE SAMPLE CMP METHODS

template<typename T>
T tex_sample_cmp(uint id_texture, uint id_sampler, float u, float cmp)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, u, cmp);
}

template<typename T>
T tex_sample_cmp(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, uv, cmp);
}

template<typename T>
T tex_sample_cmp(uint id_texture, uint id_sampler, float3 uvw, float cmp)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, uvw, cmp);
}

template<typename T>
T tex_sample_cmp_arr(uint id_texture, uint id_sampler, float u, uint index, float cmp)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, float2(u, index), cmp);
}

template<typename T>
T tex_sample_cmp_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float cmp)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, float3(uv, index), cmp);
}

template<typename T>
T tex_sample_cmp_cube(uint id_texture, uint id_sampler, float3 uvw, float cmp)
{
    TextureCube<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, uvw, cmp);
}

template<typename T>
T tex_sample_cmp_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float cmp)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmp(s, float4(uvw, index), cmp);
}

// TEXTURE SAMPLE CMP LEVEL METHODS

template<typename T>
T tex_sample_cmp_level(uint id_texture, uint id_sampler, float u, float cmp, float mip)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, u, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level(uint id_texture, uint id_sampler, float2 uv, float cmp, float mip)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, uv, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level(uint id_texture, uint id_sampler, float3 uvw, float cmp, float mip)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, uvw, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_arr(uint id_texture, uint id_sampler, float u, uint index, float cmp, float mip)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, float2(u, index), cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float cmp, float mip)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, float3(uv, index), cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_cube(uint id_texture, uint id_sampler, float3 uvw, float cmp, float mip)
{
    TextureCube<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, uvw, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float cmp, float mip)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleCmpLevel(s, float4(uvw, index), cmp, mip);
}

// TEXTURE SAMPLE GRAD METHODS

template<typename T>
T tex_sample_grad(uint id_texture, uint id_sampler, float u, float ddx, float ddy)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, u, ddx, ddy);
}

template<typename T>
T tex_sample_grad(uint id_texture, uint id_sampler, float2 uv, float ddx, float ddy)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, uv, ddx, ddy);
}

template<typename T>
T tex_sample_grad(uint id_texture, uint id_sampler, float3 uvw, float ddx, float ddy)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, uvw, ddx, ddy);
}

template<typename T>
T tex_sample_grad_arr(uint id_texture, uint id_sampler, float u, uint index, float ddx, float ddy)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, float2(u, index), ddx, ddy);
}

template<typename T>
T tex_sample_grad_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float ddx, float ddy)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, float3(uv, index), ddx, ddy);
}

template<typename T>
T tex_sample_grad_cube(uint id_texture, uint id_sampler, float3 uvw, float ddx, float ddy)
{
    TextureCube<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, uvw, ddx, ddy);
}

template<typename T>
T tex_sample_grad_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float ddx, float ddy)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleGrad(s, float4(uvw, index), ddx, ddy);
}

// TEXTURE SAMPLE LEVEL METHODS

template<typename T>
T tex_sample_level(uint id_texture, uint id_sampler, float u, float mip)
{
    Texture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, u, mip);
}

template<typename T>
T tex_sample_level(uint id_texture, uint id_sampler, float2 uv, float mip)
{
    Texture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, uv, mip);
}

template<typename T>
T tex_sample_level(uint id_texture, uint id_sampler, float3 uvw, float mip)
{
    Texture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, uvw, mip);
}

template<typename T>
T tex_sample_level_arr(uint id_texture, uint id_sampler, float u, uint index, float mip)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, float2(u, index), mip);
}

template<typename T>
T tex_sample_level_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float mip)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, float3(uv, index), mip);
}

template<typename T>
T tex_sample_level_cube(uint id_texture, uint id_sampler, float3 uvw, float mip)
{
    TextureCube<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, uvw, mip);
}

template<typename T>
T tex_sample_level_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float mip)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_texture)];
    SamplerState s = SamplerDescriptorHeap[NonUniformResourceIndex(id_sampler)];
    return tex.SampleLevel(s, float4(uvw, index), mip);
}

// TEXTURE STORE METHODS

template<typename T>
void tex_store(uint id_texture, uint pos, T value)
{
    RWTexture1D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id_texture))];
    tex[pos] = value;
}

template<typename T>
void tex_store_load(uint id_texture, uint2 pos, T value)
{
    RWTexture2D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id_texture))];
    tex[pos] = value;
}

template<typename T>
void tex_store(uint id_texture, uint3 pos, T value)
{
    RWTexture3D<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id_texture))];
    tex[pos] = value;
}

template<typename T>
void tex_store_arr(uint id_texture, uint2 pos, uint index, T value)
{
    RWTexture2DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id_texture))];
    tex[uint3(pos, index)] = value;
}

template<typename T>
void tex_store_arr(uint id_texture, uint pos, uint index, T value)
{
    RWTexture1DArray<T> tex = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id_texture))];
    tex[uint2(pos, index)] = value;
}

namespace atomic
{
// BUFFER ATOMIC METHODS

template<typename T, typename Element>
Element buf_add(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_and(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedAnd(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_compare_exchange(uint id, uint index, uint value_offset_in_struct, Element compare, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedCompareExchange(index * sizeof(T) + value_offset_in_struct, compare, value, original_value);
    return original_value;
}

template<typename T, typename Element>
void buf_compare_store(uint id, uint index, uint value_offset_in_struct, Element compare, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    buffer.InterlockedCompareStore(index * sizeof(T) + value_offset_in_struct, compare, value);
}

template<typename T, typename Element>
Element buf_exchange(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedExchange(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_max(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedMax(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_min(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedMin(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_or(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedOr(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_xor(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[NonUniformResourceIndex(id_uav(id))];
    Element original_value;
    buffer.InterlockedXor(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}
} // namespace atomic

namespace uni // uniform
{

// BUFFER LOAD METHODS

template<typename T>
T buf_load(uint id)
{
    ByteAddressBuffer buf = ResourceDescriptorHeap[(id)];
    return buf.Load<T>(0);
}

template<typename T>
T buf_load_arr(uint id, uint index)
{
    ByteAddressBuffer buf = ResourceDescriptorHeap[(id)];
    return buf.Load<T>(index * sizeof(T));
}

template<typename T, uint Align>
T buf_load_arr_aligned(uint id, uint index, T value)
{
    ByteAddressBuffer buf = ResourceDescriptorHeap[(id)];
    return buf.Load<T>(index * Align);
}

// BUFFER STORE METHODS

template<typename T>
void buf_store(uint id, T value)
{
    RWByteAddressBuffer buf = ResourceDescriptorHeap[(id_uav(id))];
    buf.Store(0, value);
}

template<typename T>
void buf_store_arr(uint id, uint index, T value)
{
    RWByteAddressBuffer buf = ResourceDescriptorHeap[(id_uav(id))];
    buf.Store(index * sizeof(T), value);
}

template<typename T, uint Align>
void buf_store_arr_aligned(uint id, uint index, T value)
{
    RWByteAddressBuffer buf = ResourceDescriptorHeap[(id_uav(id))];
    buf.Store(index * Align, value);
}

// TEXTURE GATHER METHODS

template<typename T>
float4 tex_gather(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Gather(s, uv);
}

template<typename T>
float4 tex_gather_red(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherRed(s, uv);
}

template<typename T>
float4 tex_gather_green(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherGreen(s, uv);
}

template<typename T>
float4 tex_gather_blue(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherBlue(s, uv);
}

template<typename T>
float4 tex_gather_alpha(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherAlpha(s, uv);
}

template<typename T>
float4 tex_gather_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Gather(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_red_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherRed(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_green_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherGreen(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_blue_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherBlue(s, float3(uv, index));
}

template<typename T>
float4 tex_gather_alpha_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherAlpha(s, float3(uv, index));
}

// TEXTURE GATHER CMP METHODS

template<typename T>
float4 tex_gather_cmp(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmp(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_red(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpRed(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_green(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpGreen(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_blue(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpBlue(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_alpha(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpAlpha(s, uv, cmp);
}

template<typename T>
float4 tex_gather_cmp_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmp(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_red_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpRed(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_green_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpGreen(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_blue_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpBlue(s, float3(uv, index), cmp);
}

template<typename T>
float4 tex_gather_cmp_alpha_arr(uint id_texture, uint id_sampler, float2 uv, float cmp, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.GatherCmpAlpha(s, float3(uv, index), cmp);
}

// TEXTURE LOAD METHODS

template<typename T>
T tex_load(uint id_texture, uint pos)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint2(pos, 0));
}

template<typename T>
T tex_load(uint id_texture, uint pos, uint mip)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint2(pos, mip));
}

template<typename T>
T tex_load(uint id_texture, uint2 pos)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint3(pos, 0));
}

template<typename T>
T tex_load(uint id_texture, uint2 pos, uint mip)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint3(pos, mip));
}

template<typename T>
T tex_load(uint id_texture, uint3 pos)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint4(pos, 0));
}

template<typename T>
T tex_load(uint id_texture, uint3 pos, uint mip)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint4(pos, mip));
}

template<typename T>
T tex_load_arr(uint id_texture, uint pos, uint index)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint3(pos, index, 0));
}

template<typename T>
T tex_load_arr(uint id_texture, uint pos, uint index, uint mip)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint3(pos, index, mip));
}

template<typename T>
T tex_load_arr(uint id_texture, uint2 pos, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint4(pos, index, 0));
}

template<typename T>
T tex_load_arr(uint id_texture, uint2 pos, uint index, uint mip)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    return tex.Load(uint4(pos, index, mip));
}

// TEXTURE SAMPLE METHODS

template<typename T>
T tex_sample(uint id_texture, uint id_sampler, float u)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, u);
}

template<typename T>
T tex_sample(uint id_texture, uint id_sampler, float2 uv)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, uv);
}

template<typename T>
T tex_sample(uint id_texture, uint id_sampler, float3 uvw)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, uvw);
}

template<typename T>
T tex_sample_arr(uint id_texture, uint id_sampler, float u, uint index)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, float2(u, index));
}

template<typename T>
T tex_sample_arr(uint id_texture, uint id_sampler, float2 uv, uint index)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, float3(uv, index));
}

template<typename T>
T tex_sample_cube(uint id_texture, uint id_sampler, float3 uvw)
{
    TextureCube<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, uvw);
}

template<typename T>
T tex_sample_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.Sample(s, float4(uvw, index));
}

// TEXTURE SAMPLE BIAS METHODS

template<typename T>
T tex_sample_bias(uint id_texture, uint id_sampler, float u, float bias)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, u, bias);
}

template<typename T>
T tex_sample_bias(uint id_texture, uint id_sampler, float2 uv, float bias)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, uv, bias);
}

template<typename T>
T tex_sample_bias(uint id_texture, uint id_sampler, float3 uvw, float bias)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, uvw, bias);
}

template<typename T>
T tex_sample_bias_arr(uint id_texture, uint id_sampler, float u, uint index, float bias)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, float2(u, index), bias);
}

template<typename T>
T tex_sample_bias_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float bias)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, float3(uv, index), bias);
}

template<typename T>
T tex_sample_bias_cube(uint id_texture, uint id_sampler, float3 uvw, float bias)
{
    TextureCube<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, uvw, bias);
}

template<typename T>
T tex_sample_bias_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float bias)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleBias(s, float4(uvw, index), bias);
}

// TEXTURE SAMPLE CMP METHODS

template<typename T>
T tex_sample_cmp(uint id_texture, uint id_sampler, float u, float cmp)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, u, cmp);
}

template<typename T>
T tex_sample_cmp(uint id_texture, uint id_sampler, float2 uv, float cmp)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, uv, cmp);
}

template<typename T>
T tex_sample_cmp(uint id_texture, uint id_sampler, float3 uvw, float cmp)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, uvw, cmp);
}

template<typename T>
T tex_sample_cmp_arr(uint id_texture, uint id_sampler, float u, uint index, float cmp)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, float2(u, index), cmp);
}

template<typename T>
T tex_sample_cmp_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float cmp)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, float3(uv, index), cmp);
}

template<typename T>
T tex_sample_cmp_cube(uint id_texture, uint id_sampler, float3 uvw, float cmp)
{
    TextureCube<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, uvw, cmp);
}

template<typename T>
T tex_sample_cmp_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float cmp)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmp(s, float4(uvw, index), cmp);
}

// TEXTURE SAMPLE CMP LEVEL METHODS

template<typename T>
T tex_sample_cmp_level(uint id_texture, uint id_sampler, float u, float cmp, float mip)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, u, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level(uint id_texture, uint id_sampler, float2 uv, float cmp, float mip)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, uv, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level(uint id_texture, uint id_sampler, float3 uvw, float cmp, float mip)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, uvw, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_arr(uint id_texture, uint id_sampler, float u, uint index, float cmp, float mip)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, float2(u, index), cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float cmp, float mip)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, float3(uv, index), cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_cube(uint id_texture, uint id_sampler, float3 uvw, float cmp, float mip)
{
    TextureCube<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, uvw, cmp, mip);
}

template<typename T>
T tex_sample_cmp_level_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float cmp, float mip)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerComparisonState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleCmpLevel(s, float4(uvw, index), cmp, mip);
}

// TEXTURE SAMPLE GRAD METHODS

template<typename T>
T tex_sample_grad(uint id_texture, uint id_sampler, float u, float ddx, float ddy)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, u, ddx, ddy);
}

template<typename T>
T tex_sample_grad(uint id_texture, uint id_sampler, float2 uv, float ddx, float ddy)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, uv, ddx, ddy);
}

template<typename T>
T tex_sample_grad(uint id_texture, uint id_sampler, float3 uvw, float ddx, float ddy)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, uvw, ddx, ddy);
}

template<typename T>
T tex_sample_grad_arr(uint id_texture, uint id_sampler, float u, uint index, float ddx, float ddy)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, float2(u, index), ddx, ddy);
}

template<typename T>
T tex_sample_grad_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float ddx, float ddy)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, float3(uv, index), ddx, ddy);
}

template<typename T>
T tex_sample_grad_cube(uint id_texture, uint id_sampler, float3 uvw, float ddx, float ddy)
{
    TextureCube<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, uvw, ddx, ddy);
}

template<typename T>
T tex_sample_grad_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float ddx, float ddy)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleGrad(s, float4(uvw, index), ddx, ddy);
}

// TEXTURE SAMPLE LEVEL METHODS

template<typename T>
T tex_sample_level(uint id_texture, uint id_sampler, float u, float mip)
{
    Texture1D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, u, mip);
}

template<typename T>
T tex_sample_level(uint id_texture, uint id_sampler, float2 uv, float mip)
{
    Texture2D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, uv, mip);
}

template<typename T>
T tex_sample_level(uint id_texture, uint id_sampler, float3 uvw, float mip)
{
    Texture3D<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, uvw, mip);
}

template<typename T>
T tex_sample_level_arr(uint id_texture, uint id_sampler, float u, uint index, float mip)
{
    Texture1DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, float2(u, index), mip);
}

template<typename T>
T tex_sample_level_arr(uint id_texture, uint id_sampler, float2 uv, uint index, float mip)
{
    Texture2DArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, float3(uv, index), mip);
}

template<typename T>
T tex_sample_level_cube(uint id_texture, uint id_sampler, float3 uvw, float mip)
{
    TextureCube<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, uvw, mip);
}

template<typename T>
T tex_sample_level_cube_arr(uint id_texture, uint id_sampler, float3 uvw, uint index, float mip)
{
    TextureCubeArray<T> tex = ResourceDescriptorHeap[(id_texture)];
    SamplerState s = SamplerDescriptorHeap[(id_sampler)];
    return tex.SampleLevel(s, float4(uvw, index), mip);
}

// TEXTURE STORE METHODS

template<typename T>
void tex_store(uint id_texture, uint pos, T value)
{
    RWTexture1D<T> tex = ResourceDescriptorHeap[(id_uav(id_texture))];
    tex[pos] = value;
}

template<typename T>
void tex_store_load(uint id_texture, uint2 pos, T value)
{
    RWTexture2D<T> tex = ResourceDescriptorHeap[(id_uav(id_texture))];
    tex[pos] = value;
}

template<typename T>
void tex_store(uint id_texture, uint3 pos, T value)
{
    RWTexture3D<T> tex = ResourceDescriptorHeap[(id_uav(id_texture))];
    tex[pos] = value;
}

template<typename T>
void tex_store_arr(uint id_texture, uint2 pos, uint index, T value)
{
    RWTexture2DArray<T> tex = ResourceDescriptorHeap[(id_uav(id_texture))];
    tex[uint3(pos, index)] = value;
}

template<typename T>
void tex_store_arr(uint id_texture, uint pos, uint index, T value)
{
    RWTexture1DArray<T> tex = ResourceDescriptorHeap[(id_uav(id_texture))];
    tex[uint2(pos, index)] = value;
}

namespace atomic
{
// BUFFER ATOMIC METHODS

template<typename T, typename Element>
Element buf_add(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedAdd(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_and(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedAnd(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_compare_exchange(uint id, uint index, uint value_offset_in_struct, Element compare, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedCompareExchange(index * sizeof(T) + value_offset_in_struct, compare, value, original_value);
    return original_value;
}

template<typename T, typename Element>
void buf_compare_store(uint id, uint index, uint value_offset_in_struct, Element compare, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    buffer.InterlockedCompareStore(index * sizeof(T) + value_offset_in_struct, compare, value);
}

template<typename T, typename Element>
Element buf_exchange(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedExchange(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_max(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedMax(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_min(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedMin(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_or(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedOr(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}

template<typename T, typename Element>
Element buf_xor(uint id, uint index, uint value_offset_in_struct, Element value)
{
    RWByteAddressBuffer buffer = ResourceDescriptorHeap[(id_uav(id))];
    Element original_value;
    buffer.InterlockedXor(index * sizeof(T) + value_offset_in_struct, value, original_value);
    return original_value;
}
} // namespace atomic
} // namespace uni
}

#endif
