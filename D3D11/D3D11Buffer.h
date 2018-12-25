#pragma once

#include "D3D11Object.h"

class D3D11Buffer : public D3DObject<ID3D11Buffer>
{
public:
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