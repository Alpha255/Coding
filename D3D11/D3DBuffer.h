#pragma once

#include "D3DObject.h"

class D3DBuffer : public D3DObject<ID3D11Buffer>
{
public:
	enum eUsage
	{
		eGpuReadWrite = D3D11_USAGE_DEFAULT,
		eGpuReadOnly = D3D11_USAGE_IMMUTABLE,
		eGpuReadCpuWrite = D3D11_USAGE_DYNAMIC,
		eGpuCopyToCpu = D3D11_USAGE_STAGING
	};

	enum eCpuAccessFlag
	{
		eCpuRead = D3D11_CPU_ACCESS_READ,
		eCpuWrite = D3D11_CPU_ACCESS_WRITE,
		eCpuReadWrite = eCpuRead | eCpuWrite
	};

	enum eBindFlag
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
		eBindAsVideoEncoder = D3D11_BIND_VIDEO_ENCODER
	};

	enum eResourceMiscFlag
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

	inline void CreateAsVertexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t bindFlags = 0U)
	{
		Create(eBindAsVertexBuffer | bindFlags, byteWidth, usage, pData, GetCpuAccessFlag(usage));
	}

	inline void CreateAsIndexBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData)
	{
		Create(eBindAsIndexBuffer, byteWidth, usage, pData, GetCpuAccessFlag(usage));
	}

	inline void CreateAsConstantBuffer(
		size_t byteWidth,
		uint32_t usage,
		const void *pData = nullptr)
	{
		Create(eBindAsConstantBuffer, byteWidth, usage, pData, GetCpuAccessFlag(usage));
	}

	inline void CreateAsUnorderedAccessBuffer(
		size_t byteWidth,
		uint32_t usage,
		uint32_t miscFlags,
		uint32_t byteStride,
		uint32_t bindFlags = 0U)
	{
		assert(byteStride);
		Create(bindFlags | eBindAsShaderResource | eBindAsUnorderedAccess, byteWidth, usage, nullptr, 0U, miscFlags, byteStride, 0U);
	}

	void Update(const void *pData, size_t size);

protected:
	void Create(
		uint32_t bindFlags,
		size_t byteWidth,
		uint32_t usage,
		const void *pData,
		uint32_t cpuAccessFlag = 0U,
		uint32_t miscFlags = 0U,
		uint32_t byteStride = 0U,
		uint32_t memPitch = 0U);

	inline uint32_t GetCpuAccessFlag(uint32_t usage) const 
	{
		switch (usage)
		{
		case eGpuReadWrite:
		case eGpuReadOnly:
			return 0U;
		case eGpuReadCpuWrite:
			return eCpuWrite;
		case eGpuCopyToCpu:
			return eCpuRead;
		}

		assert(0);
		return 0U;
	}
};

struct D3DVertexBuffer
{
	D3DBuffer Buffer;
	uint32_t Stride = 0U;
	uint32_t Offset = 0U;

	bool operator==(const D3DVertexBuffer &other)
	{
		return (Stride == other.Stride && Offset == other.Offset && Buffer == other.Buffer);
	}

	bool operator!=(const D3DVertexBuffer &other)
	{
		return !(*this == other);
	}
};

struct D3DIndexBuffer
{
	D3DBuffer Buffer;
	uint32_t Format = 0U;
	uint32_t Offset = 0U;

	bool operator==(const D3DIndexBuffer &other)
	{
		return (Format == other.Format && Offset == other.Offset && Buffer == other.Buffer);
	}

	bool operator!=(const D3DIndexBuffer &other)
	{
		return !(*this == other);
	}
};

enum ePrimitiveTopology
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
};

enum eFormat
{
	eUnknown = DXGI_FORMAT_UNKNOWN,
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
	eX32_Typeless_G8X24_Uint = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
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
	eForce_UInt = DXGI_FORMAT_FORCE_UINT
};