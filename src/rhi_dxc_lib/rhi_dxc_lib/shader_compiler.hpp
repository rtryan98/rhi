#pragma once

#include <wrl.h>
#include <directx_shader_compiler/inc/dxcapi.h>
#include <string>
#include <vector>

namespace rhi::dxc
{
constexpr static auto SERIALIZED_MAGIC_NUM = { 'r', 'h', 'i', '_', 's', 'h', 'a', 'd', 'e', 'r' };
constexpr static auto SERIALIZED_MAGIC_NUM_SIZE = 10;

enum class Matrix_Majorness
{
    Row_Major,
    Column_Major
};

enum class Shader_Type
{
    Vertex,
    Pixel,
    Task,
    Mesh,
    Compute,
    Ray_Gen,            // lib
    Ray_Any_Hit,        // lib
    Ray_Closest_Hit,    // lib
    Ray_Miss,           // lib
    Ray_Intersection,   // lib
    Ray_Callable        // lib
};

enum class Shader_Version
{
    SM6_6,
    SM6_7,
    SM6_8,
    SM6_9,
};

struct Shader_Compiler_Settings
{
    std::vector<std::wstring> include_dirs;
    std::vector<std::wstring> defines;
};

struct Shader_Compile_Info
{
    void* data;
    std::size_t data_size;
    std::wstring entrypoint;
    Matrix_Majorness matrix_majorness;
    Shader_Type shader_type;
    Shader_Version version;
    bool embed_debug;
};

struct Shader_Reflection_Data
{
    uint32_t workgroups_x;
    uint32_t workgroups_y;
    uint32_t workgroups_z;
};

struct Shader
{
    Shader_Reflection_Data reflection;
    std::vector<uint8_t> dxil;
    std::vector<uint8_t> spirv;

    std::vector<uint8_t> serialize();
};

class Shader_Compiler
{
public:
    Shader_Compiler();

    Shader compile_from_memory(const Shader_Compiler_Settings& settings, const Shader_Compile_Info& compile_info);

private:
    Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
    Microsoft::WRL::ComPtr<IDxcUtils> m_utils;
};
}
