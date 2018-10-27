#include "D3DShader.h"
#include "D3DEngine.h"
#include "ResourceFile.h"

#include <d3dcompiler.h>

void D3DInputLayout::Create(D3DBlob &blob, const D3D11_INPUT_ELEMENT_DESC *pInputElement, size_t elementSize)
{
	assert(blob.IsValid() && pInputElement && elementSize);

	ID3D11InputLayout *pLayout = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateInputLayout(pInputElement, (uint32_t)elementSize, blob->GetBufferPointer(), blob->GetBufferSize(), &pLayout));

	MakeObject(pLayout);
}

D3DBlob D3DShader::CompileShaderFile(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(pFileName && pEntryPoint && m_Type != eShaderTypeCount);

	const char *const models[eShaderTypeCount] =
	{
		"vs_5_0",
		"hs_5_0",
		"ds_5_0",
		"gs_5_0",
		"ps_5_0",
		"cs_5_0"
	};

	D3DBlob result;
	D3DBlob errMsg;
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

void D3DVertexShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	m_Blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11VertexShader *pVertexShader = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateVertexShader(m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(), nullptr, &pVertexShader));

	MakeObject(pVertexShader);
}

void D3DHullShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3DBlob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11HullShader *pHullShader = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pHullShader));

	MakeObject(pHullShader);
}

void D3DDomainShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3DBlob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11DomainShader *pDomainShader = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pDomainShader));

	MakeObject(pDomainShader);
}

void D3DGeometryShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3DBlob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11GeometryShader *pGeometryShader = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pGeometryShader));

	MakeObject(pGeometryShader);
}

void D3DPixelShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3DBlob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11PixelShader *pPixelShader = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pPixelShader));

	MakeObject(pPixelShader);
}

void D3DComputeShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3DBlob blob = CompileShaderFile(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11ComputeShader *pComputeShader = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pComputeShader));

	MakeObject(pComputeShader);
}