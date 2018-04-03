#pragma once

#include "D3DObject.h"

class D3DBlob : public D3DObject<ID3DBlob> {};

class D3DInputLayout : public D3DObject<ID3D11InputLayout>
{
public:
	void Create(D3DBlob &blob, const D3D11_INPUT_ELEMENT_DESC *pInputElement, size_t elementSize);
};

class D3DShader
{
public:
	enum eShaderType
	{
		eVertexShader,
		eHullShader,
		eDomainShader,
		eGeometryShader,
		ePixelShader,
		eComputeShader,
		eShaderTypeCount
	};

	D3DShader(eShaderType type)
		: m_Type(type)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) = 0;

	D3DBlob CompileShaderFile(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude);
protected:
	eShaderType m_Type = eShaderTypeCount;
private:
};

class D3DVertexShader : public D3DObject<ID3D11VertexShader>, public D3DShader
{
public:
	D3DVertexShader()
		: D3DShader(eVertexShader)
	{
	}

	D3DBlob &GetBlob()
	{
		return m_Blob;
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
protected:
	D3DBlob m_Blob;
};

class D3DHullShader : public D3DObject<ID3D11HullShader>, public D3DShader
{
public:
	D3DHullShader()
		: D3DShader(eHullShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3DDomainShader : public D3DObject<ID3D11DomainShader>, public D3DShader
{
public:
	D3DDomainShader()
		: D3DShader(eDomainShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3DGeometryShader : public D3DObject<ID3D11GeometryShader>, public D3DShader
{
public:
	D3DGeometryShader()
		: D3DShader(eGeometryShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3DPixelShader : public D3DObject<ID3D11PixelShader>, public D3DShader
{
public:
	D3DPixelShader()
		: D3DShader(ePixelShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};

class D3DComputeShader : public D3DObject<ID3D11ComputeShader>, public D3DShader
{
public:
	D3DComputeShader()
		: D3DShader(eComputeShader)
	{
	}

	virtual void Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros = nullptr, ID3DInclude *pInclude = nullptr) override;
};