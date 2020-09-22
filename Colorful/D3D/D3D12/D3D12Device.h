#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

#include <d3d12.h>

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(D3D12Debug)
class D3D12Debug final : public D3DObject<ID3D12Debug>
{
public:
	D3D12Debug();
};

DECLARE_UNIQUE_PTR(D3D12Device)
class D3D12Device final : public D3DObject<ID3D12Device>, public Adapter
{
public:
	D3D12Device(IDXGIAdapter1* adapter1, IDXGIAdapter4* adapter4);
protected:
private:
};

NAMESPACE_END(Gfx)
