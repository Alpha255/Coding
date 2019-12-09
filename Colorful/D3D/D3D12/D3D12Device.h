#pragma once

#include "Colorful/D3D/D3D11/D3D11Context.h"

/// ID3D11Device5
class d3d12CommandQueue : public d3dObject<ID3D12CommandQueue>
{
public:
	void create(const class d3d12Device &device);
};

class d3d12Device : public d3dObject<ID3D12Device>, public rDevice
{
public:
	void create(const dxgiFactory7 &inDxgiFactory);
	
	rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) override final;

	inline dxgiAdapter4 *getDxgiAdapter()
	{
		return &m_dxgiAdapter;
	}

	inline d3d12CommandQueue *getCommandQueue()
	{
		return &m_CommandQueue;
	}
protected:
private:
	dxgiAdapter4 m_dxgiAdapter;
	d3d12CommandQueue m_CommandQueue;
};
