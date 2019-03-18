#pragma once

#include "D3D11Object.h"
#include "Public/Geometry.h"

class D3D11InputLayout : public D3DObject<ID3D11InputLayout>
{
public:
	void Create(D3D11Blob &blob, const std::vector<Geometry::VertexLayout> &layouts);
};

class D3D11Shader
{
public:
	D3D11Shader(eRShaderType type)
		: m_Type(type)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) = 0;
protected:
	D3D11Blob CompileShaderCode(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude);
	eRShaderType m_Type = eRShaderTypeCount;
private:
};

class D3D11VertexShader : public D3DObject<ID3D11VertexShader>, public D3D11Shader
{
public:
	D3D11VertexShader()
		: D3D11Shader(eVertexShader)
	{
	}

	D3D11Blob &GetBlob()
	{
		return m_Blob;
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
protected:
	D3D11Blob m_Blob;
};

class D3D11HullShader : public D3DObject<ID3D11HullShader>, public D3D11Shader
{
public:
	D3D11HullShader()
		: D3D11Shader(eHullShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3D11DomainShader : public D3DObject<ID3D11DomainShader>, public D3D11Shader
{
public:
	D3D11DomainShader()
		: D3D11Shader(eDomainShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3D11GeometryShader : public D3DObject<ID3D11GeometryShader>, public D3D11Shader
{
public:
	D3D11GeometryShader()
		: D3D11Shader(eGeometryShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3D11PixelShader : public D3DObject<ID3D11PixelShader>, public D3D11Shader
{
public:
	D3D11PixelShader()
		: D3D11Shader(ePixelShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3D11ComputeShader : public D3DObject<ID3D11ComputeShader>, public D3D11Shader
{
public:
	D3D11ComputeShader()
		: D3D11Shader(eComputeShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};