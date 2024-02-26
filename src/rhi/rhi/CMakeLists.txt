target_sources(
    rhi PRIVATE
    command_list.hpp
    graphics_device.cpp
    graphics_device.hpp
    resource.hpp
    swapchain.hpp
)

if(${RHI_API} STREQUAL "D3D12")
    add_subdirectory(d3d12)
elseif(${RHI_API} STREQUAL "Vulkan")
    add_subdirectory(vulkan)
endif()
