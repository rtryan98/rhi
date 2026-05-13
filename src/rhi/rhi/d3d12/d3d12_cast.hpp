#pragma once

#include <cstdint>

namespace rhi
{
enum class Sampler_Filter;
enum class Sampler_Reduction_Type;
}

namespace rhi::d3d12
{
template<typename D3D12_Type, typename Base_Type>
D3D12_Type d3d12_cast(const Base_Type base);

template<typename D3D12_Type, typename Base_Type>
D3D12_Type d3d12_cast(const Base_Type base, uint32_t identity);

template<typename D3D12_Type, typename Base_Type>
D3D12_Type d3d12_cast(const Base_Type r, const Base_Type g, const Base_Type b, const Base_Type a);

template<typename D3D12_Type>
D3D12_Type d3d12_cast(Sampler_Filter min, Sampler_Filter mag, Sampler_Filter mip, Sampler_Reduction_Type reduction, bool aniso);

template<typename D3D12_Type, typename Base_Type>
D3D12_Type d3d12_cast(Base_Type* type);
}
