#pragma once

#include "AssetTool/Asset.h"

class D3D11InputLayout : public D3DObject<ID3D11InputLayout>
{
public:
	D3D11InputLayout(
		const class D3D11Device& device, 
		const AssetTool::ShaderBinary& binary, 
		const std::vector<GfxVertexAttributes>& layout,
		uint32_t alignment);

	inline uint32_t stride() const
	{
		return m_Stride;
	}
protected:
private:
	uint32_t m_Stride = 0u;
};
using D3D11InputLayoutPtr = std::shared_ptr<D3D11InputLayout>;

class D3D11Shader : public D3DObject<ID3D11DeviceChild>, public GfxShader
{
public:
	D3D11Shader(const class D3D11Device& device, eRShaderUsage usage, const std::string& shaderName);

	const AssetTool::ShaderBinary& binary() const
	{
		return m_Binary;
	}
protected:
private:
	AssetTool::ShaderBinary m_Binary;
};
using D3D11ShaderPtr = std::shared_ptr<D3D11Shader>;