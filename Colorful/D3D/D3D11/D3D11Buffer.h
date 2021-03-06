#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

class D3D11Buffer final : public D3DObject<ID3D11Buffer>, public GpuBuffer
{
public:
	D3D11Buffer(ID3D11Device* device, ID3D11DeviceContext* IMContext, uint32_t bindFlags, EBufferUsage usage, size_t size, const void* data);

	void unmap() override final;
protected:
	inline uint32_t CPUAccessFlags(EBufferUsage usage) const
	{
		switch (usage)
		{
		case EBufferUsage::Default:
		case EBufferUsage::Immutable: 
		default:
			return 0u;
		case EBufferUsage::Dynamic:   return D3D11_CPU_ACCESS_WRITE;
		case EBufferUsage::Staging:   return D3D11_CPU_ACCESS_READ;
		}
	}

	void* map_native(size_t size, size_t offset) override final;
	void update_native(const void* data, size_t size, size_t offset, bool8_t persistence) override final;
	void flushMappedRange_native(size_t size, size_t offset) override final;
private:
	static ID3D11DeviceContext* s_IMContext;
};

NAMESPACE_END(Gfx)
