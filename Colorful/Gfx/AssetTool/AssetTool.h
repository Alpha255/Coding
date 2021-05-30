#pragma once

#include "Gear/Asset.h"
#include "Colorful/Gfx/AssetTool/Material.h"
#include "Colorful/Gfx/AssetTool/Model.h"

NAMESPACE_START(Gfx)

class EXPORT_API AssetTool
{
public:
	static AssetTool& instance();

	void initialize(const std::string& baseDirectory, class GfxDevice* gfxDevice);

	Gear::AssetPtr findAsset(const char8_t* name);

	ShaderPtr loadShader(EShaderLanguage language, EShaderStage stage, const char8_t* name);
	ShaderDesc compileShader(EShaderLanguage language, EShaderStage stage, const char8_t* const code);

	TexturePtr loadTexture(const char8_t* name, bool8_t sRGB);

	ModelPtr loadModel(const char8_t* name, const Model::LoadingSettings& options = Model::LoadingSettings());

	MaterialPtr loadMaterial(const char8_t* name);

	void finalize();
};
DECLARE_UNIQUE_PTR(AssetTool)

NAMESPACE_END(Gfx)
