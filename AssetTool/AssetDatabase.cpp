#include "AssetDatabase.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(AssetTool)

static nlohmann::json s_AssetConfigJson;

void AssetDatabase::initialize()
{
	auto initInOtherThread = [&]()
	{
		std::string currentPath = getApplicationPath();
		m_BasePath = File::directory(File::directory(currentPath));
		m_BasePath += "\\Assets";
		verify(File::isDirectoryExists(m_BasePath));

		std::string configPath(File::directory(getApplicationPath()) + "\\Configurations.json");
		assert(File::isExists(configPath));
		std::ifstream filestream(configPath);
		filestream >> s_AssetConfigJson;
		filestream.close();

		std::vector<std::string> filters;
		auto fileList = Gear::File::buildFileList(m_BasePath, filters, true);

		uint32_t index = 0;
		for each (auto &file in fileList)
		{
			auto asset = std::make_shared<Asset>(file, tryToGetAssetType(file), index++);
			m_Assets.insert(std::make_pair(asset->name(), asset));
		}
	};

	std::thread(initInOtherThread).detach();
}

Asset::eAssetType AssetDatabase::tryToGetAssetType(const std::string& assetName)
{
	auto assetFormatPtr = s_AssetConfigJson.find("assetFormat");
	assert(assetFormatPtr != s_AssetConfigJson.end());

	std::string ext = File::extension(assetName);
	std::string keyType;
	for (auto it = assetFormatPtr->cbegin(); it != assetFormatPtr->cend(); ++it)
	{
		assert(it->is_array());
		for (uint32_t i = 0u; i < it->size(); ++i)
		{
			if ((*it)[i] == ext)
			{
				keyType = it.key();
				break;
			}
		}

		if (keyType.size() > 0u)
		{
			break;
		}
	}

	uint32_t assetType = Asset::eUnknown;
	for (uint32_t i = Asset::eDDSTexture; i != Asset::eAssetType_MaxEnum; ++i)
	{
		switch (i)
		{
		case Asset::eDDSTexture:
			if (keyType == enumToString(eDDSTexture))
			{
				assetType = i;
			}
			break;
		case Asset::eWICTexture:
			if (keyType == enumToString(eWICTexture))
			{
				assetType = i;
			}
			break;
		case Asset::eVulkanTexture:
			if (keyType == enumToString(eVulkanTexture))
			{
				assetType = i;
			}
			break;
		case Asset::eShader:
			if (keyType == enumToString(eShader))
			{
				assetType = i;
			}
			break;
		case Asset::eStaticMesh:
			if (keyType == enumToString(eStaticMesh))
			{
				assetType = i;
			}
			break;
		case Asset::eSound:
			if (keyType == enumToString(eSound))
			{
				assetType = i;
			}
			break;
		case Asset::eLevel:
			if (keyType == enumToString(eLevel))
			{
				assetType = i;
			}
			break;
		}

		if (assetType != Asset::eUnknown)
		{
			break;
		}
	}

	return (Asset::eAssetType)assetType;
}

AssetPtr AssetDatabase::tryToGetAsset(const std::string& assetName)
{
	std::string lowerName(assetName);
	Gear::toLower(lowerName);

	auto it = m_Assets.find(lowerName);
	if (it != m_Assets.cend())
	{
		return it->second;
	}

	std::string assetPath = File::find(m_BasePath, assetName);
	if (assetPath.length() != 0u)
	{
		auto asset = std::make_shared<Asset>(assetPath, tryToGetAssetType(assetName), (uint32_t)(m_Assets.size() + 1u));
		m_Assets.insert(std::make_pair(lowerName, asset));
		return asset;
	}

	return nullptr;
}

namespaceEnd(AssetTool)
