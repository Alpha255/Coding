#pragma once

#include "gear/Asset/AssetFile.h"

class assetBucket final : public gear::singleton<assetBucket>
{
	singletonDeclare(assetBucket);
public:
	void initialize();
	std::shared_ptr<gear::assetFile> getAsset(const std::string &assetName);
private:
	std::string m_AssetsPath;
	std::unordered_map<std::string, std::shared_ptr<gear::assetFile>> m_Assets;
};