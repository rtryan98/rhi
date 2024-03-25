#pragma once

#include <wrl.h>
#include <dxcapi.h>
#include <string>
#include <vector>

namespace rhi::dxc
{
constexpr static auto SERIALIZED_MAGIC_NUM = { 'r', 'h', 'i', '_', 's', 'h', 'a', 'd', 'e', 'r' };
constexpr static auto SERIALIZED_MAGIC_NUM_SIZE = 10;

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
    std::wstring shader_model;
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
    Shader compile_from_memory(const Shader_Compiler_Settings& settings, const Shader_Compile_Info& compile_info);

private:
    Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
    Microsoft::WRL::ComPtr<IDxcUtils> m_utils;
};
}
