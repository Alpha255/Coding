#include "D3D11Shader.h"
#include "D3D11Engine.h"

d3d11Shader::d3d11Shader(const d3d11Device &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary)
	: GfxShader(usage)
{
	assert(!isValid() && device.isValid());
	assert(binary.Binary && binary.Size > 0u && usage < eRShaderUsage_MaxEnum);

	switch (usage)
	{
	case eVertexShader:
	{
		ID3D11VertexShader *pShader = nullptr;
		rVerifyD3D11(device->CreateVertexShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);

		m_ShaderBlob = binary;
	}
		break;
	case eHullShader:
	{
		ID3D11HullShader *pShader = nullptr;
		rVerifyD3D11(device->CreateHullShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eDomainShader:
	{
		ID3D11DomainShader *pShader = nullptr;
		rVerifyD3D11(device->CreateDomainShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eGeometryShader:
	{
		ID3D11GeometryShader *pShader = nullptr;
		rVerifyD3D11(device->CreateGeometryShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eFragmentShader:
	{
		ID3D11PixelShader *pShader = nullptr;
		rVerifyD3D11(device->CreatePixelShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eComputeShader:
	{
		ID3D11ComputeShader *pShader = nullptr;
		rVerifyD3D11(device->CreateComputeShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	}
}

void d3d11Shader::setInputLayout(const std::vector<GfxVertexAttributes> &vertexAttributes)
{
	assert(isValid() && m_Usage == eVertexShader);
	assert(!m_InputLayout.isValid());

	static constexpr char8_t *c_Semantic[eRVertexUsge_MaxEnum]
	{
		"POSITION",
		"NORMAL",
		"BINNORMAL",
		"TANGENT",
		"TEXCOORD",
		"COLOR"
	};

	size_t vertexStride = 0u;
	std::vector<D3D11_INPUT_ELEMENT_DESC> descs(vertexAttributes.size());
	for (uint32_t i = 0u; i < vertexAttributes.size(); ++i)
	{
		assert(vertexAttributes[i].InputRate < eRInputRate_MaxEnum);
		descs[i] = D3D11_INPUT_ELEMENT_DESC
		{
			c_Semantic[vertexAttributes[i].Usage],
			0u,
			d3d11Engine::enumTranslator::toFormat(vertexAttributes[i].Format),
			0u,
			D3D11_APPEND_ALIGNED_ELEMENT,
			vertexAttributes[i].InputRate == eVertex ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA,
			0u
		};
		vertexStride += GfxVertexAttributes::getFormatStride(vertexAttributes[i].Format);
	}

	ID3D11InputLayout *pLayout = nullptr;
	rVerifyD3D11(d3d11Engine::instance().getDevice()->CreateInputLayout(
		descs.data(), 
		(uint32_t)descs.size(), 
		m_ShaderBlob.Binary.get(), 
		m_ShaderBlob.Size, &pLayout));
	reset(pLayout);

	m_ShaderBlob.Binary.reset();
	m_ShaderBlob.Size = 0ull;
}
