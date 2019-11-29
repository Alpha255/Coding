#pragma once

#include "Colorful/D3D11/D3D11Context.h"

class d3d11Device : public d3dObject<ID3D11Device>, public rDevice
{
public:
	void create(__out d3d11ContextPtr &context, dxgiFactory7Ptr &dxgiFactory);
	
	rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) override final;

	inline dxgiAdapter4Ptr getDxgiAdapter() const
	{
		return m_dxgiAdapter;
	}
protected:
private:
	dxgiAdapter4Ptr m_dxgiAdapter = nullptr;
};
typedef std::shared_ptr<d3d11Device> d3d11DevicePtr;
