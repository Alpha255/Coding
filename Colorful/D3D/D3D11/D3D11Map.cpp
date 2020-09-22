#include "Colorful/D3D/D3D11/D3D11Map.h"

NAMESPACE_START(Gfx)

D3D11_FILL_MODE D3D11Map::polygonMode(EPolygonMode mode)
{
	switch (mode)
	{
	case EPolygonMode::Solid:     return D3D11_FILL_SOLID;
	case EPolygonMode::Wireframe: return D3D11_FILL_WIREFRAME;
	case EPolygonMode::Point:     return D3D11_FILL_WIREFRAME;  /// ???
	}

	assert(0);
	return D3D11_FILL_SOLID;
}

D3D11_CULL_MODE D3D11Map::cullMode(ECullMode mode)
{
	switch (mode)
	{
	case ECullMode::None:      return D3D11_CULL_NONE;
	case ECullMode::FrontFace: return D3D11_CULL_FRONT;
	case ECullMode::BackFace:  return D3D11_CULL_BACK;
	}

	assert(0);
	return D3D11_CULL_NONE;
}

D3D11_BLEND_OP D3D11Map::blendOp(EBlendOp op)
{
	switch (op)
	{
	case EBlendOp::Add:             return D3D11_BLEND_OP_ADD;
	case EBlendOp::Subtract:        return D3D11_BLEND_OP_SUBTRACT;
	case EBlendOp::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
	case EBlendOp::Min:             return D3D11_BLEND_OP_MIN;
	case EBlendOp::Max:             return D3D11_BLEND_OP_MAX;
	}

	assert(0);
	return D3D11_BLEND_OP_ADD;
}

D3D11_BLEND D3D11Map::blendFactor(EBlendFactor factor)
{
	switch (factor)
	{
	case EBlendFactor::Zero:             return D3D11_BLEND_ZERO;
	case EBlendFactor::One:              return D3D11_BLEND_ONE;
	case EBlendFactor::Constant:         return D3D11_BLEND_BLEND_FACTOR;
	case EBlendFactor::InverseConstant:  return D3D11_BLEND_INV_BLEND_FACTOR;
	case EBlendFactor::SrcAlpha:         return D3D11_BLEND_SRC_ALPHA;
	case EBlendFactor::InverseSrcAlpha:  return D3D11_BLEND_INV_SRC_ALPHA;
	case EBlendFactor::DstAlpha:         return D3D11_BLEND_DEST_ALPHA;
	case EBlendFactor::InverseDstAlpha:  return D3D11_BLEND_INV_DEST_ALPHA;
	case EBlendFactor::SrcAlphaSaturate: return D3D11_BLEND_SRC_ALPHA_SAT;
	case EBlendFactor::Src1Alpha:        return D3D11_BLEND_SRC1_ALPHA;
	case EBlendFactor::InverseSrc1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA;
	case EBlendFactor::SrcColor:         return D3D11_BLEND_SRC_COLOR;
	case EBlendFactor::InverseSrcColor:  return D3D11_BLEND_INV_SRC_COLOR;
	case EBlendFactor::DstColor:         return D3D11_BLEND_DEST_COLOR;
	case EBlendFactor::InverseDstColor:  return D3D11_BLEND_INV_DEST_COLOR;
	case EBlendFactor::Src1Color:        return D3D11_BLEND_SRC1_COLOR;
	case EBlendFactor::InverseSrc1Color: return D3D11_BLEND_INV_SRC1_COLOR;
	}

	assert(0);
	return D3D11_BLEND_ZERO;
}

//uint8_t D3D11Map::colorComponentFlags(uint32_t colorMask)
//{
//	if (colorMask == eColorNone)
//	{
//		return 0u;
//	}
//
//	if (colorMask == eColorAll)
//	{
//		return D3D11_COLOR_WRITE_ENABLE_ALL;
//	}
//
//	uint8_t flags = 0u;
//	if (colorMask & eColorRed)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_RED;
//	}
//	if (colorMask & eColorGreen)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_GREEN;
//	}
//	if (colorMask & eColorBlue)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_BLUE;
//	}
//	if (colorMask & eColorAlpha)
//	{
//		flags |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
//	}
//	return flags;
//}

D3D11_COMPARISON_FUNC D3D11Map::compareFunc(ECompareFunc func)
{
	switch (func)
	{
	case ECompareFunc::Never:          return D3D11_COMPARISON_NEVER;
	case ECompareFunc::Less:           return D3D11_COMPARISON_LESS;
	case ECompareFunc::Equal:          return D3D11_COMPARISON_EQUAL;
	case ECompareFunc::LessOrEqual:    return D3D11_COMPARISON_LESS_EQUAL;
	case ECompareFunc::Greater:        return D3D11_COMPARISON_GREATER;
	case ECompareFunc::NotEqual:       return D3D11_COMPARISON_NOT_EQUAL;
	case ECompareFunc::GreaterOrEqual: return D3D11_COMPARISON_GREATER_EQUAL;
	case ECompareFunc::Always:         return D3D11_COMPARISON_ALWAYS;
	}

	assert(0);
	return D3D11_COMPARISON_ALWAYS;
}

D3D11_STENCIL_OP D3D11Map::stencilOp(EStencilOp op)
{
	switch (op)
	{
	case EStencilOp::Keep:              return D3D11_STENCIL_OP_KEEP;
	case EStencilOp::Zero:              return D3D11_STENCIL_OP_ZERO;
	case EStencilOp::Replace:           return D3D11_STENCIL_OP_REPLACE;
	case EStencilOp::IncrementAndClamp: return D3D11_STENCIL_OP_INCR_SAT;
	case EStencilOp::DecrementAndClamp: return D3D11_STENCIL_OP_DECR_SAT;
	case EStencilOp::Invert:            return D3D11_STENCIL_OP_INVERT;
	case EStencilOp::IncrementAndWrap:  return D3D11_STENCIL_OP_INCR;
	case EStencilOp::DecrementAndWrap:  return D3D11_STENCIL_OP_DECR;
	}

	assert(0);
	return D3D11_STENCIL_OP_KEEP;
}

DXGI_FORMAT D3D11Map::format(EFormat format)
{
	switch (format)
	{
	case EFormat::Unknown:                  return DXGI_FORMAT_UNKNOWN;
	case EFormat::D16_UNorm:                return DXGI_FORMAT_D16_UNORM;
	case EFormat::D32_Float:                return DXGI_FORMAT_D32_FLOAT;
	case EFormat::D32_Float_S8X24_Uint:     return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case EFormat::D24_UNorm_S8_UInt:        return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case EFormat::RGBA32_Typeless:          return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case EFormat::RGBA32_Float:             return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case EFormat::RGBA32_UInt:              return DXGI_FORMAT_R32G32B32A32_UINT;
	case EFormat::RGBA32_SInt:              return DXGI_FORMAT_R32G32B32A32_SINT;
	case EFormat::RGB32_Typeless:           return DXGI_FORMAT_R32G32B32_TYPELESS;
	case EFormat::RGB32_Float:              return DXGI_FORMAT_R32G32B32_FLOAT;
	case EFormat::RGB32_UInt:               return DXGI_FORMAT_R32G32B32_UINT;
	case EFormat::RGB32_SInt:               return DXGI_FORMAT_R32G32B32_SINT;
	case EFormat::RGBA16_Typeless:          return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case EFormat::RGBA16_Float:             return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case EFormat::RGBA16_UNorm:             return DXGI_FORMAT_R16G16B16A16_UNORM;
	case EFormat::RGBA16_UInt:              return DXGI_FORMAT_R16G16B16A16_UINT;
	case EFormat::RGBA16_SNorm:             return DXGI_FORMAT_R16G16B16A16_SNORM;
	case EFormat::RGBA16_SInt:              return DXGI_FORMAT_R16G16B16A16_SINT;
	case EFormat::RG32_Typeless:            return DXGI_FORMAT_R32G32_TYPELESS;
	case EFormat::RG32_Float:               return DXGI_FORMAT_R32G32_FLOAT;
	case EFormat::RG32_UInt:                return DXGI_FORMAT_R32G32_UINT;
	case EFormat::RG32_SInt:                return DXGI_FORMAT_R32G32_SINT;
	case EFormat::R32G8X24_Typeless:        return DXGI_FORMAT_R32G8X24_TYPELESS;
	case EFormat::R32_Float_X8X24_Typeless: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case EFormat::X32_Typeless_G8X24_UInt:  return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case EFormat::RGB10A2_Typeless:         return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case EFormat::RGB10A2_UNorm:            return DXGI_FORMAT_R10G10B10A2_UNORM;
	case EFormat::RGB10A2_UInt:             return DXGI_FORMAT_R10G10B10A2_UINT;
	case EFormat::RG11B10_Float:            return DXGI_FORMAT_R11G11B10_FLOAT;
	case EFormat::RGBA8_Typeless:           return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case EFormat::RGBA8_UNorm:              return DXGI_FORMAT_R8G8B8A8_UNORM;
	case EFormat::RGBA8_UNorm_SRGB:         return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case EFormat::RGBA8_UInt:               return DXGI_FORMAT_R8G8B8A8_UINT;
	case EFormat::RGBA8_SNorm:              return DXGI_FORMAT_R8G8B8A8_SNORM;
	case EFormat::RGBA8_SInt:               return DXGI_FORMAT_R8G8B8A8_SINT;
	case EFormat::RG16_Typeless:            return DXGI_FORMAT_R16G16_TYPELESS;
	case EFormat::RG16_Float:               return DXGI_FORMAT_R16G16_FLOAT;
	case EFormat::RG16_UNorm:               return DXGI_FORMAT_R16G16_UNORM;
	case EFormat::RG16_UInt:                return DXGI_FORMAT_R16G16_UINT;
	case EFormat::RG16_SNorm:               return DXGI_FORMAT_R16G16_SNORM;
	case EFormat::RG16_SInt:                return DXGI_FORMAT_R16G16_SINT;
	case EFormat::R32_Typeless:             return DXGI_FORMAT_R32_TYPELESS;
	case EFormat::R32_Float:                return DXGI_FORMAT_R32_FLOAT;
	case EFormat::R32_UInt:                 return DXGI_FORMAT_R32_UINT;
	case EFormat::R32_SInt:                 return DXGI_FORMAT_R32_SINT;
	case EFormat::R24G8_Typeless:           return DXGI_FORMAT_R24G8_TYPELESS;
	case EFormat::R24_UNorm_X8_Typeless:    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case EFormat::X24_Typelesss_G8_UInt:    return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	case EFormat::RG8_Typeless:             return DXGI_FORMAT_R8G8_TYPELESS;
	case EFormat::RG8_UNorm:                return DXGI_FORMAT_R8G8_UNORM;
	case EFormat::RG8_UInt:                 return DXGI_FORMAT_R8G8_UINT;
	case EFormat::RG8_SNorm:                return DXGI_FORMAT_R8G8_SNORM;
	case EFormat::RG8_SInt:                 return DXGI_FORMAT_R8G8_SINT;
	case EFormat::R16_Typeless:             return DXGI_FORMAT_R16_TYPELESS;
	case EFormat::R16_Float:                return DXGI_FORMAT_R16_FLOAT;
	case EFormat::R16_UNorm:                return DXGI_FORMAT_R16_UNORM;
	case EFormat::R16_UInt:                 return DXGI_FORMAT_R16_UINT;
	case EFormat::R16_SNorm:                return DXGI_FORMAT_R16_SNORM;
	case EFormat::R16_SInt:                 return DXGI_FORMAT_R16_SINT;
	case EFormat::R8_Typeless:              return DXGI_FORMAT_R8_TYPELESS;
	case EFormat::R8_UNorm:                 return DXGI_FORMAT_R8_UNORM;
	case EFormat::R8_UInt:                  return DXGI_FORMAT_R8_UINT;
	case EFormat::R8_SNorm:                 return DXGI_FORMAT_R8_SNORM;
	case EFormat::R8_SInt:                  return DXGI_FORMAT_R8_SINT;
	case EFormat::A8_UNorm:                 return DXGI_FORMAT_A8_UNORM;
	case EFormat::R1_UNorm:                 return DXGI_FORMAT_R1_UNORM;
	case EFormat::RGB9E5_SharedXP:          return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case EFormat::RG8_BG8_UNorm:            return DXGI_FORMAT_R8G8_B8G8_UNORM;
	case EFormat::GR8_GB8_UNorm:            return DXGI_FORMAT_G8R8_G8B8_UNORM;
	case EFormat::BC1_Typeless:             return DXGI_FORMAT_BC1_TYPELESS;
	case EFormat::BC1_UNorm:                return DXGI_FORMAT_BC1_UNORM;
	case EFormat::BC1_UNorm_SRGB:           return DXGI_FORMAT_BC1_UNORM_SRGB;
	case EFormat::BC2_Typeless:             return DXGI_FORMAT_BC2_TYPELESS;
	case EFormat::BC2_UNorm:                return DXGI_FORMAT_BC2_UNORM;
	case EFormat::BC2_UNorm_SRGB:           return DXGI_FORMAT_BC2_UNORM_SRGB;
	case EFormat::BC3_Typeless:             return DXGI_FORMAT_BC3_TYPELESS;
	case EFormat::BC3_UNorm:                return DXGI_FORMAT_BC3_UNORM;
	case EFormat::BC3_UNorm_SRGB:           return DXGI_FORMAT_BC3_UNORM_SRGB;
	case EFormat::BC4_Typeless:             return DXGI_FORMAT_BC4_TYPELESS;
	case EFormat::BC4_UNorm:                return DXGI_FORMAT_BC4_UNORM;
	case EFormat::BC4_SNorm:                return DXGI_FORMAT_BC4_SNORM;
	case EFormat::BC5_Typeless:             return DXGI_FORMAT_BC5_TYPELESS;
	case EFormat::BC5_UNorm:                return DXGI_FORMAT_BC5_UNORM;
	case EFormat::BC5_SNorm:                return DXGI_FORMAT_BC5_SNORM;
	case EFormat::B5G6R5_UNorm:             return DXGI_FORMAT_B5G6R5_UNORM;
	case EFormat::BGR5A1_UNorm:             return DXGI_FORMAT_B5G5R5A1_UNORM;
	case EFormat::BGRA8_UNorm:              return DXGI_FORMAT_B8G8R8A8_UNORM;
	case EFormat::BGRX8_UNorm:              return DXGI_FORMAT_B8G8R8X8_UNORM;
	case EFormat::RGB10_XR_Bias_A2_UNorm:   return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case EFormat::BGRA8_Typeless:           return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case EFormat::BGRA8_UNorm_SRGB:         return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case EFormat::BGRX8_Typeless:           return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case EFormat::BGRX8_UNorm_SRGB:         return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case EFormat::BC6H_Typeless:            return DXGI_FORMAT_BC6H_TYPELESS;
	case EFormat::BC6H_UF16:                return DXGI_FORMAT_BC6H_UF16;
	case EFormat::BC6H_SF16:                return DXGI_FORMAT_BC6H_SF16;
	case EFormat::BC7_Typeless:             return DXGI_FORMAT_BC7_TYPELESS;
	case EFormat::BC7_UNorm:                return DXGI_FORMAT_BC7_UNORM;
	case EFormat::BC7_UNorm_SRGB:           return DXGI_FORMAT_BC7_UNORM_SRGB;
	case EFormat::AYUV:                     return DXGI_FORMAT_AYUV;
	case EFormat::Y410:                     return DXGI_FORMAT_Y410;
	case EFormat::Y416:                     return DXGI_FORMAT_Y416;
	case EFormat::NV12:                     return DXGI_FORMAT_NV12;
	case EFormat::P010:                     return DXGI_FORMAT_P010;
	case EFormat::P016:                     return DXGI_FORMAT_P016;
	case EFormat::Opaque_420:               return DXGI_FORMAT_420_OPAQUE;
	case EFormat::YUY2:                     return DXGI_FORMAT_YUY2;
	case EFormat::Y210:                     return DXGI_FORMAT_Y210;
	case EFormat::Y216:                     return DXGI_FORMAT_Y216;
	case EFormat::NV11:                     return DXGI_FORMAT_NV11;
	case EFormat::AI44:                     return DXGI_FORMAT_AI44;
	case EFormat::IA44:                     return DXGI_FORMAT_IA44;
	case EFormat::P8:                       return DXGI_FORMAT_P8;
	case EFormat::AP8:                      return DXGI_FORMAT_A8P8;
	case EFormat::BGRA4_UNorm:              return DXGI_FORMAT_B4G4R4A4_UNORM;
	case EFormat::P208:                     return DXGI_FORMAT_P208;
	case EFormat::V208:                     return DXGI_FORMAT_V208;
	case EFormat::V408:                     return DXGI_FORMAT_V408;
	case EFormat::Force_UInt:               return DXGI_FORMAT_FORCE_UINT;
	}

	assert(0);
	return (DXGI_FORMAT)-1;
}

//D3D11_USAGE D3D11Map::toUsage(eRBufferUsage usage)
//{
//	switch (usage)
//	{
//	case eGpuReadWrite:    return D3D11_USAGE_DEFAULT;
//	case eGpuReadOnly:     return D3D11_USAGE_IMMUTABLE;
//	case eGpuReadCpuWrite: return D3D11_USAGE_DYNAMIC;
//	case eGpuCopyToCpu:    return D3D11_USAGE_STAGING;
//	}
//
//	return (D3D11_USAGE)-1;
//}

D3D11_FILTER D3D11Map::textureFilter(ETextureFilter filter)
{
	switch (filter)
	{
	case ETextureFilter::Nearest:     return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case ETextureFilter::Linear:      return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case ETextureFilter::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
	}

	assert(0);
	return (D3D11_FILTER)-1;
}

D3D11_TEXTURE_ADDRESS_MODE D3D11Map::samplerAddressMode(ESamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case ESamplerAddressMode::Repeat:            return D3D11_TEXTURE_ADDRESS_WRAP;
	case ESamplerAddressMode::MirroredRepeat:    return D3D11_TEXTURE_ADDRESS_MIRROR;
	case ESamplerAddressMode::ClampToEdge:       return D3D11_TEXTURE_ADDRESS_CLAMP;
	case ESamplerAddressMode::ClampToBorder:     return D3D11_TEXTURE_ADDRESS_BORDER;
	case ESamplerAddressMode::MirrorClampToEdge: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	}

	assert(0);
	return (D3D11_TEXTURE_ADDRESS_MODE)-1;
}

D3D11_PRIMITIVE_TOPOLOGY D3D11Map::primitiveTopology(EPrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
	case EPrimitiveTopology::PointList:        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case EPrimitiveTopology::LineList:         return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case EPrimitiveTopology::LineStrip:        return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case EPrimitiveTopology::TriangleList:     return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case EPrimitiveTopology::TriangleStrip:    return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case EPrimitiveTopology::LineListAdj:      return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case EPrimitiveTopology::LineStripAdj:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case EPrimitiveTopology::TriangleListAdj:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case EPrimitiveTopology::TriangleStripAdj: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case EPrimitiveTopology::PatchList: 
		assert(0);
		return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	}

	assert(0);
	return (D3D11_PRIMITIVE_TOPOLOGY)-1;
}

NAMESPACE_END(Gfx)
