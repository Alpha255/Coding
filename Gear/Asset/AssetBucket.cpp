#include "Gear/Gear.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(Gear)

static nlohmann::json s_AssetConfigJson;

void assetBucket::initialize()
{
	std::string currentPath = getApplicationPath();
	m_AssetsPath = File::directory(File::directory(currentPath));
	m_AssetsPath += "\\Assets";
	verify(File::isDirectoryExists(m_AssetsPath));

	std::string configPath(File::directory(getApplicationPath()) + "\\Configurations.json");
	assert(File::isExists(configPath));
	std::ifstream filestream(configPath);
	filestream >> s_AssetConfigJson;
	filestream.close();
}

AssetFile::eAssetType assetBucket::getAssetType(const std::string &assetName)
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

	uint32_t assetType = AssetFile::eUnknown;
	for (uint32_t i = AssetFile::eDDSTexture; i != AssetFile::eAssetType_MaxEnum; ++i)
	{
		switch (i)
		{
		case AssetFile::eDDSTexture:
			if (keyType == enumToString(eDDSTexture))
			{
				assetType = i;
			}
			break;
		case AssetFile::eWICTexture:
			if (keyType == enumToString(eWICTexture))
			{
				assetType = i;
			}
			break;
		case AssetFile::eVulkanTexture:
			if (keyType == enumToString(eVulkanTexture))
			{
				assetType = i;
			}
			break;
		case AssetFile::eShader:
			if (keyType == enumToString(eShader))
			{
				assetType = i;
			}
			break;
		case AssetFile::eStaticMesh:
			if (keyType == enumToString(eStaticMesh))
			{
				assetType = i;
			}
			break;
		case AssetFile::eSound:
			if (keyType == enumToString(eSound))
			{
				assetType = i;
			}
			break;
		case AssetFile::eLevel:
			if (keyType == enumToString(eLevel))
			{
				assetType = i;
			}
			break;
		}

		if (assetType != AssetFile::eUnknown)
		{
			break;
		}
	}

	return (AssetFile::eAssetType)assetType;
}

assetFilePtr assetBucket::getAsset(const std::string &assetName)
{
	assert(m_AssetsPath.length() > 0u);
	std::string lowerName(assetName);
	toLower(lowerName);

	auto it = m_Assets.find(lowerName);
	if (it != m_Assets.cend())
	{
		return it->second;
	}

	std::string assetPath = File::find(m_AssetsPath, assetName);
	if (assetPath.length() != 0u)
	{
		assetFilePtr assetPtr = std::make_shared<AssetFile>(assetPath, getAssetType(assetName), UINT32_MAX);
		m_Assets.insert(std::make_pair(lowerName, assetPtr));
		return assetPtr;
	}

	Logger::instance().log(Logger::eError, "Can't find file: \"%s\"", assetName.c_str());
	assert(0);
	return nullptr;
}

namespaceEnd(Gear)
