target_sources(
    rhi PRIVATE
    command_list.hpp
    graphics_device.cpp
    graphics_device.hpp
    resource.hpp
    swapchain.hpp
)

add_subdirectory(common)

if(${RHI_BUILD_D3D12})
    add_subdirectory(d3d12)
elseif(${RHI_BUILD_VULKAN})
    add_subdirectory(vulkan)
endif()
