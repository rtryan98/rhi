#pragma once
#ifdef RHI_GRAPHICS_API_D3D12

#include "rhi/command_list.hpp"

namespace rhi::d3d12
{
class D3D12_Command_List final : public Command_List
{

};
}

#endif // RHI_GRAPHICS_API_D3D12
