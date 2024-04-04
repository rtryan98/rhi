cmake_minimum_required(VERSION 3.20)

project(
    rhi
    VERSION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(thirdparty)

core_download_and_extract_zip(
    https://www.nuget.org/api/v2/package/WinPixEventRuntime/1.0.240308001
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty
    win_pix_event_runtime
)
core_deploy_files(
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/win_pix_event_runtime/bin/x64/WinPixEventRuntime.dll
    .
)

core_download_and_extract_zip(
    https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2403.1/dxc_2024_03_22.zip
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty
    directx_shader_compiler
)
core_deploy_files(
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/directx_shader_compiler/bin/x64/
    .
)

option(RHI_BUILD_D3D12 "If true, builds the RHI D3D12 backend." ON)
option(RHI_BUILD_VULKAN "If true, builds the RHI Vulkan backend." ON)
option(RHI_USE_PIX "If true, compiles the RHI with WinPixEventRuntime linked when compiling for D3D12." ON)
option(RHI_BUILD_TESTS "If true, builds the RHI tests" OFF)

add_library(rhi)
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
if(${RHI_BUILD_D3D12})
    target_compile_definitions(
        rhi PUBLIC
        RHI_GRAPHICS_API_D3D12
    )
    target_compile_definitions(
        rhi PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    if(${RHI_USE_PIX})
        target_compile_definitions(
            rhi PUBLIC
            USE_PIX)
    endif()
endif()
if(${RHI_BUILD_VULKAN})
    target_compile_definitions(
        rhi PUBLIC
        RHI_GRAPHICS_API_VULKAN
    )
endif()
if(NOT ${RHI_BUILD_D3D12} AND NOT ${RHI_BUILD_VULKAN})
    message(WARNING "No graphics API backend is being built. RHI will be unusable.")
endif()

add_library(rhi_dxc_lib)
target_link_libraries(
    rhi_dxc_lib PUBLIC
    rhi
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/directx_shader_compiler/lib/x64/dxcompiler.lib
)
target_include_directories(
    rhi_dxc_lib PUBLIC
    src/rhi_dxc_lib
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/directx_shader_compiler/inc
)
set_target_properties(
    rhi_dxc_lib PROPERTIES
    CXX_STANDARD 23
)
target_compile_definitions(
    rhi_dxc_lib PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

add_subdirectory(src)
get_target_property(RHI_SOURCES rhi SOURCES)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/rhi PREFIX src FILES ${RHI_SOURCES})
get_target_property(RHI_DXC_LIB_SOURCES rhi_dxc_lib SOURCES)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/rhi_dxc_lib PREFIX src FILES ${RHI_DXC_LIB_SOURCES})

if(${RHI_BUILD_TESTS})
    message(STATUS "Building RHI tests.")

    include(CTest)
    include(${CMAKE_CURRENT_LIST_DIR}/thirdparty/catch2/extras/Catch.cmake)

    add_executable(rhi_tests)
    target_link_libraries(
        rhi_tests PRIVATE
        rhi
        Catch2::Catch2WithMain)
    set_target_properties(
        rhi_tests PROPERTIES
        CXX_STANDARD 23
    )
    add_subdirectory(test)
    get_target_property(RHI_TESTS_SOURCES rhi_tests SOURCES)
    source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/test/rhi PREFIX test FILES ${RHI_TESTS_SOURCES})

    catch_discover_tests(rhi_tests)
endif()
