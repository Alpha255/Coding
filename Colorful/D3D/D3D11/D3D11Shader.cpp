#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "AssetTool/AssetDatabase.h"

D3D11InputLayout::D3D11InputLayout(
	const D3D11Device& device, 
	const AssetTool::ShaderBinary& binary, 
	const std::vector<GfxVertexAttributes>& layout,
	uint32_t alignment)
{
	assert(binary.Size && layout.size());

	static constexpr char8_t* c_Semantic[eRVertexUsge_MaxEnum]
	{
		"POSITION",
		"NORMAL",
		"BINNORMAL",
		"TANGENT",
		"BITANGENT",
		"TEXCOORD",
		"COLOR"
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> descs(layout.size());
	for (uint32_t i = 0u; i < layout.size(); ++i)
	{
		descs[i] = D3D11_INPUT_ELEMENT_DESC
		{
			"TEXCOORD",
			i,
			D3D11Enum::toFormat(layout[i].Format),
			0u,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0u
		};
		m_Stride += (uint32_t)(GfxVertexAttributes::formatStride(layout[i].Format));
		m_Stride = (uint32_t)Gear::LinearAllocator::align_to(m_Stride, alignment);
	}

	ID3D11InputLayout* inputLayout = nullptr;
	GfxVerifyD3D(device->CreateInputLayout(descs.data(), (uint32_t)descs.size(), binary.Binary.get(), binary.Size, &inputLayout));

	reset(inputLayout);
}

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
