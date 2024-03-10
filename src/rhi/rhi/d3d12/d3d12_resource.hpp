#pragma once

#include "rhi/resource.hpp"

#include <agility_sdk/d3d12.h>

namespace D3D12MA
{
class Allocation;
}

namespace rhi::d3d12
{
struct D3D12_Buffer : public Buffer
{
    ID3D12Resource2* resource;
    D3D12MA::Allocation* allocation;
};

struct D3D12_Image : public Image
{
    ID3D12Resource2* resource;
    D3D12MA::Allocation* allocation;
};

struct D3D12_Image_View : public Image_View
{
    uint32_t rtv_dsv_index;
};

struct D3D12_Sampler : public Sampler
{};

struct D3D12_Pipeline : public Pipeline
{
    ID3D12PipelineState* pso;
    ID3D12StateObject* rtpso;
};

DXGI_FORMAT translate_format(Image_Format format) noexcept;
D3D12_SRV_DIMENSION translate_view_type_srv(Image_View_Type type) noexcept;
D3D12_UAV_DIMENSION translate_view_type_uav(Image_View_Type type) noexcept;
D3D12_RTV_DIMENSION translate_view_type_rtv(Image_View_Type type) noexcept;
D3D12_DSV_DIMENSION translate_view_type_dsv(Image_View_Type type) noexcept;
}
