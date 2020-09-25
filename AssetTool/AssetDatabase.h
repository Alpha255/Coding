#pragma once

#include "Gear/Asset.h"
#include "Colorful/Public/GfxResource.h"

NAMESPACE_START(Tool)

DECLARE_UNIQUE_PTR(AssetDatabase)
class EXPORT_API AssetDatabase
{
public:
	static AssetDatabase& instance();

	void initialize(const std::string& baseDirectory);

	Gear::AssetPtr findAsset(const std::string& name);

	std::vector<byte8_t> loadShader(Gfx::EShaderLanguage language, Gfx::EShaderStage stage, const std::string& name);

	void loadTexture(const std::string& name);

	void loadModel(const std::string& name);

	void finalize() {}
protected:
private:
};

NAMESPACE_END(Tool)
