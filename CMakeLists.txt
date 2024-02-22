cmake_minimum_required(VERSION 3.20)

project(
    rhi
    VERION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(thirdparty)

add_library(
    rhi
)
target_link_libraries(
    rhi PUBLIC
    core
)
target_include_directories(
    rhi PUBLIC
    src/rhi
)
set_target_properties(
    rhi PROPERTIES
    CXX_STANDARD 23
)

add_subdirectory(src)
