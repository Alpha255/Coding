#pragma once

#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

/// ID3D11Device5
class d3d12CommandQueue : public D3DObject<ID3D12CommandQueue>
{
public:
	void create(const class D3D12Device &device);
};

class D3D12Device : public D3DObject<ID3D12Device>, public GfxDevice
{
public:
	void create(const DXGIFactory7& dxgiFactory);

	inline DXGIAdapter4 *getDxgiAdapter()
	{
		return &m_dxgiAdapter;
	}

	inline d3d12CommandQueue *getCommandQueue()
	{
		return &m_CommandQueue;
	}
protected:
private:
	DXGIAdapter4 m_dxgiAdapter;
	d3d12CommandQueue m_CommandQueue;
};
