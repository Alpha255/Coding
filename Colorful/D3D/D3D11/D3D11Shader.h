#pragma once

#include "AssetTool/Asset.h"

class d3d11InputLayout : public D3DObject<ID3D11InputLayout>
{
};

class d3d11Shader : public D3DObject<ID3D11DeviceChild>, public GfxShader
{
public:
	d3d11Shader(const class D3D11Device &device, eRShaderUsage usage, const AssetTool::ShaderBinary &binary);

	void setInputLayout(const std::vector<GfxVertexAttributes> &vertexAttributes);

	inline d3d11InputLayout getInputLayout() const
	{
		assert(m_Usage == eVertexShader && m_InputLayout.isValid());
		return m_InputLayout;
	}

	void allocResources() override final
	{

	}
protected:
private:
	d3d11InputLayout m_InputLayout;
	AssetTool::ShaderBinary m_ShaderBlob;
};