#pragma once

#include "Gear/Public/Independent/Singleton.h"
#include "Gear/Public/Extension/File.h"

namespaceStart(gear)

class assetFile : public file
{
public:
	enum eAssetType
	{
		eUnknown,
		eDDSTexture,
		eWICTexture,
		eVulkanTexture,
		eShader,
		eStaticMesh,
		eSound,
		eLevel,
		eAssetType_MaxEnum
	};

	assetFile(const std::string &filePath, eAssetType type, uint32_t assetID)
		: file(filePath)
		, m_Type(type)
		, m_ID(assetID)
	{
	}

	inline eAssetType getType() const
	{
		return m_Type;
	}

	inline uint32_t getID() const
	{
		return m_ID;
	}
protected:
	friend class assetBucket;
private:
	eAssetType m_Type = eUnknown;
	uint32_t m_ID = UINT32_MAX;
};
typedef std::shared_ptr<assetFile> assetFilePtr;

class assetBucket
{
public:
	void initialize();
	assetFilePtr getAsset(const std::string &assetName);
protected:
	assetFile::eAssetType getAssetType(const std::string &assetName);
private:
	std::string m_AssetsPath;
	std::unordered_map<std::string, assetFilePtr> m_Assets;
};

namespaceEnd(gear)