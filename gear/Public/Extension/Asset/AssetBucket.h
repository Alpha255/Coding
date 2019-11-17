#pragma once

#include "Gear/Public/Independent/Singleton.h"
#include "Gear/Public/Extension/Asset/AssetFile.h"

namespaceStart(gear)

class assetBucket final : public singleton<assetBucket>
{
	singletonDeclare(assetBucket);
public:
	void initialize();
	std::shared_ptr<assetFile> getAsset(const std::string &assetName);
private:
	std::string m_AssetsPath;
	std::unordered_map<std::string, std::shared_ptr<assetFile>> m_Assets;
};

namespaceEnd(gear)