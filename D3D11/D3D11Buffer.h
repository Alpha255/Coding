#pragma once

#include "D3D11Object.h"

class D3D11Buffer : public D3DObject<ID3D11Buffer>
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

struct D3D11VertexBuffer
{
	D3D11Buffer Buffer;
	uint32_t Stride = 0U;
	uint32_t Offset = 0U;

	bool operator==(const D3D11VertexBuffer &other)
	{
		return (Stride == other.Stride && Offset == other.Offset && Buffer == other.Buffer);
	}

	bool operator!=(const D3D11VertexBuffer &other)
	{
		return !(*this == other);
	}
};

struct D3D11IndexBuffer
{
	D3D11Buffer Buffer;
	uint32_t Format = 0U;
	uint32_t Offset = 0U;

	bool operator==(const D3D11IndexBuffer &other)
	{
		return (Format == other.Format && Offset == other.Offset && Buffer == other.Buffer);
	}

	bool operator!=(const D3D11IndexBuffer &other)
	{
		return !(*this == other);
	}
};