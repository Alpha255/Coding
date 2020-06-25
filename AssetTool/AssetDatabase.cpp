#include "AssetDatabase.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(AssetTool)

static nlohmann::json s_AssetConfigJson;
static std::string s_BasePath;
static std::unordered_map<std::string, AssetPtr> s_Assets;
static bool8_t s_Ready = false;

void AssetDatabase::initialize()
{
	auto initInOtherThread = [this]()
	{
		Gear::CpuTimer timer;
		timer.start();
		std::string currentPath = getApplicationPath();
		s_BasePath = File::directory(File::directory(currentPath));
		s_BasePath += "\\Assets";
		verify(File::isDirectoryExists(s_BasePath));

		std::string configPath(File::directory(getApplicationPath()) + "\\Configurations.json");
		assert(File::isExists(configPath));
		std::ifstream filestream(configPath);
		filestream >> s_AssetConfigJson;
		filestream.close();

		std::vector<std::string> filters;
		auto fileList = Gear::File::buildFileList(s_BasePath, filters, true);

		uint32_t index = 0;
		for each (auto &file in fileList)
		{
			auto asset = std::make_shared<Asset>(file, tryToGetAssetType(file), index++);
			s_Assets.insert(std::make_pair(asset->name(), asset));
		}
		s_Ready = true;

		timer.stop();
		Logger::instance().log(Logger::eInfo, format("Find all assets takes %.2fs", timer.elapsedTime()));
	};
#if 0
	std::thread(initInOtherThread).detach();
#else
	initInOtherThread();
#endif
}

std::string AssetDatabase::assetBasePath() const
{
	return s_BasePath;
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
		case Asset::eModel:
			if (keyType == enumToString(eModel))
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
		case Asset::eMaterial:
			if (keyType == enumToString(eMaterial))
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

	auto it = s_Assets.find(lowerName);
	if (it != s_Assets.cend())
	{
		return it->second;
	}

	std::string assetPath = File::find(s_BasePath, assetName);
	if (assetPath.length() != 0u)
	{
		auto asset = std::make_shared<Asset>(assetPath, tryToGetAssetType(assetName), (uint32_t)(s_Assets.size() + 1u));
		s_Assets.insert(std::make_pair(lowerName, asset));
		return asset;
	}

	return nullptr;
}

namespaceEnd(AssetTool)
