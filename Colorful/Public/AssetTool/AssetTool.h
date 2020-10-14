#pragma once

#include "Gear/Asset.h"
#include "Colorful/Public/GfxResource.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(AssetTool)
class EXPORT_API AssetTool
{
public:
	static AssetTool& instance();

	void initialize(const std::string& baseDirectory);

	Gear::AssetPtr findAsset(const std::string& name);

	std::vector<uint32_t> loadShader(EShaderLanguage language, EShaderStage stage, const std::string& name);

	void loadTexture(const std::string& name);

	void loadModel(const std::string& name);

	void loadMaterial(const std::string& name);

	void finalize() {}
protected:
private:
	/// warning C4251
};

NAMESPACE_END(Gfx)
