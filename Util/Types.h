#pragma once

enum eRShaderType
{
	eVertexShader,
	eHullShader,
	eDomainShader,
	eGeometryShader,
	ePixelShader,
	eComputeShader,
	eRShaderTypeCount
};

enum eRBufferUsage
{
	eGpuReadWrite,
	eGpuReadOnly,
	eGpuReadCpuWrite,
	eGpuCopyToCpu,
	eRBufferUsageCount
};

enum eRBufferCpuAccessFlags
{
	eCpuRead,
	eCpuWrite,
	eCpuReadWrite = eCpuRead | eCpuWrite,
	eRBufferCpuAccessFlagCount
};

enum eRBufferBindFlags
{
	eBindAsVertexBuffer,
	eBindAsIndexBuffer,
	eBindAsConstantBuffer,
	eBindAsShaderResource,
	eBindAsStreamOutput,
	eBindAsRenderTarget,
	eBindAsDepthStencil,
	eBindAsUnorderedAccess,
	eBindAsDecoder,
	eBindAsVideoEncoder,
	eRBufferBindFlagsCount
};

enum eRResourceMiscFlag
{
	eMiscGenerateMips,
	eMiscShared,
	eMiscCubemapE,
	eMiscDrawIndirectArgsS,
	eMiscAllowRawViews,
	eMiscBufferStructuredD,
	eMiscClamp,
	eMiscSharedKeyedMutex,
	eMiscGDICompatible,
	eMiscSharedNTHandle,
	eMiscRestrictedContentT,
	eMiscRestrictSharedResourceE,
	eMiscRestrictSharedResourceDriverR,
	eMiscGuarded,
	eMiscTilePool,
	eMiscTiled,
	eRResourceMiscFlagsCount
};

enum eRPrimitiveTopology
{
	ePointList,
	eLineList,
	eLineStrip,
	eTriangleList,
	eTriangleStrip,
	eLineListAdj,
	eLineStripAdj,
	eTriangleListAdj,
	eTriangleStripAdj,
	eControlPointPatchList3,
	eRPrimitiveTopologyCount
};

enum eRViewType
{
	eUnknownViewType,
	eBuffer,
	eTexture1D,
	eTexture2D,
	eTexture3D,
	eTextureCube,
	eTexture1DArray,
	eTexture2DArray,
	eTextureCubeArray,
	eRViewTypeCount
};

enum eRDepthStencilViewFlags
{
	eReadonlyDepth,
	eReadonlyStencil,
	eReadonlyDepthStencil = eReadonlyDepth | eReadonlyStencil,
	eRDepthStencilViewFlagsCount
};

enum eRDepthStencilClearFlag
{
	eDepthOnly,
	eStencilOnly,
	eDepthStencil = eDepthOnly | eStencilOnly,
	eRDepthStencilClearFlagCount
};

enum eRFormat
{
	eUnknownFormat,
	eRGBA32_Typeless,
	eRGBA32_Float,
	eRGBA32_UInt,
	eRGBA32_SInt,
	eRGB32_Typeless,
	eRGB32_Float,
	eRGB32_UInt,
	eRGB32_SInt,
	eRGBA16_Typeless,
	eRGBA16_Float,
	eRGBA16_UNorm,
	eRGBA16_UInt,
	eRGBA16_SNorm,
	eRGBA16_SInt,
	eRG32_Typeless,
	eRG32_Float,
	eRG32_UInt,
	eRG32_SInt,
	eR32G8X24_Typeless,
	eD32_Float_S8X24_Uint,
	eR32_Float_X8X24_Typeless,
	eX32_Typeless_G8X24_UInt,
	eRGB10A2_Typeless,
	eRGB10A2_UNorm,
	eRGB10A2_UInt,
	eRG11B10_Float,
	eRGBA8_Typeless,
	eRGBA8_UNorm,
	eRGBA8_UNorm_SRGB,
	eRGBA8_UInt,
	eRGBA8_SNorm,
	eRGBA8_SInt,
	eRG16_Typeless,
	eRG16_Float,
	eRG16_UNorm,
	eRG16_UInt,
	eRG16_SNorm,
	eRG16_SInt,
	eR32_Typeless,
	eD32_Float,
	eR32_Float,
	eR32_UInt,
	eR32_SInt,
	eR24G8_Typeless,
	eD24_UNorm_S8_UInt,
	eR24_UNorm_X8_Typeless,
	eX24_Typelesss_G8_UInt,
	eRG8_Typeless,
	eRG8_UNorm,
	eRG8_UInt,
	eRG8_SNorm,
	eRG8_SInt,
	eR16_Typeless,
	eR16_Float,
	eD16_UNorm,
	eR16_UNorm,
	eR16_UInt,
	eR16_SNorm,
	eR16_SInt,
	eR8_Typeless,
	eR8_UNorm,
	eR8_UInt,
	eR8_SNorm,
	eR8_SInt,
	eA8_UNorm,
	eR1_UNorm,
	eRGB9E5_SharedXP,
	eRG8_BG8_UNorm,
	eGR8_GB8_UNorm,
	eBC1_Typeless,
	eBC1_UNorm,
	eBC1_UNorm_SRGB,
	eBC2_Typeless,
	eBC2_UNorm,
	eBC2_UNorm_SRGB,
	eBC3_Typeless,
	eBC3_UNorm,
	eBC3_UNorm_SRGB,
	eBC4_Typeless,
	eBC4_UNorm,
	eBC4_SNorm,
	eBC5_Typeless,
	eBC5_UNorm,
	eBC5_SNorm,
	eB5G6R5_UNorm,
	eBGR5A1_UNorm,
	eBGRA8_UNorm,
	eBGRX8_UNorm,
	eRGB10_XR_Bias_A2_UNorm,
	eBGRA8_Typeless,
	eBGRA8_UNorm_SRGB,
	eBGRX8_Typeless,
	eBGRX8_UNorm_SRGB,
	eBC6H_Typeless,
	eBC6H_UF16,
	eBC6H_SF16,
	eBC7_Typeless,
	eBC7_UNorm,
	eBC7_UNorm_SRGB,
	eAYUV,
	eY410,
	eY416,
	eNV12,
	eP010,
	eP016,
	e420_Opaque,
	eYUY2,
	eY210,
	eY216,
	eNV11,
	eAI44,
	eIA44,
	eP8,
	eAP8,
	eBGRA4_UNorm,
	eForce_UInt,
	eRFormatCount
};
