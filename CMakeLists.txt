cmake_minimum_required(VERSION 3.20)

project(
    rhi
    VERSION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(thirdparty)

core_download_and_extract_zip(
    https://www.nuget.org/api/v2/package/WinPixEventRuntime/1.0.231030001
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty
    win_pix_event_runtime
)
core_deploy_files(
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/win_pix_event_runtime/bin/x64/WinPixEventRuntime.dll
    .
)

# set RHI_API to either "D3D12" or "Vulkan".
if(NOT DEFINED RHI_API)
    set(RHI_API "D3D12")
endif()

option(RHI_USE_PIX "If true, compiles with WinPixEventRuntime linked when compiling for D3D12." TRUE)

add_library(
    rhi
)
target_link_libraries(
    rhi PUBLIC
    core_lib
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/win_pix_event_runtime/bin/x64/WinPixEventRuntime.lib
)
target_include_directories(
    rhi PUBLIC
    src/rhi
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/win_pix_event_runtime/Include
)
set_target_properties(
    rhi PROPERTIES
    CXX_STANDARD 23
)
if(${RHI_API} STREQUAL "D3D12")
    target_compile_definitions(rhi PUBLIC
        RHI_GRAPHICS_API_D3D12
    )
    if(${RHI_USE_PIX})
        target_compile_definitions(rhi PUBLIC
            USE_PIX)
    endif()
elseif(${RHI_API} STREQUAL "Vulkan")
    target_compile_definitions(rhi PUBLIC
        RHI_GRAPHICS_API_VULKAN
    )
endif()

add_subdirectory(src)
get_target_property(RHI_SOURCES rhi SOURCES)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/rhi PREFIX src FILES ${RHI_SOURCES})
