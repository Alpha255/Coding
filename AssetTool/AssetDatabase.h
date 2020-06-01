#pragma once

#include "Asset.h"

class GfxModel;

namespaceStart(AssetTool)

class exportAPI AssetDatabase : public Singleton<AssetDatabase>
{
public:
	void initialize();

	ShaderBinary tryToGetShaderBinary(Configurations::eRenderEngine engine, eRShaderUsage usage, const std::string& shaderName);

	TextureBinary tryToGetTextureBinary(Configurations::eRenderEngine engine, const std::string& texName);

	AssetPtr tryToLoadStaticMesh(const std::string& modelName, GfxModel& gfxModel);

	AssetPtr tryToGetAsset(const std::string& assetName);
protected:
	Asset::eAssetType tryToGetAssetType(const std::string& assetName);
};

namespaceEnd(AssetTool)
