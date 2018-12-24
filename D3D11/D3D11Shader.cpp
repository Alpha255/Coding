#include "D3D11Shader.h"
#include "D3D11Engine.h"
#include "Util/ResourceFile.h"

#include <d3dcompiler.h>

void D3D11InputLayout::Create(D3D11Blob &blob, const D3D11_INPUT_ELEMENT_DESC *pInputElement, size_t elementSize)
{
	assert(blob.IsValid() && pInputElement && elementSize);

	ID3D11InputLayout *pLayout = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateInputLayout(pInputElement, (uint32_t)elementSize, blob->GetBufferPointer(), blob->GetBufferSize(), &pLayout));

	MakeObject(pLayout);
}

D3D11Blob D3D11Shader::CompileShaderFile(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(pFileName && pEntryPoint && m_Type != eRShaderTypeCount);

	const char *const models[eRShaderTypeCount] =
	{
		"vs_5_0",
		"hs_5_0",
		"ds_5_0",
		"gs_5_0",
		"ps_5_0",
		"cs_5_0"
	};

	D3D11Blob result;
	D3D11Blob errMsg;
	ID3DBlob *pResult = nullptr;
	ID3DBlob *pErrMsg = nullptr;

	ResourceFile shaderFile(pFileName);

#if defined(DEBUG) || defined(_DEBUG)
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	std::string fileDir = shaderFile.GetFilePath(); 
	fileDir = fileDir.substr(0, fileDir.rfind('\\'));
	auto pData = std::unique_ptr<uint8_t>(shaderFile.Load());

	char workingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, workingDir);

	::SetCurrentDirectoryA(fileDir.c_str());
	ID3DInclude* pIncludeInfo = (nullptr == pInclude ? D3D_COMPILE_STANDARD_FILE_INCLUDE : pInclude);
	if (FAILED(D3DCompile((void *)pData.get(), shaderFile.GetSize(), pFileName, pMacros, pIncludeInfo, pEntryPoint, models[m_Type], flags, 0U, &pResult, &pErrMsg)))
	{
		OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
		assert(!"Shader compile failed!!!");
	}
	::SetCurrentDirectoryA(workingDir);

	errMsg.MakeObject(pErrMsg);
	result.MakeObject(pResult);

	return result;
}

void D3D11VertexShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	m_Blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11VertexShader *pVertexShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateVertexShader(m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(), nullptr, &pVertexShader));

	MakeObject(pVertexShader);
}

void D3D11HullShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11HullShader *pHullShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pHullShader));

	MakeObject(pHullShader);
}

void D3D11DomainShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11DomainShader *pDomainShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pDomainShader));

	MakeObject(pDomainShader);
}

void D3D11GeometryShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11GeometryShader *pGeometryShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pGeometryShader));

	MakeObject(pGeometryShader);
}

void D3D11PixelShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11PixelShader *pPixelShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pPixelShader));

	MakeObject(pPixelShader);
}

void D3D11ComputeShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11ComputeShader *pComputeShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pComputeShader));

	MakeObject(pComputeShader);
}