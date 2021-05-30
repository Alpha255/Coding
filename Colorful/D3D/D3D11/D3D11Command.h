#pragma once

#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

class D3D11CommandBuffer : public CommandBuffer
{
public:
	D3D11CommandBuffer(ID3D11DeviceContext* IMContext)
		: m_Context(std::move(create_shared<D3D11Context>(IMContext)))
	{
		assert(m_Context);
	}

	D3D11CommandBuffer(ID3D11Device* device);
protected:
	enum class EType
	{
		Immediate,
		Deferred
	};

	D3D11ContextPtr m_Context = nullptr;
	EType m_Type = EType::Immediate;
private:
};
DECLARE_SHARED_PTR(D3D11CommandBuffer)

class D3D11CommandList : public D3DObject<ID3D11CommandList>, public D3D11CommandBuffer
{
public:
	using D3D11CommandBuffer::D3D11CommandBuffer;
};
DECLARE_SHARED_PTR(D3D11CommandList)

NAMESPACE_END(Gfx)
