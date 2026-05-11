add_subdirectory(D3D12MemoryAllocator)

if(WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

add_subdirectory(volk)
add_subdirectory(VulkanMemoryAllocator)
add_subdirectory(vk-bootstrap)
