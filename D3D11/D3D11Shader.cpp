#include "D3D11Shader.h"
#include "D3D11Engine.h"
#include "Util/ResourceFile.h"
#include "Util/System.h"

#include <d3dcompiler.h>

void D3D11InputLayout::Create(D3D11Blob &blob, const VertexLayout *pVertexLayout, size_t layoutCount)
{
	assert(blob.IsValid() && pVertexLayout && layoutCount);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc;
	const VertexLayout *pLayout = pVertexLayout;
	for (uint32_t i = 0U; i < layoutCount; ++i)
	{
		assert(pLayout->Semantic);
		D3D11_INPUT_ELEMENT_DESC desc
		{
			pLayout->Semantic,
			0U,
			(DXGI_FORMAT)pLayout->Format,
			0U,
			pLayout->Offset,
			D3D11_INPUT_PER_VERTEX_DATA,
			0U
		};
		inputDesc.emplace_back(desc);
		
		++pLayout;
	}

	ID3D11InputLayout *pInputLayout = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateInputLayout(inputDesc.data(), (uint32_t)layoutCount, blob->GetBufferPointer(), blob->GetBufferSize(), &pInputLayout));

	MakeObject(pInputLayout);
}

D3D11Blob D3D11Shader::CompileShaderCode(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(pEntryPoint && m_Type != eRShaderTypeCount);

	std::string shaderCode = System::GetShaderCode(pFileName, "#D3D11", m_Type);

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

	char workingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, workingDir);

	::SetCurrentDirectoryA(fileDir.c_str());
	ID3DInclude* pIncludeInfo = (nullptr == pInclude ? D3D_COMPILE_STANDARD_FILE_INCLUDE : pInclude);
	if (FAILED(D3DCompile((const void *)shaderCode.c_str(), shaderCode.length(), pFileName, pMacros, pIncludeInfo, pEntryPoint, models[m_Type], flags, 0U, &pResult, &pErrMsg)))
	{
		System::Log((const char*)pErrMsg->GetBufferPointer());
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

	m_Blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11VertexShader *pVertexShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateVertexShader(m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(), nullptr, &pVertexShader));

	MakeObject(pVertexShader);
}

void D3D11HullShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11HullShader *pHullShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pHullShader));

	MakeObject(pHullShader);
}

void D3D11DomainShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11DomainShader *pDomainShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pDomainShader));

	MakeObject(pDomainShader);
}

void D3D11GeometryShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11GeometryShader *pGeometryShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pGeometryShader));

	MakeObject(pGeometryShader);
}

void D3D11PixelShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11PixelShader *pPixelShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pPixelShader));

	MakeObject(pPixelShader);
}

void D3D11ComputeShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11ComputeShader *pComputeShader = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pComputeShader));

	MakeObject(pComputeShader);
}