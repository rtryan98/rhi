#include "rhi_dxc_lib/shader_compiler.hpp"

#include <d3d12shader.h>

namespace rhi::dxc
{
using Microsoft::WRL::ComPtr;

constexpr std::vector<std::wstring> get_default_compile_args()
{
    std::vector<std::wstring> result = {};
    result.push_back(L"-enable-16bit-types");
    result.push_back(L"-HV");
    result.push_back(L"2021");
    result.push_back(L"-no-legacy-cbuf-layout");
    result.push_back(L"-Zpr");
    result.push_back(L"-O3");
    // result.push_back(L"-Qstrip_reflect");
    return result;
}

constexpr std::vector<std::wstring> get_spirv_args()
{
    std::vector<std::wstring> result = {};
    result.push_back(L"-spirv");
    result.push_back(L"-fvk-invert-y");
    result.push_back(L"-fvk-use-dx-position-w");
    result.push_back(L"-fvk-use-scalar-layout");
    result.push_back(L"-fspv-use-legacy-buffer-matrix-order");
    return result;
}

Shader_Compiler::Shader_Compiler()
    : m_compiler()
    , m_utils()
{
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler));
}

Shader Shader_Compiler::compile_from_memory(
    const Shader_Compiler_Settings& settings,
    const Shader_Compile_Info& compile_info)
{
    constexpr static uint32_t USER_ARG_COUNT = 2 * 2;
    constexpr static uint32_t SPIRV_ARG_COUNT = 0;

    Shader result = {};

    ComPtr<IDxcResult> result_dxil = nullptr;
    ComPtr<IDxcResult> result_spirv = nullptr;

    std::vector<const wchar_t*> arguments_wchar_ptr = {};
    std::vector<std::wstring> arguments = {};
    arguments.reserve(
        SPIRV_ARG_COUNT +
        USER_ARG_COUNT +
        settings.include_dirs.size() * 2 +
        settings.defines.size() * 2);

    ComPtr<IDxcIncludeHandler> include_handler;
    m_utils->CreateDefaultIncludeHandler(include_handler.GetAddressOf());

    for (const auto& argument : settings.defines)
    {
        arguments.push_back(L"-D");
        arguments.push_back(argument);
    }
    for (const auto& include_dir : settings.include_dirs)
    {
        arguments.push_back(L"-I");
        arguments.push_back(include_dir);
    }

    arguments.push_back(L"-T");
    arguments.push_back(compile_info.shader_model);

    arguments.push_back(L"-E");
    arguments.push_back(compile_info.entrypoint);

    arguments_wchar_ptr.reserve(arguments.size());
    for (const auto& arg : arguments)
    {
        arguments_wchar_ptr.push_back(arg.c_str());
    }

    const auto default_args = get_default_compile_args();
    for (const auto& arg : default_args)
    {
        arguments_wchar_ptr.push_back(arg.c_str());
    }

    DxcBuffer source = {
        .Ptr = compile_info.data,
        .Size = compile_info.data_size,
        .Encoding = DXC_CP_ACP
    };

    m_compiler->Compile(
        &source,
        arguments_wchar_ptr.data(),
        arguments_wchar_ptr.size(),
        include_handler.Get(),
        IID_PPV_ARGS(&result_dxil));

    ComPtr<IDxcBlobUtf8> dxil_errors = nullptr;
    result_dxil->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxil_errors), nullptr);
    if (dxil_errors != nullptr && dxil_errors->GetStringLength() != 0)
    {
        wprintf(L"DXC Warnings and Errors:\n%S\n", dxil_errors->GetStringPointer());
    }
    HRESULT dxil_compile_status = S_OK;
    result_dxil->GetStatus(&dxil_compile_status);
    if (FAILED(dxil_compile_status))
    {
        printf("Failed to compile shader (dxil).\n");
        return Shader();
    }

    ComPtr<IDxcBlob> dxil_blob = nullptr;
    result_dxil->GetOutput(
        DXC_OUT_OBJECT,
        IID_PPV_ARGS(dxil_blob.GetAddressOf()),
        nullptr);

    ComPtr<IDxcBlob> reflection = nullptr;
    result_dxil->GetOutput(
        DXC_OUT_REFLECTION,
        IID_PPV_ARGS(&reflection),
        nullptr);
    if (reflection != nullptr)
    {
        DxcBuffer reflection_data = {
            .Ptr = reflection->GetBufferPointer(),
            .Size = reflection->GetBufferSize(),
            .Encoding = DXC_CP_ACP
        };
        ComPtr<ID3D12ShaderReflection> d3d12_reflection = nullptr;
        m_utils->CreateReflection(&reflection_data, IID_PPV_ARGS(&d3d12_reflection));

        result.reflection = {
            .workgroups_x = 0,
            .workgroups_y = 0,
            .workgroups_z = 0
        };
        d3d12_reflection->GetThreadGroupSize(
            &result.reflection.workgroups_x,
            &result.reflection.workgroups_y,
            &result.reflection.workgroups_z);
    }
    else
    {
        printf("Warning: Shader reflection failed.\n");
    }

    const auto spirv_args = get_spirv_args();
    for (const auto& arg : spirv_args)
    {
        arguments_wchar_ptr.push_back(arg.c_str());
    }
    m_compiler->Compile(
        &source,
        arguments_wchar_ptr.data(),
        arguments_wchar_ptr.size(),
        nullptr,
        IID_PPV_ARGS(&result_spirv));

    ComPtr<IDxcBlobUtf8> spirv_errors = nullptr;
    result_dxil->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&spirv_errors), nullptr);
    if (spirv_errors != nullptr && spirv_errors->GetStringLength() != 0)
    {
        wprintf(L"Warnings and Errors:\n%S\n", spirv_errors->GetStringPointer());
    }
    HRESULT spirv_compile_status = S_OK;
    result_spirv->GetStatus(&dxil_compile_status);
    if (FAILED(spirv_compile_status))
    {
        printf("Failed to compile shader (spirv).\n");
        return Shader();
    }

    ComPtr<IDxcBlob> spirv_blob = {};
    result_spirv->GetOutput(
        DXC_OUT_OBJECT,
        IID_PPV_ARGS(spirv_blob.GetAddressOf()),
        nullptr);

    result.dxil.resize(dxil_blob->GetBufferSize());
    memcpy(result.dxil.data(), dxil_blob->GetBufferPointer(), dxil_blob->GetBufferSize());
    result.spirv.resize(spirv_blob->GetBufferSize());
    memcpy(result.spirv.data(), spirv_blob->GetBufferPointer(), spirv_blob->GetBufferSize());
    return result;
}

struct Serialized_Shader_Blob_Sizes
{
    uint32_t dxil_size;
    uint32_t spirv_size;
};

std::vector<uint8_t> Shader::serialize()
{
    std::vector<uint8_t> result{};
    result.reserve(sizeof(Shader_Reflection_Data) + sizeof(Serialized_Shader_Blob_Sizes) + dxil.size() + spirv.size());

    Serialized_Shader_Blob_Sizes blob_sizes = {
        .dxil_size = uint32_t(dxil.size()),
        .spirv_size = uint32_t(spirv.size())
    };

    auto dst = result.data();
    memcpy(dst, &reflection, sizeof(Shader_Reflection_Data));
    dst += sizeof(Shader_Reflection_Data);
    memcpy(dst, &blob_sizes, sizeof(Serialized_Shader_Blob_Sizes));
    dst += sizeof(Serialized_Shader_Blob_Sizes);
    memcpy(dst, dxil.data(), dxil.size());
    dst += dxil.size();
    memcpy(dst, spirv.data(), spirv.size());

    return result;
}

}
