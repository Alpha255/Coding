#pragma once

#include "Base/Base.h"
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#define Check(func) if (FAILED(func)) { assert(0); }

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
	eGpuReadWrite = D3D11_USAGE_DEFAULT,
	eGpuReadOnly = D3D11_USAGE_IMMUTABLE,
	eGpuReadCpuWrite = D3D11_USAGE_DYNAMIC,
	eGpuCopyToCpu = D3D11_USAGE_STAGING,
};

enum eRBufferCpuAccessFlags
{
	eCpuRead = D3D11_CPU_ACCESS_READ,
	eCpuWrite = D3D11_CPU_ACCESS_WRITE,
	eCpuReadWrite = eCpuRead | eCpuWrite
};

enum eRBufferBindFlags
{
	eBindAsVertexBuffer = D3D11_BIND_VERTEX_BUFFER,
	eBindAsIndexBuffer = D3D11_BIND_INDEX_BUFFER,
	eBindAsConstantBuffer = D3D11_BIND_CONSTANT_BUFFER,
	eBindAsShaderResource = D3D11_BIND_SHADER_RESOURCE,
	eBindAsStreamOutput = D3D11_BIND_STREAM_OUTPUT,
	eBindAsRenderTarget = D3D11_BIND_RENDER_TARGET,
	eBindAsDepthStencil = D3D11_BIND_DEPTH_STENCIL,
	eBindAsUnorderedAccess = D3D11_BIND_UNORDERED_ACCESS,
	eBindAsDecoder = D3D11_BIND_DECODER,
	eBindAsVideoEncoder = D3D11_BIND_VIDEO_ENCODER,
};

enum eRResourceMiscFlag
{
	eMiscGenerateMips = D3D11_RESOURCE_MISC_GENERATE_MIPS,
	eMiscShared = D3D11_RESOURCE_MISC_SHARED,
	eMiscCubemap = D3D11_RESOURCE_MISC_TEXTURECUBE,
	eMiscDrawIndirectArgs = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS,
	eMiscAllowRawViews = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS,
	eMiscBufferStructured = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
	eMiscClamp = D3D11_RESOURCE_MISC_RESOURCE_CLAMP,
	eMiscSharedKeyedMutex = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX,
	eMiscGDICompatible = D3D11_RESOURCE_MISC_GDI_COMPATIBLE,
	eMiscSharedNTHandle = D3D11_RESOURCE_MISC_SHARED_NTHANDLE,
	eMiscRestrictedContent = D3D11_RESOURCE_MISC_RESTRICTED_CONTENT,
	eMiscRestrictSharedResource = D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE,
	eMiscRestrictSharedResourceDriver = D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER,
	eMiscGuarded = D3D11_RESOURCE_MISC_GUARDED,
	eMiscTilePool = D3D11_RESOURCE_MISC_TILE_POOL,
	eMiscTiled = D3D11_RESOURCE_MISC_TILED
};

enum eRPrimitiveTopology
{
	ePointList = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	eLineList = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	eLineStrip = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	eTriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	eTriangleStrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	eLineListAdj = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	eLineStripAdj = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	eTriangleListAdj = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	eTriangleStripAdj = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
	eControlPointPatchList3 = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
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
};

enum eRTextureFilter
{
	ePoint = D3D11_FILTER_MIN_MAG_MIP_POINT,
	eLinear = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	eAnisotropic = D3D11_FILTER_ANISOTROPIC
};

enum eRTextureAddressMode
{
	eWrap = D3D11_TEXTURE_ADDRESS_WRAP,
	eMirror = D3D11_TEXTURE_ADDRESS_MIRROR,
	eClamp = D3D11_TEXTURE_ADDRESS_CLAMP,
	eBorder = D3D11_TEXTURE_ADDRESS_BORDER,
	eMirrorOnce = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
};

enum eRDepthStencilViewFlags
{
	eReadonlyDepth = D3D11_DSV_READ_ONLY_DEPTH,
	eReadonlyStencil = D3D11_DSV_READ_ONLY_STENCIL,
	eReadonlyDepthStencil = eReadonlyDepth | eReadonlyStencil
};

enum eRDepthStencilClearFlag
{
	eClearDepthOnly = D3D11_CLEAR_DEPTH,
	eClearStencilOnly = D3D11_CLEAR_STENCIL,
	eClearDepthStencil = eClearDepthOnly | eClearStencilOnly,
};

enum eRFillMode
{
	eWireframe = D3D11_FILL_WIREFRAME,
	eSolid = D3D11_FILL_SOLID
};

enum eRCullMode
{
	eCullNone = D3D11_CULL_NONE,
	eCullFrontFace = D3D11_CULL_FRONT,
	eCullBackFace = D3D11_CULL_BACK
};

enum class eRComparisonFunc
{
	eNever = D3D11_COMPARISON_NEVER,
	eLess = D3D11_COMPARISON_LESS,
	eEqual = D3D11_COMPARISON_EQUAL,
	eLessEqual = D3D11_COMPARISON_LESS_EQUAL,
	eGreater = D3D11_COMPARISON_GREATER,
	eNotEqual = D3D11_COMPARISON_NOT_EQUAL,
	eGreaterEqual = D3D11_COMPARISON_GREATER_EQUAL,
	eAlways = D3D11_COMPARISON_ALWAYS
};

enum class eRBlend
{
	eZero = D3D11_BLEND_ZERO,
	eOne = D3D11_BLEND_ONE,
	eFactor = D3D11_BLEND_BLEND_FACTOR,
	eInvFactor = D3D11_BLEND_INV_BLEND_FACTOR,

	eSrcAlpha = D3D11_BLEND_SRC_ALPHA,
	eInvSrcAlpha = D3D11_BLEND_INV_SRC_ALPHA,
	eDestAlpha = D3D11_BLEND_DEST_ALPHA,
	eInvDestAlpha = D3D11_BLEND_INV_DEST_ALPHA,
	eSrcAlphaSat = D3D11_BLEND_SRC_ALPHA_SAT,
	eSrc1Alpha = D3D11_BLEND_SRC1_ALPHA,
	eInvSrc1Alpha = D3D11_BLEND_INV_SRC1_ALPHA,

	eSrcColor = D3D11_BLEND_SRC_COLOR,
	eInvSrcColor = D3D11_BLEND_INV_SRC_COLOR,
	eDestColor = D3D11_BLEND_DEST_COLOR,
	eInvDestColor = D3D11_BLEND_INV_DEST_COLOR,
	eSrc1Color = D3D11_BLEND_SRC1_COLOR,
	eInvSrc1Color = D3D11_BLEND_INV_SRC1_COLOR,
};

enum class eRBlendOp
{
	eAdd = D3D11_BLEND_OP_ADD,
	eSubtract = D3D11_BLEND_OP_SUBTRACT,
	eRevSubtract = D3D11_BLEND_OP_REV_SUBTRACT,
	eMin = D3D11_BLEND_OP_MIN,
	eMax = D3D11_BLEND_OP_MAX
};

enum class eRStencilOp
{
	eKeep = D3D11_STENCIL_OP_KEEP,
	eZero = D3D11_STENCIL_OP_ZERO,
	eReplace = D3D11_STENCIL_OP_REPLACE,  /// Set the stencil data to the reference value
	eIncrSat = D3D11_STENCIL_OP_INCR_SAT, /// Increment the stencil value by 1, and clamp the result
	eDecrSat = D3D11_STENCIL_OP_DECR_SAT, /// Decrement the stencil value by 1, and clamp the result
	eInvert = D3D11_STENCIL_OP_INVERT,
	eIncr = D3D11_STENCIL_OP_INCR,        /// Increment the stencil value by 1, and wrap the result if necessary
	eDecr = D3D11_STENCIL_OP_DECR         /// Increment the stencil value by 1, and wrap the result if necessary
};

enum eRBlendWriteMask : uint8_t
{
	eColorNone = 0,
	eColorRed = D3D11_COLOR_WRITE_ENABLE_RED,
	eColorGreen = D3D11_COLOR_WRITE_ENABLE_GREEN,
	eColorBlue = D3D11_COLOR_WRITE_ENABLE_BLUE,
	eColorAlpha = D3D11_COLOR_WRITE_ENABLE_ALPHA,
	eColorAll = D3D11_COLOR_WRITE_ENABLE_ALL
};

enum eRDepthWriteMask
{
	eDepthMaskZero = D3D11_DEPTH_WRITE_MASK_ZERO,  /// Turn off writes to the depth-stencil buffer.
	eDepthMaskAll = D3D11_DEPTH_WRITE_MASK_ALL     /// Turn on writes to the depth-stencil buffer.
};

enum eRStencilMask : uint8_t
{
	eStencilDefaultReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
	eStencilDefaultWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
};

enum eRFormat
{
	eUnknownFormat = DXGI_FORMAT_UNKNOWN,
	eRGBA32_Typeless = DXGI_FORMAT_R32G32B32A32_TYPELESS,
	eRGBA32_Float = DXGI_FORMAT_R32G32B32A32_FLOAT,
	eRGBA32_UInt = DXGI_FORMAT_R32G32B32A32_UINT,
	eRGBA32_SInt = DXGI_FORMAT_R32G32B32A32_SINT,
	eRGB32_Typeless = DXGI_FORMAT_R32G32B32_TYPELESS,
	eRGB32_Float = DXGI_FORMAT_R32G32B32_FLOAT,
	eRGB32_UInt = DXGI_FORMAT_R32G32B32_UINT,
	eRGB32_SInt = DXGI_FORMAT_R32G32B32_SINT,
	eRGBA16_Typeless = DXGI_FORMAT_R16G16B16A16_TYPELESS,
	eRGBA16_Float = DXGI_FORMAT_R16G16B16A16_FLOAT,
	eRGBA16_UNorm = DXGI_FORMAT_R16G16B16A16_UNORM,
	eRGBA16_UInt = DXGI_FORMAT_R16G16B16A16_UINT,
	eRGBA16_SNorm = DXGI_FORMAT_R16G16B16A16_SNORM,
	eRGBA16_SInt = DXGI_FORMAT_R16G16B16A16_SINT,
	eRG32_Typeless = DXGI_FORMAT_R32G32_TYPELESS,
	eRG32_Float = DXGI_FORMAT_R32G32_FLOAT,
	eRG32_UInt = DXGI_FORMAT_R32G32_UINT,
	eRG32_SInt = DXGI_FORMAT_R32G32_SINT,
	eR32G8X24_Typeless = DXGI_FORMAT_R32G8X24_TYPELESS,
	eD32_Float_S8X24_Uint = DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	eR32_Float_X8X24_Typeless = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
	eX32_Typeless_G8X24_UInt = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
	eRGB10A2_Typeless = DXGI_FORMAT_R10G10B10A2_TYPELESS,
	eRGB10A2_UNorm = DXGI_FORMAT_R10G10B10A2_UNORM,
	eRGB10A2_UInt = DXGI_FORMAT_R10G10B10A2_UINT,
	eRG11B10_Float = DXGI_FORMAT_R11G11B10_FLOAT,
	eRGBA8_Typeless = DXGI_FORMAT_R8G8B8A8_TYPELESS,
	eRGBA8_UNorm = DXGI_FORMAT_R8G8B8A8_UNORM,
	eRGBA8_UNorm_SRGB = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	eRGBA8_UInt = DXGI_FORMAT_R8G8B8A8_UINT,
	eRGBA8_SNorm = DXGI_FORMAT_R8G8B8A8_SNORM,
	eRGBA8_SInt = DXGI_FORMAT_R8G8B8A8_SINT,
	eRG16_Typeless = DXGI_FORMAT_R16G16_TYPELESS,
	eRG16_Float = DXGI_FORMAT_R16G16_FLOAT,
	eRG16_UNorm = DXGI_FORMAT_R16G16_UNORM,
	eRG16_UInt = DXGI_FORMAT_R16G16_UINT,
	eRG16_SNorm = DXGI_FORMAT_R16G16_SNORM,
	eRG16_SInt = DXGI_FORMAT_R16G16_SINT,
	eR32_Typeless = DXGI_FORMAT_R32_TYPELESS,
	eD32_Float = DXGI_FORMAT_D32_FLOAT,
	eR32_Float = DXGI_FORMAT_R32_FLOAT,
	eR32_UInt = DXGI_FORMAT_R32_UINT,
	eR32_SInt = DXGI_FORMAT_R32_SINT,
	eR24G8_Typeless = DXGI_FORMAT_R24G8_TYPELESS,
	eD24_UNorm_S8_UInt = DXGI_FORMAT_D24_UNORM_S8_UINT,
	eR24_UNorm_X8_Typeless = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	eX24_Typelesss_G8_UInt = DXGI_FORMAT_X24_TYPELESS_G8_UINT,
	eRG8_Typeless = DXGI_FORMAT_R8G8_TYPELESS,
	eRG8_UNorm = DXGI_FORMAT_R8G8_UNORM,
	eRG8_UInt = DXGI_FORMAT_R8G8_UINT,
	eRG8_SNorm = DXGI_FORMAT_R8G8_SNORM,
	eRG8_SInt = DXGI_FORMAT_R8G8_SINT,
	eR16_Typeless = DXGI_FORMAT_R16_TYPELESS,
	eR16_Float = DXGI_FORMAT_R16_FLOAT,
	eD16_UNorm = DXGI_FORMAT_D16_UNORM,
	eR16_UNorm = DXGI_FORMAT_R16_UNORM,
	eR16_UInt = DXGI_FORMAT_R16_UINT,
	eR16_SNorm = DXGI_FORMAT_R16_SNORM,
	eR16_SInt = DXGI_FORMAT_R16_SINT,
	eR8_Typeless = DXGI_FORMAT_R8_TYPELESS,
	eR8_UNorm = DXGI_FORMAT_R8_UNORM,
	eR8_UInt = DXGI_FORMAT_R8_UINT,
	eR8_SNorm = DXGI_FORMAT_R8_SNORM,
	eR8_SInt = DXGI_FORMAT_R8_SINT,
	eA8_UNorm = DXGI_FORMAT_A8_UNORM,
	eR1_UNorm = DXGI_FORMAT_R1_UNORM,
	eRGB9E5_SharedXP = DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	eRG8_BG8_UNorm = DXGI_FORMAT_R8G8_B8G8_UNORM,
	eGR8_GB8_UNorm = DXGI_FORMAT_G8R8_G8B8_UNORM,
	eBC1_Typeless = DXGI_FORMAT_BC1_TYPELESS,
	eBC1_UNorm = DXGI_FORMAT_BC1_UNORM,
	eBC1_UNorm_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,
	eBC2_Typeless = DXGI_FORMAT_BC2_TYPELESS,
	eBC2_UNorm = DXGI_FORMAT_BC2_UNORM,
	eBC2_UNorm_SRGB = DXGI_FORMAT_BC2_UNORM_SRGB,
	eBC3_Typeless = DXGI_FORMAT_BC3_TYPELESS,
	eBC3_UNorm = DXGI_FORMAT_BC3_UNORM,
	eBC3_UNorm_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,
	eBC4_Typeless = DXGI_FORMAT_BC4_TYPELESS,
	eBC4_UNorm = DXGI_FORMAT_BC4_UNORM,
	eBC4_SNorm = DXGI_FORMAT_BC4_SNORM,
	eBC5_Typeless = DXGI_FORMAT_BC5_TYPELESS,
	eBC5_UNorm = DXGI_FORMAT_BC5_UNORM,
	eBC5_SNorm = DXGI_FORMAT_BC5_SNORM,
	eB5G6R5_UNorm = DXGI_FORMAT_B5G6R5_UNORM,
	eBGR5A1_UNorm = DXGI_FORMAT_B5G5R5A1_UNORM,
	eBGRA8_UNorm = DXGI_FORMAT_B8G8R8A8_UNORM,
	eBGRX8_UNorm = DXGI_FORMAT_B8G8R8X8_UNORM,
	eRGB10_XR_Bias_A2_UNorm = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
	eBGRA8_Typeless = DXGI_FORMAT_B8G8R8A8_TYPELESS,
	eBGRA8_UNorm_SRGB = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
	eBGRX8_Typeless = DXGI_FORMAT_B8G8R8X8_TYPELESS,
	eBGRX8_UNorm_SRGB = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
	eBC6H_Typeless = DXGI_FORMAT_BC6H_TYPELESS,
	eBC6H_UF16 = DXGI_FORMAT_BC6H_UF16,
	eBC6H_SF16 = DXGI_FORMAT_BC6H_SF16,
	eBC7_Typeless = DXGI_FORMAT_BC7_TYPELESS,
	eBC7_UNorm = DXGI_FORMAT_BC7_UNORM,
	eBC7_UNorm_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,
	eAYUV = DXGI_FORMAT_AYUV,
	eY410 = DXGI_FORMAT_Y410,
	eY416 = DXGI_FORMAT_Y416,
	eNV12 = DXGI_FORMAT_NV12,
	eP010 = DXGI_FORMAT_P010,
	eP016 = DXGI_FORMAT_P016,
	e420_Opaque = DXGI_FORMAT_420_OPAQUE,
	eYUY2 = DXGI_FORMAT_YUY2,
	eY210 = DXGI_FORMAT_Y210,
	eY216 = DXGI_FORMAT_Y216,
	eNV11 = DXGI_FORMAT_NV11,
	eAI44 = DXGI_FORMAT_AI44,
	eIA44 = DXGI_FORMAT_IA44,
	eP8 = DXGI_FORMAT_P8,
	eAP8 = DXGI_FORMAT_A8P8,
	eBGRA4_UNorm = DXGI_FORMAT_B4G4R4A4_UNORM,
	eForce_UInt = DXGI_FORMAT_FORCE_UINT,
};
