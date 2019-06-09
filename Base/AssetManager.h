#pragma once

#include "AssetFile.h"

class AssetManager
{
public:
	static AssetManager &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<AssetManager>(new AssetManager);
		}
		return *s_Instance;
	}

	void Initialize();

	inline AssetFile Get(const std::string &assetName)
	{
		auto it = m_Assets.find(assetName);
		if (it != m_Assets.end())
		{
			return it->second;
		}

		assert(0);
		return AssetFile("");
	}
protected:
private:
	static std::unique_ptr<AssetManager> s_Instance;
	std::unordered_map<std::string, AssetFile> m_Assets;
};
