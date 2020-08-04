#pragma once

#include "Asset.h"

class GfxModel;
class GfxEngine;

namespaceStart(AssetTool)

class exportAPI AssetDatabase : public Singleton<AssetDatabase>
{
public:
	void initialize();

	ShaderBinary tryToGetShaderBinary(Configurations::eRenderEngine engine, eRShaderUsage usage, const std::string& shaderName);

	TextureBinary tryToGetTextureBinary(Configurations::eRenderEngine engine, const std::string& texName);

	void tryToLoadModel(const std::string& modelName, __out GfxModel& gfxModel, GfxEngine* gfxEngine);

	AssetPtr tryToGetAsset(const std::string& assetName);

	ID3D11ShaderResourceView* tryToLoadD3DTextureFromFile(ID3D11Device* device, const AssetPtr& asset);

	std::string assetBasePath() const;
protected:
	Asset::eAssetType tryToGetAssetType(const std::string& assetName);
};

namespaceEnd(AssetTool)
