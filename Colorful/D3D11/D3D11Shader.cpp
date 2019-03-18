#include "D3D11Shader.h"
#include "D3D11Engine.h"
#include "Base/AssetFile.h"

void D3D11InputLayout::Create(D3D11Blob &blob, const std::vector<Geometry::VertexLayout> &layouts)
{
	assert(blob.IsValid() && layouts.size() > 0U);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc;
	for each (auto &layout in layouts)
	{
		assert(layout.Semantic.length() > 0U);
		D3D11_INPUT_ELEMENT_DESC desc
		{
			layout.Semantic.c_str(),
			0U,
			(DXGI_FORMAT)layout.Format,
			0U,
			layout.Offset,
			D3D11_INPUT_PER_VERTEX_DATA,
			0U
		};
		inputDesc.emplace_back(desc);
	}

	ID3D11InputLayout *pInputLayout = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateInputLayout(inputDesc.data(), (uint32_t)layouts.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &pInputLayout));

	Reset(pInputLayout);
}

D3D11Blob D3D11Shader::CompileShaderCode(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(pEntryPoint && m_Type != eRShaderTypeCount);

	///std::string shaderCode = Base::GetShaderCode(pFileName, "#D3D11", m_Type);
	std::string shaderCode("");

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

	AssetFile shaderFile(pFileName);

#if defined(DEBUG) || defined(_DEBUG)
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	char workingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, workingDir);

	::SetCurrentDirectoryA(shaderFile.GetRoot().c_str());
	ID3DInclude* pIncludeInfo = (nullptr == pInclude ? D3D_COMPILE_STANDARD_FILE_INCLUDE : pInclude);
	if (FAILED(D3DCompile((const void *)shaderCode.c_str(), shaderCode.length(), pFileName, pMacros, pIncludeInfo, pEntryPoint, models[m_Type], flags, 0U, &pResult, &pErrMsg)))
	{
		Base::Log((const char*)pErrMsg->GetBufferPointer());
		assert(!"Shader compile failed!!!");
	}
	::SetCurrentDirectoryA(workingDir);

	errMsg.Reset(pErrMsg);
	result.Reset(pResult);

	return result;
}

void D3D11VertexShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	m_Blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11VertexShader *pVertexShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateVertexShader(m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(), nullptr, &pVertexShader));

	Reset(pVertexShader);
}

void D3D11HullShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11HullShader *pHullShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pHullShader));

	Reset(pHullShader);
}

void D3D11DomainShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11DomainShader *pDomainShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pDomainShader));

	Reset(pDomainShader);
}

void D3D11GeometryShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11GeometryShader *pGeometryShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pGeometryShader));

	Reset(pGeometryShader);
}

void D3D11PixelShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11PixelShader *pPixelShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pPixelShader));

	Reset(pPixelShader);
}

void D3D11ComputeShader::Create(const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(pFileName, pEntryPoint, pMacros, pInclude);

	ID3D11ComputeShader *pComputeShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pComputeShader));

	Reset(pComputeShader);
}