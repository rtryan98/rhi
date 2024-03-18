#pragma once

#include "rhi/swapchain.hpp"
#include "rhi/d3d12/d3d12_resource.hpp"

#include <array>
#include <agility_sdk/d3d12.h>
#include <dxgi1_6.h>

namespace rhi::d3d12
{
class D3D12_Graphics_Device;

class D3D12_Swapchain : public Swapchain
{
public:
    D3D12_Swapchain(
        D3D12_Graphics_Device* graphics_device,
        const Swapchain_Win32_Create_Info& create_info) noexcept;
    virtual ~D3D12_Swapchain() noexcept;

    virtual void acquire_next_image() noexcept override;
    virtual void present() noexcept override;

    virtual [[nodiscard]] Swapchain_Resize_Info query_resize() noexcept override;
    virtual [[nodiscard]] Image_Format get_image_format() noexcept override;
    virtual [[nodiscard]] Image_View* get_current_image_view() noexcept override;

private:
    void recreate_resources() noexcept;

private:
    D3D12_Graphics_Device* m_device;
    HWND m_hwnd;
    std::array<ID3D12Resource*, DXGI_MAX_SWAP_CHAIN_BUFFERS> m_dxgi_swapchain_buffers;
    std::array<D3D12_Image*, DXGI_MAX_SWAP_CHAIN_BUFFERS> m_images;
    IDXGISwapChain4* m_dxgi_swapchain;
    uint32_t m_current_image_index;
};
}
