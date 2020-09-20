#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

#if 0

DECLARE_SHARED_PTR(D3D11Context)
class D3D11Context : public D3DObject<ID3D11DeviceContext>
{
};

DECLARE_UNIQUE_PTR(D3D11Device)
class D3D11Device final : public D3DObject<ID3D11Device>, public Adapter
{
public:
	D3D11Device(DXGI_Factory& factory);
protected:
	struct D3D11SubVersions
	{
		uint8_t DXGI_SubVersion = 0u;
	};
private:
	D3D11ContextPtr m_IMContext = nullptr;
	DXGI_Adapter m_Adapter;
	D3D11SubVersions m_SubVersions{};
};

#endif

NAMESPACE_END(Gfx)
