#pragma once
#ifdef RHI_GRAPHICS_API_D3D12

#include "rhi/resource.hpp"

#include <agility_sdk/d3d12.h>

namespace rhi::d3d12
{
struct D3D12_Buffer : public Buffer
{

};

struct D3D12_Pipeline : public Pipeline
{
    ID3D12PipelineState* pso;
    ID3D12StateObject* rtpso;
};
}

#endif // RHI_GRAPHICS_API_D3D12
