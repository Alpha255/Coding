#pragma once

#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

/// ID3D11Device5
class d3d12CommandQueue : public D3DObject<ID3D12CommandQueue>
{
public:
	void create(const class d3d12Device &device);
};

class d3d12Device : public D3DObject<ID3D12Device>, public GfxDevice
{
public:
	void create(const dxgiFactory7 &inDxgiFactory);
	
	rShader *createShader(eRShaderUsage usage, const std::string &shaderName);

	rTexture *createTexture(const std::string &textureName);

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
