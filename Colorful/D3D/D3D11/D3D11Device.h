#pragma once

#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

/// ID3D11Device5
class d3d11Device : public D3DObject<ID3D11Device>, public rDevice
{
public:
	void create(__out d3d11Context &context, const dxgiFactory7 &inDxgiFactory);
	
	rShader *createShader(eRShaderUsage usage, const std::string &shaderName);

	rTexture *createTexture(const std::string &textureName);

	inline dxgiAdapter4 *getDxgiAdapter()
	{
		return &m_dxgiAdapter;
	}
protected:
private:
	dxgiAdapter4 m_dxgiAdapter;
};
