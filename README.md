# RHI
Rendering Hardware Interface for D3D12 and Vulkan.

## Table of Contents
- [Building](#building)
- [Usage](#usage)
    - [Device Creation](#device-creation)
    - [Resource Creation](#resources)

## Building
1. Clone the repository: `git clone https://github.com/rtryan98/rhi.git --recurse-submodules --shallow-submodules`.
2. Call CMake inside the cloned repository: `cmake -B build -G "Visual Studio 17 2022"`.
    - Optionally disable building either the D3D12 backend or the Vulkan backend by setting the respective CMake option `RHI_GRAPHICS_API_D3D12` or `RHI_GRAPHICS_API_VULKAN` to `FALSE`.
    However, at least one graphics backend must be built.
    - Optionally disable usage of WinPixEventRuntime by setting the CMake option `RHI_USE_PIX` off.
    This option has no effect if the D3D12 backend is not being built.

## Usage
The RHI is designed to be as easy as possible to use if you're familiar with either Vulkan or D3D12.
To begin using the RHI, you first need to link to it.
```cmake
add_subdirectory(<path/to/rhi>)
target_link_libraries(<your_target> PUBLIC rhi)
```
Once linked you can start using the RHI in your code.

### Device Creation
The first step in using it is to create a `Graphics_Device`.
```cpp
#include <rhi/graphics_device.hpp>
// ...
rhi::Graphics_API_Create_Info device_create_info = {
    .graphics_api = rhi::Graphics_API::D3D12,
    .enable_validation = true,
    .enable_gpu_validation = false,
    .enable_locking = true
};
auto graphics_device = rhi::Graphics_Device::create(device_create_info);
```

### Resources
All resources, like Fences, Buffers, Images, etc. are created using the newly created `Graphics_Device`.
The memory of those resources is owned by the `Graphics_Device`.
Destruction, however, still needs to be managed by the user.
Resources in use by the GPU may must not be destroyed - such destructions need to be protected by a fence.
If `rhi::Graphics_API_Create_Info::enable_locking` was set to `true`, then resources may be created and destroyed from multiple threads at the same time.
Locking does only lock creation and destruction of resources, not reads, as most resources are completely read-only except when creating views, which is also a guarded process.
This does mean that a resource may be destroyed whilst it is being read so it does not solve lifetime races.
However, the address stays valid so this will not cause an illegal access.
```cpp
#include <rhi/resource.hpp>
// ...
rhi::Buffer_Create_Info buffer_create_info = {
    .size = 1ull << 16,
    .heap = rhi::Memory_Heap_Type::GPU
};
auto buffer = graphics_device->create_buffer(buffer_create_info);
```
