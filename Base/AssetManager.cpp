#include "AssetManager.h"

std::unique_ptr<AssetManager> AssetManager::s_Instance;

void AssetManager::Initialize()
{
	char curPath[MAX_PATH] = {};
	Verify(::GetCurrentDirectoryA(MAX_PATH, curPath) != 0);

	std::string assetPath = Base::GetRootDirectory(curPath);
	assetPath += "\\Assets";

	std::vector<std::string> assetList;
	Base::BuildFileList(assetList, assetPath, std::vector<std::string>{}, true);

	for (auto it = assetList.cbegin(); it != assetList.cend(); ++it)
	{
		AssetFile asset(*it);
		if (AssetFile::eTypeCount != asset.GetType())
		{
			m_Assets.insert(std::make_pair(asset.GetName(), asset));
		}
	}
}