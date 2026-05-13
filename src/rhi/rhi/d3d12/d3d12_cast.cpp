#include "rhi/d3d12/d3d12_cast.hpp"

#include "rhi/d3d12/d3d12_resource.hpp"
#include <bit>

namespace rhi::d3d12
{
template<>
uint32_t d3d12_cast<uint32_t, Image_Component_Swizzle>(const Image_Component_Swizzle swizzle, uint32_t identity)
{
    switch (swizzle)
    {
    case rhi::Image_Component_Swizzle::Identity:
        // HACK: D3D12 does not have an identity mapping
        return D3D12_SHADER_COMPONENT_MAPPING(identity % 4);
    case rhi::Image_Component_Swizzle::Zero:
        return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0;
    case rhi::Image_Component_Swizzle::One:
        return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1;
    case rhi::Image_Component_Swizzle::R:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0;
    case rhi::Image_Component_Swizzle::G:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1;
    case rhi::Image_Component_Swizzle::B:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2;
    case rhi::Image_Component_Swizzle::A:
        return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3;
    default:
        return D3D12_SHADER_COMPONENT_MAPPING(identity % 4);
    }
}

template<>
uint32_t d3d12_cast<uint32_t, Image_Component_Swizzle>(
    const Image_Component_Swizzle r,
    const Image_Component_Swizzle g,
    const Image_Component_Swizzle b,
    const Image_Component_Swizzle a)
{
    auto r_ = d3d12_cast<uint32_t>(r, 0);
    auto g_ = d3d12_cast<uint32_t>(g, 1);
    auto b_ = d3d12_cast<uint32_t>(b, 2);
    auto a_ = d3d12_cast<uint32_t>(a, 3);
    return D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(r_, g_, b_, a_);
}

template<>
D3D12_TEXTURE_ADDRESS_MODE d3d12_cast<D3D12_TEXTURE_ADDRESS_MODE, Image_Sample_Address_Mode>(const Image_Sample_Address_Mode address_mode)
{
    switch (address_mode)
    {
    case Image_Sample_Address_Mode::Wrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case Image_Sample_Address_Mode::Mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case Image_Sample_Address_Mode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case Image_Sample_Address_Mode::Border:
        return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    default:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}

template<>
D3D12_COMPARISON_FUNC d3d12_cast<D3D12_COMPARISON_FUNC, Comparison_Func>(const Comparison_Func comparison_func)
{
    return std::bit_cast<D3D12_COMPARISON_FUNC>(comparison_func);
}

template<>
D3D12_STENCIL_OP d3d12_cast<D3D12_STENCIL_OP, Stencil_Op>(const Stencil_Op stencil_op)
{
    return std::bit_cast<D3D12_STENCIL_OP>(stencil_op);
}

template<>
D3D12_HEAP_TYPE d3d12_cast<D3D12_HEAP_TYPE, Memory_Heap_Type>(const Memory_Heap_Type heap_type)
{
    switch (heap_type)
    {
    case Memory_Heap_Type::GPU:
        return D3D12_HEAP_TYPE_DEFAULT;
    case Memory_Heap_Type::CPU_Upload:
        return D3D12_HEAP_TYPE_UPLOAD;
    case Memory_Heap_Type::CPU_Readback:
        return D3D12_HEAP_TYPE_READBACK;
    default:
        return D3D12_HEAP_TYPE_DEFAULT;
    }
}

template<>
DXGI_FORMAT d3d12_cast<DXGI_FORMAT, Image_Format>(const Image_Format image_format)
{
    return translate_format(image_format);
}

template<>
D3D12_SRV_DIMENSION d3d12_cast<D3D12_SRV_DIMENSION, Image_View_Type>(const Image_View_Type type)
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_SRV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_SRV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_SRV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
    case Image_View_Type::Texture_3D:
        return D3D12_SRV_DIMENSION_TEXTURE3D;
    case Image_View_Type::Texture_Cube:
        return D3D12_SRV_DIMENSION_TEXTURECUBE;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
    default:
        return D3D12_SRV_DIMENSION_UNKNOWN;
    }
}

template<>
D3D12_UAV_DIMENSION d3d12_cast<D3D12_UAV_DIMENSION, Image_View_Type>(const Image_View_Type type)
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_UAV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_UAV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_UAV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_UAV_DIMENSION_TEXTURE2DMSARRAY;
    case Image_View_Type::Texture_3D:
        return D3D12_UAV_DIMENSION_TEXTURE3D;
    case Image_View_Type::Texture_Cube:
        return D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    default:
        return D3D12_UAV_DIMENSION_UNKNOWN;
    }
}

template<>
D3D12_RTV_DIMENSION d3d12_cast<D3D12_RTV_DIMENSION, Image_View_Type>(const Image_View_Type type)
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_RTV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_RTV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_RTV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
    case Image_View_Type::Texture_3D:
        return D3D12_RTV_DIMENSION_TEXTURE3D;
    case Image_View_Type::Texture_Cube:
        return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    default:
        return D3D12_RTV_DIMENSION_TEXTURE1D;
    }
}

template<>
D3D12_DSV_DIMENSION d3d12_cast<D3D12_DSV_DIMENSION, Image_View_Type>(const Image_View_Type type)
{
    switch (type)
    {
    case Image_View_Type::Texture_1D:
        return D3D12_DSV_DIMENSION_TEXTURE1D;
    case Image_View_Type::Texture_1D_Array:
        return D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
    case Image_View_Type::Texture_2D:
        return D3D12_DSV_DIMENSION_TEXTURE2D;
    case Image_View_Type::Texture_2D_Array:
        return D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    case Image_View_Type::Texture_2D_MS:
        return D3D12_DSV_DIMENSION_TEXTURE2DMS;
    case Image_View_Type::Texture_2D_MS_Array:
        return D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
        // case Image_View_Type::Texture_3D:
        //     return D3D12_DSV_DIMENSION_UNKNOWN;
    case Image_View_Type::Texture_Cube:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    case Image_View_Type::Texture_Cube_Array:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    default:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    }
}

template<>
D3D12_FILTER_TYPE d3d12_cast<D3D12_FILTER_TYPE, Sampler_Filter>(const Sampler_Filter filter)
{
    if (filter == Sampler_Filter::Linear)
        return D3D12_FILTER_TYPE_LINEAR;
    return D3D12_FILTER_TYPE_POINT;
}

template<>
D3D12_FILTER_REDUCTION_TYPE d3d12_cast<D3D12_FILTER_REDUCTION_TYPE, Sampler_Reduction_Type>(const Sampler_Reduction_Type reduction_type)
{
    if (reduction_type == Sampler_Reduction_Type::Standard)
        return D3D12_FILTER_REDUCTION_TYPE_STANDARD;
    return D3D12_FILTER_REDUCTION_TYPE_COMPARISON;
}

template<>
D3D12_SHADER_BYTECODE d3d12_cast<D3D12_SHADER_BYTECODE, Shader_Blob>(Shader_Blob* blob)
{
    D3D12_SHADER_BYTECODE bytecode = {};
    if (blob)
    {
        bytecode.BytecodeLength = blob->data.size();
        bytecode.pShaderBytecode = blob->data.data();
    }
    return bytecode;
}

#define RHI_D3D12_ENCODE_ANISOTROPIC_FILTER(mip, reduction) \
    ( ( D3D12_FILTER ) ( \
    D3D12_ANISOTROPIC_FILTERING_BIT | \
    D3D12_ENCODE_BASIC_FILTER(  D3D12_FILTER_TYPE_LINEAR, \
                                D3D12_FILTER_TYPE_LINEAR, \
                                mip, \
                                reduction ) ) )

template<>
D3D12_FILL_MODE d3d12_cast<D3D12_FILL_MODE, Fill_Mode>(const Fill_Mode fill_mode)
{
    if (fill_mode == Fill_Mode::Solid)
        return D3D12_FILL_MODE_SOLID;
    return D3D12_FILL_MODE_WIREFRAME;
}

template<>
D3D12_CULL_MODE d3d12_cast<D3D12_CULL_MODE, Cull_Mode>(const Cull_Mode cull_mode)
{
    switch (cull_mode)
    {
    case rhi::Cull_Mode::None:
        return D3D12_CULL_MODE_NONE;
    case rhi::Cull_Mode::Front:
        return D3D12_CULL_MODE_FRONT;
    case rhi::Cull_Mode::Back:
        return D3D12_CULL_MODE_BACK;
    default:
        return D3D12_CULL_MODE_NONE;
    }
    return D3D12_CULL_MODE_NONE;
}

template<>
D3D12_FILTER d3d12_cast(Sampler_Filter min, Sampler_Filter mag, Sampler_Filter mip, Sampler_Reduction_Type reduction, bool aniso)
{
    if (aniso)
    {
        return RHI_D3D12_ENCODE_ANISOTROPIC_FILTER(
            d3d12_cast<D3D12_FILTER_TYPE>(mip),
            d3d12_cast<D3D12_FILTER_REDUCTION_TYPE>(reduction));
    }
    else
    {
        return D3D12_ENCODE_BASIC_FILTER(
            d3d12_cast<D3D12_FILTER_TYPE>(min),
            d3d12_cast<D3D12_FILTER_TYPE>(mag),
            d3d12_cast<D3D12_FILTER_TYPE>(mip),
            d3d12_cast<D3D12_FILTER_REDUCTION_TYPE>(reduction));
    }
}
}
