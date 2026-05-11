#pragma once

#include <cstdint>

namespace rhi::d3d12
{
template<typename D3D12_Type, typename BaseType>
D3D12_Type d3d12_cast(const BaseType base);

template<typename D3D12_Type, typename BaseType>
D3D12_Type d3d12_cast(const BaseType base, uint32_t identity);

template<typename D3D12_Type, typename BaseType>
D3D12_Type d3d12_cast(const BaseType r, const BaseType g, const BaseType b, const BaseType a);
}
