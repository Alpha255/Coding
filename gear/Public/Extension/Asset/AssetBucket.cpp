#include "AssetBucket.h"
#include "Gear/Public/Extension/System.h"
#include "Gear/Public/Independent/String.h"

namespaceStart(gear)

void assetBucket::initialize()
{
	std::string currentPath = getApplicationPath();
	m_AssetsPath = file::getFileDirectory(file::getFileDirectory(currentPath));
	m_AssetsPath += "\\Assets";
	verify(file::isDirectoryExists(m_AssetsPath));
}

std::shared_ptr<assetFile> assetBucket::getAsset(const std::string &assetName)
{
	assert(m_AssetsPath.length() > 0u);
	std::string lowerName(assetName);
	toLower(lowerName);

	auto it = m_Assets.find(lowerName);
	if (it != m_Assets.cend())
	{
		return it->second;
	}

	std::string assetPath = file::findFile(m_AssetsPath, assetName);
	if (assetPath.length() != 0u)
	{
		std::shared_ptr<assetFile> asset = std::make_shared<assetFile>(assetPath);
		m_Assets.insert(std::make_pair(lowerName, asset));
		return asset;
	}

	return nullptr;
}

namespaceEnd(gear)