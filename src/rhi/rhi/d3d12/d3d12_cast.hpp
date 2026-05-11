#pragma once

namespace rhi::d3d12
{
template<typename D3D12_Type, typename BaseType>
D3D12_Type d3d12_cast(const BaseType base);
}
