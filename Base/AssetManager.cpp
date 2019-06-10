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
		m_Assets.insert(std::make_pair(Base::GetFileName(*it), *it));
	}
}

void AssetManager::Finalize()
{
	m_Assets.clear();
}