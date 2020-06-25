#include "D3D11Shader.h"
#include "D3D11Engine.h"

d3d11Shader::d3d11Shader(const D3D11Device &device, eRShaderUsage usage, const AssetTool::ShaderBinary &binary)
	: GfxShader(usage)
{
	assert(!isValid() && device.isValid());
	assert(binary.Binary && binary.Size > 0u && usage < eRShaderUsage_MaxEnum);

	switch (usage)
	{
	case eVertexShader:
	{
		ID3D11VertexShader *pShader = nullptr;
		GfxVerifyD3D(device->CreateVertexShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);

		m_ShaderBlob = binary;
	}
		break;
	case eHullShader:
	{
		ID3D11HullShader *pShader = nullptr;
		GfxVerifyD3D(device->CreateHullShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eDomainShader:
	{
		ID3D11DomainShader *pShader = nullptr;
		GfxVerifyD3D(device->CreateDomainShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eGeometryShader:
	{
		ID3D11GeometryShader *pShader = nullptr;
		GfxVerifyD3D(device->CreateGeometryShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eFragmentShader:
	{
		ID3D11PixelShader *pShader = nullptr;
		GfxVerifyD3D(device->CreatePixelShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eComputeShader:
	{
		ID3D11ComputeShader *pShader = nullptr;
		GfxVerifyD3D(device->CreateComputeShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	}
}

//void d3d11Shader::setInputLayout(const std::vector<GfxVertexAttributes> &vertexAttributes)
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
