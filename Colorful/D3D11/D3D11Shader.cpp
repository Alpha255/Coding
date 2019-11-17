#include "D3D11Shader.h"
#include "D3D11Engine.h"
#include "Base/AssetFile.h"

void D3D11InputLayout::Create(const void *pByteCode, const size_t byteSize, const std::vector<Geometry::VertexLayout> &layouts)
{
	assert(pByteCode && byteSize > 0U);

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
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0U
		};
		inputDesc.emplace_back(desc);

		m_VertexStride += (uint32_t)layout.Stride;
	}

	ID3D11InputLayout *pInputLayout = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateInputLayout(inputDesc.data(), (uint32_t)layouts.size(), pByteCode, byteSize, &pInputLayout));

	Reset(pInputLayout);
}

D3D11Blob D3D11Shader::CompileShaderCode(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(m_Type != eRShaderTypeCount);

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

	AssetFile shaderFile(fileName);
	auto shaderCode = shaderFile.Load();

#if defined(DEBUG) || defined(_DEBUG)
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	char workingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, workingDir);

	::SetCurrentDirectoryA(shaderFile.GetRoot().c_str());
	ID3DInclude* pIncludeInfo = (nullptr == pInclude ? D3D_COMPILE_STANDARD_FILE_INCLUDE : pInclude);
	if (FAILED(D3DCompile(
		(const void *)shaderCode.get(), 
		shaderFile.GetSize(),
		fileName.c_str(), 
		pMacros, 
		pIncludeInfo, 
		entryPoint.c_str(), 
		models[m_Type], 
		flags, 0U, 
		&pResult, 
		&pErrMsg)))
	{
		Base::Log((const char*)pErrMsg->GetBufferPointer());
		assert(!"Shader compile failed!!!");
	}
	::SetCurrentDirectoryA(workingDir);

	errMsg.Reset(pErrMsg);
	result.Reset(pResult);

	return result;
}

void D3D11VertexShader::Create(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	m_Blob = CompileShaderCode(fileName, entryPoint, pMacros, pInclude);

	ID3D11VertexShader *pVertexShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateVertexShader(m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(), nullptr, &pVertexShader));

	Reset(pVertexShader);
}

void D3D11HullShader::Create(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(fileName, entryPoint, pMacros, pInclude);

	ID3D11HullShader *pHullShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pHullShader));

	Reset(pHullShader);
}

void D3D11DomainShader::Create(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(fileName, entryPoint, pMacros, pInclude);

	ID3D11DomainShader *pDomainShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pDomainShader));

	Reset(pDomainShader);
}

void D3D11GeometryShader::Create(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(fileName, entryPoint, pMacros, pInclude);

	ID3D11GeometryShader *pGeometryShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pGeometryShader));

	Reset(pGeometryShader);
}

void D3D11PixelShader::Create(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(fileName, entryPoint, pMacros, pInclude);

	ID3D11PixelShader *pPixelShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pPixelShader));

	Reset(pPixelShader);
}

void D3D11ComputeShader::Create(const std::string &fileName, const std::string &entryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	assert(!IsValid());

	D3D11Blob blob = CompileShaderCode(fileName, entryPoint, pMacros, pInclude);

	ID3D11ComputeShader *pComputeShader = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pComputeShader));

	Reset(pComputeShader);
}