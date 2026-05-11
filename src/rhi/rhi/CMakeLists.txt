target_sources(
    rhi PRIVATE
    command_list.hpp
    graphics_device.cpp
    graphics_device.hpp
    image_format.cpp
    image_format.hpp
    resource.cpp
    resource.hpp
    result.hpp
    swapchain.hpp
)

add_subdirectory(common)

if(${RHI_BUILD_D3D12})
    add_subdirectory(d3d12)
endif()
if(${RHI_BUILD_VULKAN})
    add_subdirectory(vulkan)
endif()
