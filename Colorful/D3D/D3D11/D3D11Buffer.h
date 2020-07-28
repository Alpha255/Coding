#pragma once

#include "Colorful/D3D/D3D11/D3D11Device.h"
#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

class D3D11Buffer : public D3DObject<ID3D11Buffer>, public GfxGpuBuffer
{
public:
	D3D11Buffer(const D3D11Device& device, eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data);

	void update(const void* data, size_t size, size_t offset) override final;
	void free() override final
	{
	}
protected:
	inline uint32_t getCpuAccessFlags(uint32_t usage) const
	{
		switch (usage)
		{
		case eGpuReadWrite:
		case eGpuReadOnly:
			return 0u;
		case eGpuReadCpuWrite:
			return D3D11_CPU_ACCESS_WRITE;
		case eGpuCopyToCpu:
			return D3D11_CPU_ACCESS_READ;
		}

		assert(0);
		return 0u;
	}

	inline uint32_t getBindFlags(eRBufferBindFlags bindFlags)
	{
		switch (bindFlags)
		{
		case eVertexBuffer:  return D3D11_BIND_VERTEX_BUFFER;
		case eIndexBuffer:   return D3D11_BIND_INDEX_BUFFER;
		case eUniformBuffer: return D3D11_BIND_CONSTANT_BUFFER;
		default:
			assert(0);
		}

		return (uint32_t)-1;
	}
private:
};
using D3D11BufferPtr = D3D11Buffer*;

class D3D11BufferPool : public LazySingleton<D3D11BufferPool>
{
	lazySingletonDeclare(D3D11BufferPool);
public:
	inline D3D11BufferPtr alloc(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void* data)
	{
		return new D3D11Buffer(m_Device, bindFlags, usage, size, data);
	}

	inline D3D11BufferPtr allocIndexBuffer(eRBufferUsage usage, size_t size, const void* data)
	{
		return alloc(eIndexBuffer, usage, size, data);
	}

	inline D3D11BufferPtr allocVertexBuffer(eRBufferUsage usage, size_t size, const void* data)
	{
		return alloc(eVertexBuffer, usage, size, data);
	}

	inline D3D11BufferPtr allocUniformBuffer(size_t size, const void* data)
	{
		return alloc(eUniformBuffer, eGpuReadCpuWrite, size, data);
	}

	inline void free(D3D11BufferPtr buffer)
	{
		assert(buffer);
		buffer->free();
	}

	void updateBuffer(D3D11BufferPtr buffer, const void* data, size_t size, size_t offset);
protected:
	D3D11BufferPool(const D3D11Device& device, const D3D11Context& IMContext)
		: m_Device(device)
		, m_IMContext(IMContext)
	{
	}
private:
	const D3D11Device m_Device;
	const D3D11Context m_IMContext;
};
