#include "rhi/d3d12/d3d12_swapchain.hpp"

#include "rhi/d3d12/d3d12_resource.hpp"
#include "rhi/d3d12/d3d12_graphics_device.hpp"

#include <core/d3d12/d3d12_descriptor_util.hpp>

namespace rhi::d3d12
{
D3D12_Swapchain::D3D12_Swapchain(D3D12_Graphics_Device* graphics_device, const Swapchain_Win32_Create_Info& create_info) noexcept
    : Swapchain()
    , m_device(graphics_device)
    , m_hwnd(reinterpret_cast<HWND>(create_info.hwnd))
    , m_dxgi_swapchain_buffers()
    , m_images()
    , m_dxgi_swapchain(nullptr)
    , m_current_image_index(0)
{
    DXGI_SWAP_CHAIN_DESC1 desc = {
        .Format = translate_format(create_info.preferred_format),
        .SampleDesc = { .Count = 1, .Quality = 0 },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = create_info.image_count,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = create_info.present_mode == Present_Mode::Vsync
            ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
            : 0u
    };
    IDXGISwapChain1* swapchain1 = nullptr;
    auto context = graphics_device->get_context();
    context->factory->CreateSwapChainForHwnd(
        context->direct_queue,
        reinterpret_cast<HWND>(create_info.hwnd),
        &desc,
        nullptr,
        nullptr,
        &swapchain1);
    swapchain1->QueryInterface(IID_PPV_ARGS(&m_dxgi_swapchain));
    // TODO: unsafe, figure out how to handle error here
    swapchain1->Release();

    recreate_resources();
}

D3D12_Swapchain::~D3D12_Swapchain() noexcept
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_dxgi_swapchain->GetDesc1(&desc);

    for (auto i = 0; i < desc.BufferCount; ++i)
    {
        m_images[i]->resource->Release();
        m_images[i]->resource = nullptr;
        m_images[i]->allocation = nullptr;
        m_device->release_custom_allocated_image(m_images[i]);
    }
    m_dxgi_swapchain->Release();
}

void D3D12_Swapchain::acquire_next_image() noexcept
{
    m_current_image_index = m_dxgi_swapchain->GetCurrentBackBufferIndex();
}

void D3D12_Swapchain::present() noexcept
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_dxgi_swapchain->GetDesc1(&desc);
    m_dxgi_swapchain->Present(
        0,
        bool(desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING)
        ? DXGI_PRESENT_ALLOW_TEARING
        : 0);
}

Swapchain_Resize_Info D3D12_Swapchain::query_resize() noexcept
{
    Swapchain_Resize_Info result = {
        .is_size_changed = false,
        .width = 0,
        .height = 0
    };

    DXGI_SWAP_CHAIN_DESC1 desc;
    m_dxgi_swapchain->GetDesc1(&desc);
    RECT rect;
    bool client_rect_result = GetClientRect(m_hwnd, &rect);
    uint32_t client_width = rect.right - rect.left;
    uint32_t client_height = rect.bottom - rect.top;

    bool has_area = false;
    has_area = (client_width > 0) && (client_height > 0);

    bool resize = false;
    resize = ((client_width != desc.Width) || (client_height != desc.Height)) && has_area;

    if (resize && client_rect_result)
    {
        result.is_size_changed = true;
        result.width = client_width;
        result.height = client_height;

        m_device->wait_idle();

        for (auto i = 0; i < desc.BufferCount; ++i)
        {
            m_images[i]->resource->Release();
            m_images[i]->resource = nullptr;
        }
        m_dxgi_swapchain->ResizeBuffers(
            0,
            client_width,
            client_height,
            DXGI_FORMAT_UNKNOWN,
            desc.Flags);
        recreate_resources();
    }

    return result;
}

Image_Format D3D12_Swapchain::get_image_format() noexcept
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_dxgi_swapchain->GetDesc1(&desc);
    return translate_image_format_to_dxgi_format(desc.Format);
}

Image_View* D3D12_Swapchain::get_current_image_view() noexcept
{
    return m_images[m_current_image_index]->image_view;
}

void D3D12_Swapchain::recreate_resources() noexcept
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_dxgi_swapchain->GetDesc1(&desc);

    if (m_images[0] == nullptr) // No images have been created yet.
    {
        for (auto i = 0; i < desc.BufferCount; ++i)
        {
            m_images[i] = m_device->acquire_custom_allocated_image();
            static_cast<D3D12_Image_View*>(m_images[i]->image_view)->rtv_dsv_index =
                m_device->create_descriptor_index_blocking(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }
    }
    if (desc.Width > 0 && desc.Height > 0)
    {
        for (auto i = 0; i < desc.BufferCount; ++i)
        {
            auto image = m_images[i];
            m_dxgi_swapchain->GetBuffer(i, IID_PPV_ARGS(&image->resource));
            image->allocation = nullptr;
            image->image_view_linked_list_head = nullptr;
            image->format = translate_image_format_to_dxgi_format(desc.Format);
            image->width = desc.Width;
            image->height = desc.Height;
            image->depth = 1;
            image->array_size = 1;
            image->mip_levels = 1;
            image->usage = Image_Usage::Color_Attachment;
            image->primary_view_type = Image_View_Type::Texture_2D;
            image->image_view->bindless_index = ~0u;
            image->image_view->image = image;
            image->image_view->descriptor_type = Descriptor_Type::Color_Attachment;
            static_cast<D3D12_Image_View*>(image->image_view)->next_image_view = nullptr;

            auto cpu_descriptor = m_device->get_cpu_descriptor_handle(
                static_cast<D3D12_Image_View*>(image->image_view)->rtv_dsv_index,
                D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            auto descriptor_desc = core::d3d12::make_full_texture_rtv(
                desc.Format,
                D3D12_RTV_DIMENSION_TEXTURE2D,
                1,
                0,
                0);
            m_device->get_context()->device->CreateRenderTargetView(
                image->resource, &descriptor_desc, cpu_descriptor);
        }
    }
}
}
