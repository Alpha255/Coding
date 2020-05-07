#pragma once

#include "Gear/Singleton.h"
#include "Gear/File.h"

namespaceStart(Gear)

class AssetFile : public File
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

	AssetFile(const std::string &filePath, eAssetType type, uint32_t assetID)
		: File(filePath)
		, m_Type(type)
		, m_ID(assetID)
	{
	}

	inline eAssetType type() const
	{
		return m_Type;
	}

	inline uint32_t ID() const
	{
		return m_ID;
	}
protected:
	friend class assetBucket;
private:
	eAssetType m_Type = eUnknown;
	uint32_t m_ID = std::numeric_limits<uint32_t>().max();
};
typedef std::shared_ptr<AssetFile> assetFilePtr;

class assetBucket
{
public:
	void initialize();
	assetFilePtr getAsset(const std::string &assetName);
protected:
	AssetFile::eAssetType getAssetType(const std::string &assetName);
private:
	std::string m_AssetsPath;
	std::unordered_map<std::string, assetFilePtr> m_Assets;
};

namespaceEnd(Gear)
