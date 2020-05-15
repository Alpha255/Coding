#pragma once

#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

/// ID3D11Device5
class D3D11Device : public D3DObject<ID3D11Device>, public GfxDevice
{
public:
	void create(__out D3D11Context& IMContext, const DXGIFactory7& factory);
	
	GfxShader *createShader(eRShaderUsage usage, const std::string &shaderName);

	GfxTexture *createTexture(const std::string &textureName);

	inline DXGIAdapter4 *getDxgiAdapter()
	{
		return &m_DXGIAdapter;
	}
protected:
private:
	DXGIAdapter4 m_DXGIAdapter;
};
