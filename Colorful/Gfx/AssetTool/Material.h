#pragma once

#include "Colorful/Gfx/GfxResource.h"

NAMESPACE_START(Gfx)

struct MaterialDesc
{
	std::string Name;
	VertexInputLayoutDesc InputLayoutDesc;
	std::array<ShaderDesc, EShaderStage::ShaderStageCount> ShaderDescs;
	std::vector<std::pair<uint32_t, TextureDesc>> TextureDescs;
};

class Material : public GfxResource
{
public:
	Material(const MaterialDesc& desc);

	InputLayoutPtr inputLayout() const
	{
		return m_InputLayout;
	}

	const PipelineShaderStages& pipelineShaderStages() const
	{
		return m_ShaderStages;
	}

	const std::vector<std::pair<uint32_t, TexturePtr>> textures() const
	{
		return m_Textures;
	}
protected:
private:
	InputLayoutPtr m_InputLayout = nullptr;
	PipelineShaderStages m_ShaderStages;
	std::vector<std::pair<uint32_t, TexturePtr>> m_Textures;
};
DECLARE_SHARED_PTR(Material)

NAMESPACE_END(Gfx)