#pragma once

#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

/// ID3D11Device5
class d3d11Device : public d3dObject<ID3D11Device>, public rDevice
{
public:
	void create(__out d3d11Context &context, const dxgiFactory7 &inDxgiFactory);
	
	rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) override final;

	rTexturePtr createTexture(const std::string &textureName) override final;

	inline dxgiAdapter4 *getDxgiAdapter()
	{
		return &m_dxgiAdapter;
	}
protected:
private:
	dxgiAdapter4 m_dxgiAdapter;
};
