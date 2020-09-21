#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

class D3D11Context1 : public D3DObject<ID3D11DeviceContext1> {};
class D3D11Context2 : public D3DObject<ID3D11DeviceContext2> {};
class D3D11Context3 : public D3DObject<ID3D11DeviceContext3> {};
class D3D11Context4 : public D3DObject<ID3D11DeviceContext4> {};

DECLARE_SHARED_PTR(D3D11Context)
class D3D11Context : public D3DObject<ID3D11DeviceContext>
{
public:

};

class D3D11Device1 : public D3DObject<ID3D11Device1> {};
class D3D11Device2 : public D3DObject<ID3D11Device2> {};
class D3D11Device3 : public D3DObject<ID3D11Device3> {};
class D3D11Device4 : public D3DObject<ID3D11Device4> {};
class D3D11Device5 : public D3DObject<ID3D11Device5> {};

DECLARE_UNIQUE_PTR(D3D11Device)
class D3D11Device final : public D3DObject<ID3D11Device>, public Adapter
{
public:
	D3D11Device(IDXGIFactory1* factory);
protected:
private:
	D3D11ContextPtr m_IMContext = nullptr;


};

NAMESPACE_END(Gfx)
