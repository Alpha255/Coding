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

	void Create(
		uint32_t bindFlags,
		size_t byteWidth,
		eUsage usage,
		const void *pData,
		uint32_t cpuAccessFlag = 0U,
		uint32_t miscFlags = 0U,
		uint32_t byteStride = 0U,
		uint32_t memPitch = 0U);

	inline void CreateVertexBuffer(
		size_t byteWidth,
		eUsage usage,
		const void *pData,
		uint32_t cpuAccessFlags = 0U,
		uint32_t bindFlags = 0U)
	{
		Create(D3D11_BIND_VERTEX_BUFFER | bindFlags, byteWidth, usage, pData, cpuAccessFlags);
	}

	inline void CreateIndexBuffer(
		size_t byteWidth,
		eUsage usage,
		const void *pData,
		uint32_t cpuAccessFlags = 0U)
	{
		Create(D3D11_BIND_INDEX_BUFFER, byteWidth, usage, pData, cpuAccessFlags);
	}

	inline void CreateConstantBuffer(
		size_t byteWidth,
		eUsage usage,
		const void *pData,
		uint32_t cpuAccessFlags = 0U)
	{
		Create(D3D11_BIND_CONSTANT_BUFFER, byteWidth, usage, pData, cpuAccessFlags);
	}

	inline void CreateUnorderedAccessBuffer(
		size_t byteWidth,
		eUsage usage,
		uint32_t miscFlags,
		uint32_t byteStride,
		uint32_t bindFlags = 0U)
	{
		assert(byteStride);
		Create(bindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, byteWidth, usage, nullptr, 0U, miscFlags, byteStride, 0U);
	}

	void Update(const void *pData, size_t size);
};

struct D3DVertexBuffer
{
	D3DBuffer VertexBuffer;
	uint32_t Stride = 0U;
	uint32_t Offset = 0U;
};

struct D3DIndexBuffer
{
	D3DBuffer IndexBuffer;
	uint32_t Format = 0U;
	uint32_t Offset = 0U;
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