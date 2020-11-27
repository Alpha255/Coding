#pragma once

#include "Gear/Asset.h"
#include "Colorful/Public/GfxModel.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(AssetTool)
class EXPORT_API AssetTool
{
public:
	static AssetTool& instance();

	void initialize(const std::string& baseDirectory);

	Gear::AssetPtr findAsset(const std::string& name);

	ShaderDesc loadShader(EShaderLanguage language, EShaderStage stage, const std::string& name);

	ShaderDesc compileShader(EShaderLanguage language, EShaderStage stage, const char8_t* const code);

	TextureDesc loadTexture(const std::string& name, bool8_t sRGB);

	ModelDesc loadModel(const std::string& name, const ModelOptions& options = ModelOptions());

	void loadMaterial(const std::string& name);

	void finalize() {}
protected:
private:
	std::string m_AssetDirectory;
	/// warning C4251
};

NAMESPACE_END(Gfx)
