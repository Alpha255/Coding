#include "AssetBucket.h"
#include "Gear/Public/Extension/Log.h"
#include "Gear/Public/Extension/System.h"
#include "Gear/Public/Independent/String.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(gear)

static nlohmann::json s_AssetConfigJson;

void assetBucket::initialize()
{
	std::string currentPath = getApplicationPath();
	m_AssetsPath = file::getFileDirectory(file::getFileDirectory(currentPath));
	m_AssetsPath += "\\Assets";
	verify(file::isDirectoryExists(m_AssetsPath));

	std::string configPath(file::getFileDirectory(getApplicationPath()) + "\\appConfig.json");
	assert(file::isFileExists(configPath));
	std::ifstream filestream(configPath);
	filestream >> s_AssetConfigJson;
	filestream.close();
}

assetFile::eAssetType assetBucket::getAssetType(const std::string &assetName)
{
	auto assetFormatPtr = s_AssetConfigJson.find("assetFormat");
	assert(assetFormatPtr != s_AssetConfigJson.end());

	std::string ext = file::getFileExtension(assetName);
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

	uint32_t assetType = assetFile::eUnknown;
	for (uint32_t i = assetFile::eDDSTexture; i != assetFile::eAssetType_MaxEnum; ++i)
	{
		switch (i)
		{
		case assetFile::eDDSTexture:
			if (keyType == enumToString(eDDSTexture))
			{
				assetType = i;
			}
			break;
		case assetFile::eWICTexture:
			if (keyType == enumToString(eWICTexture))
			{
				assetType = i;
			}
			break;
		case assetFile::eVulkanTexture:
			if (keyType == enumToString(eVulkanTexture))
			{
				assetType = i;
			}
			break;
		case assetFile::eShader:
			if (keyType == enumToString(eShader))
			{
				assetType = i;
			}
			break;
		case assetFile::eStaticMesh:
			if (keyType == enumToString(eStaticMesh))
			{
				assetType = i;
			}
			break;
		case assetFile::eSound:
			if (keyType == enumToString(eSound))
			{
				assetType = i;
			}
			break;
		case assetFile::eLevel:
			if (keyType == enumToString(eLevel))
			{
				assetType = i;
			}
			break;
		}

		if (assetType != assetFile::eUnknown)
		{
			break;
		}
	}

	return (assetFile::eAssetType)assetType;
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

	std::string assetPath = file::findFile(m_AssetsPath, assetName);
	if (assetPath.length() != 0u)
	{
		assetFilePtr assetPtr = std::make_shared<assetFile>(assetPath, getAssetType(assetName), UINT32_MAX);
		m_Assets.insert(std::make_pair(lowerName, assetPtr));
		return assetPtr;
	}

	logger::instance().log(logger::eError, "Can't find file: \"%s\"", assetName.c_str());
	assert(0);
	return nullptr;
}

namespaceEnd(gear)