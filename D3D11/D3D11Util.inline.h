#pragma once

#include "Util/Common.h"

NamespaceBegin(D3DUtil)

inline D3D11_USAGE TranslateResourceUsage()
{

}

inline D3D_PRIMITIVE_TOPOLOGY TranslateTopology(eRPrimitiveTopology rTopology)
{
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (rTopology)
	{
		case ePointList              : topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
		case eLineList               : topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
		case eLineStrip              : topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
		case eTriangleList           : topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
		case eTriangleStrip          : topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
		case eLineListAdj            : topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ; break;
		case eLineStripAdj           : topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ; break;
		case eTriangleListAdj        : topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ; break;
		case eTriangleStripAdj       : topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ; break;
		case eControlPointPatchList3 : topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST; break;
		default                      : topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; assert(0); break;
	}

	return topology;
}

inline DXGI_FORMAT TranslateFormat(eRFormat rFormat)
{
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	switch (rFormat)
	{
		case eUnknownFormat            : format = DXGI_FORMAT_UNKNOWN; break;
		case eRGBA32_Typeless          : format = DXGI_FORMAT_R32G32B32A32_TYPELESS; break;
		case eRGBA32_Float             : format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		case eRGBA32_UInt              : format = DXGI_FORMAT_R32G32B32A32_UINT; break;
		case eRGBA32_SInt              : format = DXGI_FORMAT_R32G32B32A32_SINT; break;
		case eRGB32_Typeless           : format = DXGI_FORMAT_R32G32B32_TYPELESS; break;
		case eRGB32_Float              : format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case eRGB32_UInt               : format = DXGI_FORMAT_R32G32B32_UINT; break;
		case eRGB32_SInt               : format = DXGI_FORMAT_R32G32B32_SINT; break;
		case eRGBA16_Typeless          : format = DXGI_FORMAT_R16G16B16A16_TYPELESS; break;
		case eRGBA16_Float             : format = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
		case eRGBA16_UNorm             : format = DXGI_FORMAT_R16G16B16A16_UNORM; break;
		case eRGBA16_UInt              : format = DXGI_FORMAT_R16G16B16A16_UINT; break;
		case eRGBA16_SNorm             : format = DXGI_FORMAT_R16G16B16A16_SNORM; break;
		case eRGBA16_SInt              : format = DXGI_FORMAT_R16G16B16A16_SINT; break;
		case eRG32_Typeless            : format = DXGI_FORMAT_R32G32_TYPELESS; break;
		case eRG32_Float               : format = DXGI_FORMAT_R32G32_FLOAT; break;
		case eRG32_UInt                : format = DXGI_FORMAT_R32G32_UINT; break;
		case eRG32_SInt                : format = DXGI_FORMAT_R32G32_SINT; break;
		case eR32G8X24_Typeless        : format = DXGI_FORMAT_R32G8X24_TYPELESS; break;
		case eD32_Float_S8X24_Uint     : format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT; break;
		case eR32_Float_X8X24_Typeless : format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; break;
		case eX32_Typeless_G8X24_UInt  : format = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT; break;
		case eRGB10A2_Typeless         : format = DXGI_FORMAT_R10G10B10A2_TYPELESS; break;
		case eRGB10A2_UNorm            : format = DXGI_FORMAT_R10G10B10A2_UNORM; break;
		case eRGB10A2_UInt             : format = DXGI_FORMAT_R10G10B10A2_UINT; break;
		case eRG11B10_Float            : format = DXGI_FORMAT_R11G11B10_FLOAT; break;
		case eRGBA8_Typeless           : format = DXGI_FORMAT_R8G8B8A8_TYPELESS; break;
		case eRGBA8_UNorm              : format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		case eRGBA8_UNorm_SRGB         : format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
		case eRGBA8_UInt               : format = DXGI_FORMAT_R8G8B8A8_UINT; break;
		case eRGBA8_SNorm              : format = DXGI_FORMAT_R8G8B8A8_SNORM; break;
		case eRGBA8_SInt               : format = DXGI_FORMAT_R8G8B8A8_SINT; break;
		case eRG16_Typeless            : format = DXGI_FORMAT_R16G16_TYPELESS; break;
		case eRG16_Float               : format = DXGI_FORMAT_R16G16_FLOAT; break;
		case eRG16_UNorm               : format = DXGI_FORMAT_R16G16_UNORM; break;
		case eRG16_UInt                : format = DXGI_FORMAT_R16G16_UINT; break;
		case eRG16_SNorm               : format = DXGI_FORMAT_R16G16_SNORM; break;
		case eRG16_SInt                : format = DXGI_FORMAT_R16G16_SINT; break;
		case eR32_Typeless             : format = DXGI_FORMAT_R32_TYPELESS; break;
		case eD32_Float                : format = DXGI_FORMAT_D32_FLOAT; break;
		case eR32_Float                : format = DXGI_FORMAT_R32_FLOAT; break;
		case eR32_UInt                 : format = DXGI_FORMAT_R32_UINT; break;
		case eR32_SInt                 : format = DXGI_FORMAT_R32_SINT; break;
		case eR24G8_Typeless           : format = DXGI_FORMAT_R24G8_TYPELESS; break;
		case eD24_UNorm_S8_UInt        : format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
		case eR24_UNorm_X8_Typeless    : format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
		case eX24_Typelesss_G8_UInt    : format = DXGI_FORMAT_X24_TYPELESS_G8_UINT; break;
		case eRG8_Typeless             : format = DXGI_FORMAT_R8G8_TYPELESS; break;
		case eRG8_UNorm                : format = DXGI_FORMAT_R8G8_UNORM; break;
		case eRG8_UInt                 : format = DXGI_FORMAT_R8G8_UINT; break;
		case eRG8_SNorm                : format = DXGI_FORMAT_R8G8_SNORM; break;
		case eRG8_SInt                 : format = DXGI_FORMAT_R8G8_SINT; break;
		case eR16_Typeless             : format = DXGI_FORMAT_R16_TYPELESS; break;
		case eR16_Float                : format = DXGI_FORMAT_R16_FLOAT; break;
		case eD16_UNorm                : format = DXGI_FORMAT_D16_UNORM; break;
		case eR16_UNorm                : format = DXGI_FORMAT_R16_UNORM; break;
		case eR16_UInt                 : format = DXGI_FORMAT_R16_UINT; break;
		case eR16_SNorm                : format = DXGI_FORMAT_R16_SNORM; break;
		case eR16_SInt                 : format = DXGI_FORMAT_R16_SINT; break;
		case eR8_Typeless              : format = DXGI_FORMAT_R8_TYPELESS; break;
		case eR8_UNorm                 : format = DXGI_FORMAT_R8_UNORM; break;
		case eR8_UInt                  : format = DXGI_FORMAT_R8_UINT; break;
		case eR8_SNorm                 : format = DXGI_FORMAT_R8_SNORM; break;
		case eR8_SInt                  : format = DXGI_FORMAT_R8_SINT; break;
		case eA8_UNorm                 : format = DXGI_FORMAT_A8_UNORM; break;
		case eR1_UNorm                 : format = DXGI_FORMAT_R1_UNORM; break;
		case eRGB9E5_SharedXP          : format = DXGI_FORMAT_R9G9B9E5_SHAREDEXP; break;
		case eRG8_BG8_UNorm            : format = DXGI_FORMAT_R8G8_B8G8_UNORM; break;
		case eGR8_GB8_UNorm            : format = DXGI_FORMAT_G8R8_G8B8_UNORM; break;
		case eBC1_Typeless             : format = DXGI_FORMAT_BC1_TYPELESS; break;
		case eBC1_UNorm                : format = DXGI_FORMAT_BC1_UNORM; break;
		case eBC1_UNorm_SRGB           : format = DXGI_FORMAT_BC1_UNORM_SRGB; break;
		case eBC2_Typeless             : format = DXGI_FORMAT_BC2_TYPELESS; break;
		case eBC2_UNorm                : format = DXGI_FORMAT_BC2_UNORM; break;
		case eBC2_UNorm_SRGB           : format = DXGI_FORMAT_BC2_UNORM_SRGB; break;
		case eBC3_Typeless             : format = DXGI_FORMAT_BC3_TYPELESS; break;
		case eBC3_UNorm                : format = DXGI_FORMAT_BC3_UNORM; break;
		case eBC3_UNorm_SRGB           : format = DXGI_FORMAT_BC3_UNORM_SRGB; break;
		case eBC4_Typeless             : format = DXGI_FORMAT_BC4_TYPELESS; break;
		case eBC4_UNorm                : format = DXGI_FORMAT_BC4_UNORM; break;
		case eBC4_SNorm                : format = DXGI_FORMAT_BC4_SNORM; break;
		case eBC5_Typeless             : format = DXGI_FORMAT_BC5_TYPELESS; break;
		case eBC5_UNorm                : format = DXGI_FORMAT_BC5_UNORM; break;
		case eBC5_SNorm                : format = DXGI_FORMAT_BC5_SNORM; break;
		case eB5G6R5_UNorm             : format = DXGI_FORMAT_B5G6R5_UNORM; break;
		case eBGR5A1_UNorm             : format = DXGI_FORMAT_B5G5R5A1_UNORM; break;
		case eBGRA8_UNorm              : format = DXGI_FORMAT_B8G8R8A8_UNORM; break;
		case eBGRX8_UNorm              : format = DXGI_FORMAT_B8G8R8X8_UNORM; break;
		case eRGB10_XR_Bias_A2_UNorm   : format = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM; break;
		case eBGRA8_Typeless           : format = DXGI_FORMAT_B8G8R8A8_TYPELESS; break;
		case eBGRA8_UNorm_SRGB         : format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; break;
		case eBGRX8_Typeless           : format = DXGI_FORMAT_B8G8R8X8_TYPELESS; break;
		case eBGRX8_UNorm_SRGB         : format = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; break;
		case eBC6H_Typeless            : format = DXGI_FORMAT_BC6H_TYPELESS; break;
		case eBC6H_UF16                : format = DXGI_FORMAT_BC6H_UF16; break;
		case eBC6H_SF16                : format = DXGI_FORMAT_BC6H_SF16; break;
		case eBC7_Typeless             : format = DXGI_FORMAT_BC7_TYPELESS; break;
		case eBC7_UNorm                : format = DXGI_FORMAT_BC7_UNORM; break;
		case eBC7_UNorm_SRGB           : format = DXGI_FORMAT_BC7_UNORM_SRGB; break;
		case eAYUV                     : format = DXGI_FORMAT_AYUV; break;
		case eY410                     : format = DXGI_FORMAT_Y410; break;
		case eY416                     : format = DXGI_FORMAT_Y416; break;
		case eNV12                     : format = DXGI_FORMAT_NV12; break;
		case eP010                     : format = DXGI_FORMAT_P010; break;
		case eP016                     : format = DXGI_FORMAT_P016; break;
		case e420_Opaque               : format = DXGI_FORMAT_420_OPAQUE; break;
		case eYUY2                     : format = DXGI_FORMAT_YUY2; break;
		case eY210                     : format = DXGI_FORMAT_Y210; break;
		case eY216                     : format = DXGI_FORMAT_Y216; break;
		case eNV11                     : format = DXGI_FORMAT_NV11; break;
		case eAI44                     : format = DXGI_FORMAT_AI44; break;
		case eIA44                     : format = DXGI_FORMAT_IA44; break;
		case eP8                       : format = DXGI_FORMAT_P8; break;
		case eAP8                      : format = DXGI_FORMAT_A8P8; break;
		case eBGRA4_UNorm              : format = DXGI_FORMAT_B4G4R4A4_UNORM; break;
		case eForce_UInt               : format = DXGI_FORMAT_FORCE_UINT; break;
		default                        : format = DXGI_FORMAT_UNKNOWN; assert(0); break;
	}

	return format;
}

NamespaceEnd(D3DUtil)
