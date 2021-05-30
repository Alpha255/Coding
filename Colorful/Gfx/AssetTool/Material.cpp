#include "Colorful/Gfx/AssetTool/Material.h"
#include "Colorful/Gfx/GfxDevice.h"

NAMESPACE_START(Gfx)

extern GfxDevice* s_GfxDevice;

Material::Material(const MaterialDesc& desc)
{
	setDebugName(desc.Name.c_str());

	/// safety???
#define CREATE_SHADER(Stage)                                                                                                                      \
	if (desc.ShaderDescs[EShaderStage::Stage].BinarySize)                                                                                         \
	{                                                                                                                                             \
		m_ShaderStages[EShaderStage::Stage] = s_GfxDevice->create##Stage##Shader(const_cast<ShaderDesc&>(desc.ShaderDescs[EShaderStage::Stage])); \
	}

	CREATE_SHADER(Vertex);
	CREATE_SHADER(Hull);
	CREATE_SHADER(Domain);
	CREATE_SHADER(Geometry);
	CREATE_SHADER(Fragment);
	CREATE_SHADER(Compute);
#undef CREATE_SHADER

	for (auto& tex : desc.TextureDescs)
	{
		m_Textures.emplace_back(std::make_pair(tex.first, s_GfxDevice->create2DTexture(tex.second)));
	}

	m_InputLayout = s_GfxDevice->createInputLayout(desc.InputLayoutDesc, desc.ShaderDescs[EShaderStage::Vertex]);
}

NAMESPACE_END(Gfx)