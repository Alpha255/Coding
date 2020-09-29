#if 0
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
#endif
