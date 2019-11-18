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
		eTexture,
		eShader,
		eStaticMesh,
		eSound,
		eLevel,
		eAssetType_MaxEnum
	};

	assetFile() = default;
	assetFile(const std::string &filePath)
		: file(filePath)
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
	void initAsset(assetFilePtr &assetPtr);
private:
	std::string m_AssetsPath;
	std::unordered_map<std::string, assetFilePtr> m_Assets;
};

namespaceEnd(gear)