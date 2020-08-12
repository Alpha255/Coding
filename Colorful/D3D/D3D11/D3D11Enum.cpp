#include "Colorful/D3D/D3D11/D3D11Enum.h"

D3D11_FILL_MODE D3D11Enum::toPolygonMode(eRPolygonMode mode)
{
	switch (mode)
	{
	case eSolid:     return D3D11_FILL_SOLID;
	case eWireframe: return D3D11_FILL_WIREFRAME;
	case ePoint:     return D3D11_FILL_WIREFRAME;  /// ???
	}
	return (D3D11_FILL_MODE)-1;
}

D3D11_CULL_MODE D3D11Enum::toCullMode(eRCullMode mode)
{
	switch (mode)
	{
	case eCullNone:      return D3D11_CULL_NONE;
	case eCullFrontFace: return D3D11_CULL_FRONT;
	case eCullBackFace:  return D3D11_CULL_BACK;
	}
	return (D3D11_CULL_MODE)-1;
}

D3D11_BLEND_OP D3D11Enum::toBlendOp(eRBlendOp op)
{
	switch (op)
	{
	case eRBlendOp::eAdd:             return D3D11_BLEND_OP_ADD;
	case eRBlendOp::eSubtract:        return D3D11_BLEND_OP_SUBTRACT;
	case eRBlendOp::eReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
	case eRBlendOp::eMin:             return D3D11_BLEND_OP_MIN;
	case eRBlendOp::eMax:             return D3D11_BLEND_OP_MAX;
	}
	return (D3D11_BLEND_OP)-1;
}

D3D11_BLEND D3D11Enum::toBlendFactor(eRBlendFactor factor)
{
	switch (factor)
	{
	case eRBlendFactor::eZero:             return D3D11_BLEND_ZERO;
	case eRBlendFactor::eOne:              return D3D11_BLEND_ONE;
	case eRBlendFactor::eConstant:         return D3D11_BLEND_BLEND_FACTOR;
	case eRBlendFactor::eInverseConstant:  return D3D11_BLEND_INV_BLEND_FACTOR;
	case eRBlendFactor::eSrcAlpha:         return D3D11_BLEND_SRC_ALPHA;
	case eRBlendFactor::eInverseSrcAlpha:  return D3D11_BLEND_INV_SRC_ALPHA;
	case eRBlendFactor::eDstAlpha:         return D3D11_BLEND_DEST_ALPHA;
	case eRBlendFactor::eInverseDstAlpha:  return D3D11_BLEND_INV_DEST_ALPHA;
	case eRBlendFactor::eSrcAlphaSaturate: return D3D11_BLEND_SRC_ALPHA_SAT;
	case eRBlendFactor::eSrc1Alpha:        return D3D11_BLEND_SRC1_ALPHA;
	case eRBlendFactor::eInverseSrc1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA;
	case eRBlendFactor::eSrcColor:         return D3D11_BLEND_SRC_COLOR;
	case eRBlendFactor::eInverseSrcColor:  return D3D11_BLEND_INV_SRC_COLOR;
	case eRBlendFactor::eDstColor:         return D3D11_BLEND_DEST_COLOR;
	case eRBlendFactor::eInverseDstColor:  return D3D11_BLEND_INV_DEST_COLOR;
	case eRBlendFactor::eSrc1Color:        return D3D11_BLEND_SRC1_COLOR;
	case eRBlendFactor::eInverseSrc1Color: return D3D11_BLEND_INV_SRC1_COLOR;
	}
	return (D3D11_BLEND)-1;
}

uint8_t D3D11Enum::toColorComponentFlags(uint32_t colorMask)
{
	if (colorMask == eColorNone)
	{
		return 0u;
	}

	if (colorMask == eColorAll)
	{
		return D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	uint8_t flags = 0u;
	if (colorMask & eColorRed)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_RED;
	}
	if (colorMask & eColorGreen)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	}
	if (colorMask & eColorBlue)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	}
	if (colorMask & eColorAlpha)
	{
		flags |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
	return flags;
}

D3D11_COMPARISON_FUNC D3D11Enum::toCompareOp(eRCompareOp op)
{
	switch (op)
	{
	case eRCompareOp::eNever:          return D3D11_COMPARISON_NEVER;
	case eRCompareOp::eLess:           return D3D11_COMPARISON_LESS;
	case eRCompareOp::eEqual:          return D3D11_COMPARISON_EQUAL;
	case eRCompareOp::eLessOrEqual:    return D3D11_COMPARISON_LESS_EQUAL;
	case eRCompareOp::eGreater:        return D3D11_COMPARISON_GREATER;
	case eRCompareOp::eNotEqual:       return D3D11_COMPARISON_NOT_EQUAL;
	case eRCompareOp::eGreaterOrEqual: return D3D11_COMPARISON_GREATER_EQUAL;
	case eRCompareOp::eAlways:         return D3D11_COMPARISON_ALWAYS;
	}
	return (D3D11_COMPARISON_FUNC)-1;
}

D3D11_STENCIL_OP D3D11Enum::toStencilOp(eRStencilOp op)
{
	switch (op)
	{
	case eRStencilOp::eKeep:              return D3D11_STENCIL_OP_KEEP;
	case eRStencilOp::eZero:              return D3D11_STENCIL_OP_ZERO;
	case eRStencilOp::eReplace:           return D3D11_STENCIL_OP_REPLACE;
	case eRStencilOp::eIncrementAndClamp: return D3D11_STENCIL_OP_INCR_SAT;
	case eRStencilOp::eDecrementAndClamp: return D3D11_STENCIL_OP_DECR_SAT;
	case eRStencilOp::eInvert:            return D3D11_STENCIL_OP_INVERT;
	case eRStencilOp::eIncrementAndWrap:  return D3D11_STENCIL_OP_INCR;
	case eRStencilOp::eDecrementAndWrap:  return D3D11_STENCIL_OP_DECR;
	}
	return (D3D11_STENCIL_OP)-1;
}

DXGI_FORMAT D3D11Enum::toFormat(eRFormat format)
{
	switch (format)
	{
	case eUnknownFormat:            return DXGI_FORMAT_UNKNOWN;
	case eD16_UNorm:                return DXGI_FORMAT_D16_UNORM;
	case eD32_Float:                return DXGI_FORMAT_D32_FLOAT;
	case eD32_Float_S8X24_Uint:     return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case eD24_UNorm_S8_UInt:        return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case eRGBA32_Typeless:          return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case eRGBA32_Float:             return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case eRGBA32_UInt:              return DXGI_FORMAT_R32G32B32A32_UINT;
	case eRGBA32_SInt:              return DXGI_FORMAT_R32G32B32A32_SINT;
	case eRGB32_Typeless:           return DXGI_FORMAT_R32G32B32_TYPELESS;
	case eRGB32_Float:              return DXGI_FORMAT_R32G32B32_FLOAT;
	case eRGB32_UInt:               return DXGI_FORMAT_R32G32B32_UINT;
	case eRGB32_SInt:               return DXGI_FORMAT_R32G32B32_SINT;
	case eRGBA16_Typeless:          return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case eRGBA16_Float:             return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case eRGBA16_UNorm:             return DXGI_FORMAT_R16G16B16A16_UNORM;
	case eRGBA16_UInt:              return DXGI_FORMAT_R16G16B16A16_UINT;
	case eRGBA16_SNorm:             return DXGI_FORMAT_R16G16B16A16_SNORM;
	case eRGBA16_SInt:              return DXGI_FORMAT_R16G16B16A16_SINT;
	case eRG32_Typeless:            return DXGI_FORMAT_R32G32_TYPELESS;
	case eRG32_Float:               return DXGI_FORMAT_R32G32_FLOAT;
	case eRG32_UInt:                return DXGI_FORMAT_R32G32_UINT;
	case eRG32_SInt:                return DXGI_FORMAT_R32G32_SINT;
	case eR32G8X24_Typeless:        return DXGI_FORMAT_R32G8X24_TYPELESS;
	case eR32_Float_X8X24_Typeless: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case eX32_Typeless_G8X24_UInt:  return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case eRGB10A2_Typeless:         return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case eRGB10A2_UNorm:            return DXGI_FORMAT_R10G10B10A2_UNORM;
	case eRGB10A2_UInt:             return DXGI_FORMAT_R10G10B10A2_UINT;
	case eRG11B10_Float:            return DXGI_FORMAT_R11G11B10_FLOAT;
	case eRGBA8_Typeless:           return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case eRGBA8_UNorm:              return DXGI_FORMAT_R8G8B8A8_UNORM;
	case eRGBA8_UNorm_SRGB:         return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case eRGBA8_UInt:               return DXGI_FORMAT_R8G8B8A8_UINT;
	case eRGBA8_SNorm:              return DXGI_FORMAT_R8G8B8A8_SNORM;
	case eRGBA8_SInt:               return DXGI_FORMAT_R8G8B8A8_SINT;
	case eRG16_Typeless:            return DXGI_FORMAT_R16G16_TYPELESS;
	case eRG16_Float:               return DXGI_FORMAT_R16G16_FLOAT;
	case eRG16_UNorm:               return DXGI_FORMAT_R16G16_UNORM;
	case eRG16_UInt:                return DXGI_FORMAT_R16G16_UINT;
	case eRG16_SNorm:               return DXGI_FORMAT_R16G16_SNORM;
	case eRG16_SInt:                return DXGI_FORMAT_R16G16_SINT;
	case eR32_Typeless:             return DXGI_FORMAT_R32_TYPELESS;
	case eR32_Float:                return DXGI_FORMAT_R32_FLOAT;
	case eR32_UInt:                 return DXGI_FORMAT_R32_UINT;
	case eR32_SInt:                 return DXGI_FORMAT_R32_SINT;
	case eR24G8_Typeless:           return DXGI_FORMAT_R24G8_TYPELESS;
	case eR24_UNorm_X8_Typeless:    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case eX24_Typelesss_G8_UInt:    return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	case eRG8_Typeless:             return DXGI_FORMAT_R8G8_TYPELESS;
	case eRG8_UNorm:                return DXGI_FORMAT_R8G8_UNORM;
	case eRG8_UInt:                 return DXGI_FORMAT_R8G8_UINT;
	case eRG8_SNorm:                return DXGI_FORMAT_R8G8_SNORM;
	case eRG8_SInt:                 return DXGI_FORMAT_R8G8_SINT;
	case eR16_Typeless:             return DXGI_FORMAT_R16_TYPELESS;
	case eR16_Float:                return DXGI_FORMAT_R16_FLOAT;
	case eR16_UNorm:                return DXGI_FORMAT_R16_UNORM;
	case eR16_UInt:                 return DXGI_FORMAT_R16_UINT;
	case eR16_SNorm:                return DXGI_FORMAT_R16_SNORM;
	case eR16_SInt:                 return DXGI_FORMAT_R16_SINT;
	case eR8_Typeless:              return DXGI_FORMAT_R8_TYPELESS;
	case eR8_UNorm:                 return DXGI_FORMAT_R8_UNORM;
	case eR8_UInt:                  return DXGI_FORMAT_R8_UINT;
	case eR8_SNorm:                 return DXGI_FORMAT_R8_SNORM;
	case eR8_SInt:                  return DXGI_FORMAT_R8_SINT;
	case eA8_UNorm:                 return DXGI_FORMAT_A8_UNORM;
	case eR1_UNorm:                 return DXGI_FORMAT_R1_UNORM;
	case eRGB9E5_SharedXP:          return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case eRG8_BG8_UNorm:            return DXGI_FORMAT_R8G8_B8G8_UNORM;
	case eGR8_GB8_UNorm:            return DXGI_FORMAT_G8R8_G8B8_UNORM;
	case eBC1_Typeless:             return DXGI_FORMAT_BC1_TYPELESS;
	case eBC1_UNorm:                return DXGI_FORMAT_BC1_UNORM;
	case eBC1_UNorm_SRGB:           return DXGI_FORMAT_BC1_UNORM_SRGB;
	case eBC2_Typeless:             return DXGI_FORMAT_BC2_TYPELESS;
	case eBC2_UNorm:                return DXGI_FORMAT_BC2_UNORM;
	case eBC2_UNorm_SRGB:           return DXGI_FORMAT_BC2_UNORM_SRGB;
	case eBC3_Typeless:             return DXGI_FORMAT_BC3_TYPELESS;
	case eBC3_UNorm:                return DXGI_FORMAT_BC3_UNORM;
	case eBC3_UNorm_SRGB:           return DXGI_FORMAT_BC3_UNORM_SRGB;
	case eBC4_Typeless:             return DXGI_FORMAT_BC4_TYPELESS;
	case eBC4_UNorm:                return DXGI_FORMAT_BC4_UNORM;
	case eBC4_SNorm:                return DXGI_FORMAT_BC4_SNORM;
	case eBC5_Typeless:             return DXGI_FORMAT_BC5_TYPELESS;
	case eBC5_UNorm:                return DXGI_FORMAT_BC5_UNORM;
	case eBC5_SNorm:                return DXGI_FORMAT_BC5_SNORM;
	case eB5G6R5_UNorm:             return DXGI_FORMAT_B5G6R5_UNORM;
	case eBGR5A1_UNorm:             return DXGI_FORMAT_B5G5R5A1_UNORM;
	case eBGRA8_UNorm:              return DXGI_FORMAT_B8G8R8A8_UNORM;
	case eBGRX8_UNorm:              return DXGI_FORMAT_B8G8R8X8_UNORM;
	case eRGB10_XR_Bias_A2_UNorm:   return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case eBGRA8_Typeless:           return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case eBGRA8_UNorm_SRGB:         return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case eBGRX8_Typeless:           return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case eBGRX8_UNorm_SRGB:         return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case eBC6H_Typeless:            return DXGI_FORMAT_BC6H_TYPELESS;
	case eBC6H_UF16:                return DXGI_FORMAT_BC6H_UF16;
	case eBC6H_SF16:                return DXGI_FORMAT_BC6H_SF16;
	case eBC7_Typeless:             return DXGI_FORMAT_BC7_TYPELESS;
	case eBC7_UNorm:                return DXGI_FORMAT_BC7_UNORM;
	case eBC7_UNorm_SRGB:           return DXGI_FORMAT_BC7_UNORM_SRGB;
	case eAYUV:                     return DXGI_FORMAT_AYUV;
	case eY410:                     return DXGI_FORMAT_Y410;
	case eY416:                     return DXGI_FORMAT_Y416;
	case eNV12:                     return DXGI_FORMAT_NV12;
	case eP010:                     return DXGI_FORMAT_P010;
	case eP016:                     return DXGI_FORMAT_P016;
	case e420_Opaque:               return DXGI_FORMAT_420_OPAQUE;
	case eYUY2:                     return DXGI_FORMAT_YUY2;
	case eY210:                     return DXGI_FORMAT_Y210;
	case eY216:                     return DXGI_FORMAT_Y216;
	case eNV11:                     return DXGI_FORMAT_NV11;
	case eAI44:                     return DXGI_FORMAT_AI44;
	case eIA44:                     return DXGI_FORMAT_IA44;
	case eP8:                       return DXGI_FORMAT_P8;
	case eAP8:                      return DXGI_FORMAT_A8P8;
	case eBGRA4_UNorm:              return DXGI_FORMAT_B4G4R4A4_UNORM;
	case eP208:                     return DXGI_FORMAT_P208;
	case eV208:                     return DXGI_FORMAT_V208;
	case eV408:                     return DXGI_FORMAT_V408;
	case eForce_UInt:               return DXGI_FORMAT_FORCE_UINT;
	}

	return (DXGI_FORMAT)-1;
}

D3D11_USAGE D3D11Enum::toUsage(eRBufferUsage usage)
{
	switch (usage)
	{
	case eGpuReadWrite:    return D3D11_USAGE_DEFAULT;
	case eGpuReadOnly:     return D3D11_USAGE_IMMUTABLE;
	case eGpuReadCpuWrite: return D3D11_USAGE_DYNAMIC;
	case eGpuCopyToCpu:    return D3D11_USAGE_STAGING;
	}

	return (D3D11_USAGE)-1;
}

D3D11_FILTER D3D11Enum::toFilter(eRFilter filter)
{
	switch (filter)
	{
	case eNearest:     return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case eLinear:      return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case eAnisotropic: return D3D11_FILTER_ANISOTROPIC;
	}

	return (D3D11_FILTER)-1;
}

D3D11_TEXTURE_ADDRESS_MODE D3D11Enum::toAddressMode(eRSamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case eRepeat:            return D3D11_TEXTURE_ADDRESS_WRAP;
	case eMirroredRepeat:    return D3D11_TEXTURE_ADDRESS_MIRROR;
	case eClampToEdge:       return D3D11_TEXTURE_ADDRESS_CLAMP;
	case eClampToBorder:     return D3D11_TEXTURE_ADDRESS_BORDER;
	case eMirrorClampToEdge: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	}

	return (D3D11_TEXTURE_ADDRESS_MODE)-1;
}

D3D11_PRIMITIVE_TOPOLOGY D3D11Enum::toPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
	case ePointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case eLineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case eLineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case eTriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case eTriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case eLineListAdj: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case eLineStripAdj: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case eTriangleListAdj: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case eTriangleStripAdj: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case ePatchList: 
		assert(0);
		return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	}
	return (D3D11_PRIMITIVE_TOPOLOGY)-1;
}
