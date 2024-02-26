cmake_minimum_required(VERSION 3.20)

project(
    rhi
    VERSION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(thirdparty)

# set RHI_API to either "D3D12" or "Vulkan".
if(NOT DEFINED RHI_API)
    set(RHI_API "D3D12")
endif()

add_library(
    rhi
)
target_link_libraries(
    rhi PUBLIC
    core_lib
)
target_include_directories(
    rhi PUBLIC
    src/rhi
)
set_target_properties(
    rhi PROPERTIES
    CXX_STANDARD 23
)
if(${RHI_API} STREQUAL "D3D12")
    target_compile_definitions(rhi PUBLIC RHI_GRAPHICS_API_D3D12)
elseif(${RHI_API} STREQUAL "D3D12")
    target_compile_definitions(rhi PUBLIC RHI_GRAPHICS_API_VULKAN)
endif()

add_subdirectory(src)
get_target_property(RHI_SOURCES rhi SOURCES)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/rhi PREFIX src FILES ${RHI_SOURCES})
