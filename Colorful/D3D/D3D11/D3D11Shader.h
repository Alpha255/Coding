#pragma once

#include "AssetTool/Asset.h"

class D3D11InputLayout : public D3DObject<ID3D11InputLayout>
{
};

class D3D11Shader : public D3DObject<ID3D11DeviceChild>, public GfxShader
{
public:
	D3D11Shader(const class D3D11Device& device, eRShaderUsage usage, const std::string& shaderName);
protected:
private:
	AssetTool::ShaderBinary m_Binary;
};