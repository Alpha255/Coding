#include "AssetBucket.h"

namespaceStart(gear)

void assetBucket::initialize()
{
	char8_t currentPath[MAX_PATH] = {};
	verifyWin(::GetModuleFileNameA(nullptr, currentPath, MAX_PATH) != 0);

	m_AssetsPath = gear::file::getDirectory(gear::file::getDirectory(currentPath));
	m_AssetsPath += "\\Assets";
	verify(gear::isValidDirectory(m_AssetsPath));
}

std::shared_ptr<gear::assetFile> assetBucket::getAsset(const std::string &assetName)
{
	assert(m_AssetsPath.length() > 0u);
	std::string lowerName(assetName);
	gear::toLower(lowerName);

	auto it = m_Assets.find(lowerName);
	if (it != m_Assets.cend())
	{
		return it->second;
	}

	std::string assetPath = gear::findFile(m_AssetsPath, assetName);
	if (assetPath.length() != 0u)
	{
		std::shared_ptr<gear::assetFile> asset = std::make_shared<gear::assetFile>(assetPath);
		m_Assets.insert(std::make_pair(lowerName, asset));
		return asset;
	}

	return nullptr;
}

namespaceEnd(gear)