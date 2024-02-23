target_sources(
    rhi PRIVATE
    graphics_device.cpp
    graphics_device.hpp
)

if(${RHI_API} STREQUAL "D3D12")
    add_subdirectory(d3d12)
elseif(${RHI_API} STREQUAL "Vulkan")
    add_subdirectory(vulkan)
endif()
