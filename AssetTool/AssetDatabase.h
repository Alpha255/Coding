#pragma once

#include "Asset.h"

namespaceStart(AssetTool)

exportAPI class AssetDatabase : public Singleton<AssetDatabase>
{
public:
	void initialize();

	ShaderBinary tryTogetShaderBinary(Configurations::eRenderEngine engine, eRShaderUsage usage, const std::string& shaderName);

	TextureBinary tryToGetTextureBinary(Configurations::eRenderEngine engine, const std::string& texName);

	AssetPtr tryToGetAsset(const std::string& assetName);

	Asset::eAssetType tryToGetAssetType(const std::string& assetName);
protected:
private:
	std::string m_BasePath;
	std::unordered_map<std::string, AssetPtr> m_Assets;
	bool8_t m_Ready = false;
};

namespaceEnd(AssetTool)
