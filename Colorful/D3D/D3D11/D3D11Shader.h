#pragma once

#include "Colorful/Public/RAsset.h"

class d3d11InputLayout : public D3DObject<ID3D11InputLayout>
{
};

class d3d11Shader : public D3DObject<ID3D11DeviceChild>, public rShader
{
public:
	d3d11Shader(const class d3d11Device &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary);

	void setInputLayout(const std::vector<rVertexAttributes> &vertexAttributes);

	inline d3d11InputLayout getInputLayout() const
	{
		assert(m_Usage == eVertexShader && m_InputLayout.isValid());
		return m_InputLayout;
	}
protected:
private:
	d3d11InputLayout m_InputLayout;
	rAsset::rShaderBinary m_ShaderBlob;
};