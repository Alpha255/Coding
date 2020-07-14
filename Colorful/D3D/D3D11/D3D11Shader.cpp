#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "AssetTool/AssetDatabase.h"

D3D11Shader::D3D11Shader(const D3D11Device& device, eRShaderUsage usage, const std::string& shaderName)
	: GfxShader(usage)
{
	assert(!isValid() && device.isValid());
	assert(usage < eRShaderUsage_MaxEnum);

	m_Binary = AssetTool::AssetDatabase().instance().tryToGetShaderBinary(Configurations::eD3D11, usage, shaderName);
	assert(m_Binary.Size > 0ull);

	switch (usage)
	{
	case eVertexShader:
	{
		ID3D11VertexShader* shader = nullptr;
		GfxVerifyD3D(device->CreateVertexShader(m_Binary.Binary.get(), m_Binary.Size, nullptr, &shader));
		reset(shader);
	}
		break;
	case eHullShader:
	{
		ID3D11HullShader* shader = nullptr;
		GfxVerifyD3D(device->CreateHullShader(m_Binary.Binary.get(), m_Binary.Size, nullptr, &shader));
		reset(shader);
	}
		break;
	case eDomainShader:
	{
		ID3D11DomainShader* shader = nullptr;
		GfxVerifyD3D(device->CreateDomainShader(m_Binary.Binary.get(), m_Binary.Size, nullptr, &shader));
		reset(shader);
	}
		break;
	case eGeometryShader:
	{
		ID3D11GeometryShader* shader = nullptr;
		GfxVerifyD3D(device->CreateGeometryShader(m_Binary.Binary.get(), m_Binary.Size, nullptr, &shader));
		reset(shader);
	}
		break;
	case eFragmentShader:
	{
		ID3D11PixelShader* shader = nullptr;
		GfxVerifyD3D(device->CreatePixelShader(m_Binary.Binary.get(), m_Binary.Size, nullptr, &shader));
		reset(shader);
	}
		break;
	case eComputeShader:
	{
		ID3D11ComputeShader* shader = nullptr;
		GfxVerifyD3D(device->CreateComputeShader(m_Binary.Binary.get(), m_Binary.Size, nullptr, &shader));
		reset(shader);
	}
		break;
	}
}

//void D3D11Shader::setInputLayout(const std::vector<GfxVertexAttributes> &vertexAttributes)
//{
//	assert(isValid() && m_Usage == eVertexShader);
//	assert(!m_InputLayout.isValid());
//
//	static constexpr char8_t *c_Semantic[eRVertexUsge_MaxEnum]
//	{
//		"POSITION",
//		"NORMAL",
//		"BINNORMAL",
//		"TANGENT",
//		"BITANGENT",
//		"TEXCOORD",
//		"COLOR"
//	};
//
//	size_t vertexStride = 0u;
//	std::vector<D3D11_INPUT_ELEMENT_DESC> descs(vertexAttributes.size());
//	for (uint32_t i = 0u; i < vertexAttributes.size(); ++i)
//	{
//		descs[i] = D3D11_INPUT_ELEMENT_DESC
//		{
//			c_Semantic[vertexAttributes[i].Usage],
//			0u,
//			D3D11Enum::toFormat(vertexAttributes[i].Format),
//			0u,
//			D3D11_APPEND_ALIGNED_ELEMENT,
//			D3D11_INPUT_PER_VERTEX_DATA,
//			0u
//		};
//		vertexStride += GfxVertexAttributes::formatStride(vertexAttributes[i].Format);
//	}
//
//	ID3D11InputLayout *pLayout = nullptr;
//	GfxVerifyD3D(D3D11Engine::instance().getDevice()->CreateInputLayout(
//		descs.data(), 
//		(uint32_t)descs.size(), 
//		m_ShaderBlob.Binary.get(), 
//		m_ShaderBlob.Size, &pLayout));
//	reset(pLayout);
//
//	m_ShaderBlob.Binary.reset();
//	m_ShaderBlob.Size = 0ull;
//}
