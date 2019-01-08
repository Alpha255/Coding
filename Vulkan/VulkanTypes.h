#pragma once

#include "Util/Common.h"
#include "Util/System.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define VKCheck(func) if (VK_SUCCESS != (func)) { assert(0); }

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
};

enum eRBufferCpuAccessFlags
{
	eCpuRead,
	eCpuWrite,
	eCpuReadWrite = eCpuRead | eCpuWrite
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
};

enum eRPrimitiveTopology
{
	ePointList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
	eLineList = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	eLineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	eTriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	eTriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	eLineListAdj = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
	eLineStripAdj = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
	eTriangleListAdj = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
	eTriangleStripAdj = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
	eControlPointPatchList3 = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
	eRPrimitiveTopologyCount
};

enum eRViewType
{
	eTexture1D = VK_IMAGE_VIEW_TYPE_1D,
	eTexture2D = VK_IMAGE_VIEW_TYPE_2D,
	eTexture3D = VK_IMAGE_VIEW_TYPE_3D,
	eTextureCube = VK_IMAGE_VIEW_TYPE_CUBE,
	eTexture1DArray = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
	eTexture2DArray = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
	eTextureCubeArray = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
	eBuffer,
	eUnknownViewType
};

enum eRTextureFilter
{
	ePoint = VK_FILTER_NEAREST,
	eLinear = VK_FILTER_LINEAR,
	eAnisotropic
};

enum eRTextureAddressMode
{
	eWrap = VK_SAMPLER_ADDRESS_MODE_REPEAT,
	eMirror = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	eClamp = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
	eBorder = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
	eMirrorOnce = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

enum eRFillMode
{
	eWireframe = VK_POLYGON_MODE_LINE,
	eSolid = VK_POLYGON_MODE_FILL
};

enum eRCullMode
{
	eCullNone = VK_CULL_MODE_NONE,
	eCullFrontFace = VK_CULL_MODE_FRONT_BIT,
	eCullBackFace = VK_CULL_MODE_BACK_BIT
};

enum class eRComparisonFunc
{
	eNever = VK_COMPARE_OP_NEVER,
	eLess = VK_COMPARE_OP_LESS,
	eEqual = VK_COMPARE_OP_EQUAL,
	eLessEqual = VK_COMPARE_OP_LESS_OR_EQUAL,
	eGreater = VK_COMPARE_OP_GREATER,
	eNotEqual = VK_COMPARE_OP_NOT_EQUAL,
	eGreaterEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
	eAlways = VK_COMPARE_OP_ALWAYS
};

enum class eRBlend
{
	eZero = VK_BLEND_FACTOR_ZERO,
	eOne = VK_BLEND_FACTOR_ONE,
	//eFactor = VK_BLEND_FACTOR_CONSTANT_ALPHA,
	//eInvFactor = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,

	eSrcAlpha = VK_BLEND_FACTOR_SRC_ALPHA,
	eInvSrcAlpha = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	eDestAlpha = VK_BLEND_FACTOR_DST_ALPHA,
	eInvDestAlpha = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	eSrcAlphaSat = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
	eSrc1Alpha = VK_BLEND_FACTOR_SRC1_ALPHA,
	eInvSrc1Alpha = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,

	eSrcColor = VK_BLEND_FACTOR_SRC_COLOR,
	eInvSrcColor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	eDestColor = VK_BLEND_FACTOR_DST_COLOR,
	eInvDestColor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	eSrc1Color = VK_BLEND_FACTOR_SRC1_COLOR,
	eInvSrc1Color = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
};

enum class eRBlendOp
{
	eAdd = VK_BLEND_OP_ADD,
	eSubtract = VK_BLEND_OP_SUBTRACT,
	eRevSubtract = VK_BLEND_OP_REVERSE_SUBTRACT,
	eMin = VK_BLEND_OP_MIN,
	eMax = VK_BLEND_OP_MAX
};

enum class eRStencilOp
{
	eKeep = VK_STENCIL_OP_KEEP,
	eZero = VK_STENCIL_OP_ZERO,
	eReplace = VK_STENCIL_OP_REPLACE,  /// Set the stencil data to the reference value
	eIncrSat = VK_STENCIL_OP_INCREMENT_AND_CLAMP, /// Increment the stencil value by 1, and clamp the result
	eDecrSat = VK_STENCIL_OP_DECREMENT_AND_CLAMP, /// Decrement the stencil value by 1, and clamp the result
	eInvert = VK_STENCIL_OP_INVERT,
	eIncr = VK_STENCIL_OP_INCREMENT_AND_WRAP,        /// Increment the stencil value by 1, and wrap the result if necessary
	eDecr = VK_STENCIL_OP_DECREMENT_AND_WRAP         /// Increment the stencil value by 1, and wrap the result if necessary
};

enum eRBlendWriteMask : uint8_t
{
	eColorNone = 0,
	eColorRed,
	eColorGreen,
	eColorBlue,
	eColorAlpha,
	eColorAll
};

enum eRDepthWriteMask
{
	eDepthMaskZero,  /// Turn off writes to the depth-stencil buffer.
	eDepthMaskAll     /// Turn on writes to the depth-stencil buffer.
};

enum eRStencilMask : uint8_t
{
	eStencilDefaultReadMask = 0xff,
	eStencilDefaultWriteMask = 0xff
};

enum eRFormat
{
	eUnknownFormat = VK_FORMAT_UNDEFINED,
	eRGBA32_Typeless,
	eRGBA32_Float = VK_FORMAT_R32G32B32A32_SFLOAT,
	eRGBA32_UInt = VK_FORMAT_R32G32B32A32_UINT,
	eRGBA32_SInt = VK_FORMAT_R32G32B32A32_SINT,
	eRGB32_Typeless,
	eRGB32_Float = VK_FORMAT_R32G32B32_SFLOAT,
	eRGB32_UInt = VK_FORMAT_R32G32B32_UINT,
	eRGB32_SInt = VK_FORMAT_R32G32B32_SINT,
	eRGBA16_Typeless,
	eRGBA16_Float = VK_FORMAT_R16G16B16A16_SFLOAT,
	eRGBA16_UNorm = VK_FORMAT_R16G16B16A16_UNORM,
	eRGBA16_UInt = VK_FORMAT_R16G16B16A16_UINT,
	eRGBA16_SNorm = VK_FORMAT_R16G16B16A16_SNORM,
	eRGBA16_SInt = VK_FORMAT_R16G16B16A16_SINT,
	eRG32_Typeless,
	eRG32_Float = VK_FORMAT_R32G32_SFLOAT,
	eRG32_UInt = VK_FORMAT_R32G32_UINT,
	eRG32_SInt = VK_FORMAT_R32G32_SINT,
	eR32G8X24_Typeless,
	eD32_Float_S8X24_Uint,
	eR32_Float_X8X24_Typeless,
	eX32_Typeless_G8X24_UInt,
	eRGB10A2_Typeless,
	eRGB10A2_UNorm = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
	eRGB10A2_UInt = VK_FORMAT_A2R10G10B10_UINT_PACK32,
	eRG11B10_Float = VK_FORMAT_B10G11R11_UFLOAT_PACK32,
	eRGBA8_Typeless,
	eRGBA8_UNorm = VK_FORMAT_R8G8B8A8_UNORM,
	eRGBA8_UNorm_SRGB = VK_FORMAT_R8G8B8A8_SRGB,
	eRGBA8_UInt = VK_FORMAT_R8G8B8A8_UINT,
	eRGBA8_SNorm = VK_FORMAT_R8G8B8A8_SNORM,
	eRGBA8_SInt = VK_FORMAT_R8G8B8A8_SINT,
	eRG16_Typeless,
	eRG16_Float = VK_FORMAT_R16G16_SFLOAT,
	eRG16_UNorm = VK_FORMAT_R16G16_UNORM,
	eRG16_UInt = VK_FORMAT_R16G16_UINT,
	eRG16_SNorm = VK_FORMAT_R16G16_SNORM,
	eRG16_SInt = VK_FORMAT_R16G16_SINT,
	eR32_Typeless,
	eD32_Float = VK_FORMAT_D32_SFLOAT,
	eR32_Float = VK_FORMAT_R32_SFLOAT,
	eR32_UInt = VK_FORMAT_R32_UINT,
	eR32_SInt = VK_FORMAT_R32_SINT,
	eR24G8_Typeless,
	eD24_UNorm_S8_UInt = VK_FORMAT_D24_UNORM_S8_UINT,
	eR24_UNorm_X8_Typeless = VK_FORMAT_X8_D24_UNORM_PACK32,
	eX24_Typelesss_G8_UInt,
	eRG8_Typeless,
	eRG8_UNorm = VK_FORMAT_R8G8_UNORM,
	eRG8_UInt = VK_FORMAT_R8G8_UINT,
	eRG8_SNorm = VK_FORMAT_R8G8_SNORM,
	eRG8_SInt = VK_FORMAT_R8G8_SINT,
	eR16_Typeless,
	eR16_Float = VK_FORMAT_R16_SFLOAT,
	eD16_UNorm = VK_FORMAT_D16_UNORM,
	eR16_UNorm = VK_FORMAT_R16_UNORM,
	eR16_UInt = VK_FORMAT_R16_UINT,
	eR16_SNorm = VK_FORMAT_R16_SNORM,
	eR16_SInt = VK_FORMAT_R16_SINT,
	eR8_Typeless,
	eR8_UNorm = VK_FORMAT_R8_UNORM,
	eR8_UInt = VK_FORMAT_R8_UINT,
	eR8_SNorm = VK_FORMAT_R8_SNORM,
	eR8_SInt = VK_FORMAT_R8_SINT,
	eA8_UNorm,
	eR1_UNorm,
	eRGB9E5_SharedXP = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
	eRG8_BG8_UNorm = VK_FORMAT_B8G8R8G8_422_UNORM,
	eGR8_GB8_UNorm = VK_FORMAT_G8B8G8R8_422_UNORM,
	eBC1_Typeless,
	eBC1_UNorm = VK_FORMAT_BC1_RGB_UNORM_BLOCK,
	eBC1_UNorm_SRGB = VK_FORMAT_BC1_RGB_SRGB_BLOCK,
	eBC2_Typeless,
	eBC2_UNorm = VK_FORMAT_BC2_UNORM_BLOCK,
	eBC2_UNorm_SRGB = VK_FORMAT_BC2_SRGB_BLOCK,
	eBC3_Typeless,
	eBC3_UNorm = VK_FORMAT_BC3_UNORM_BLOCK,
	eBC3_UNorm_SRGB = VK_FORMAT_BC3_SRGB_BLOCK,
	eBC4_Typeless,
	eBC4_UNorm = VK_FORMAT_BC4_UNORM_BLOCK,
	eBC4_SNorm = VK_FORMAT_BC4_SNORM_BLOCK,
	eBC5_Typeless,
	eBC5_UNorm = VK_FORMAT_BC5_UNORM_BLOCK,
	eBC5_SNorm = VK_FORMAT_BC5_SNORM_BLOCK,
	eB5G6R5_UNorm = VK_FORMAT_R5G6B5_UNORM_PACK16,
	eBGR5A1_UNorm = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
	eBGRA8_UNorm = VK_FORMAT_B8G8R8A8_UNORM,
	eBGRX8_UNorm,
	eRGB10_XR_Bias_A2_UNorm,
	eBGRA8_Typeless,
	eBGRA8_UNorm_SRGB = VK_FORMAT_B8G8R8A8_SRGB,
	eBGRX8_Typeless,
	eBGRX8_UNorm_SRGB,
	eBC6H_Typeless,
	eBC6H_UF16 = VK_FORMAT_BC6H_UFLOAT_BLOCK,
	eBC6H_SF16 = VK_FORMAT_BC6H_SFLOAT_BLOCK,
	eBC7_Typeless,
	eBC7_UNorm = VK_FORMAT_BC7_UNORM_BLOCK,
	eBC7_UNorm_SRGB = VK_FORMAT_BC7_SRGB_BLOCK,
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
	eBGRA4_UNorm = VK_FORMAT_B4G4R4A4_UNORM_PACK16,
	eForce_UInt,
};
