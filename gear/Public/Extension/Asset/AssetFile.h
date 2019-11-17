#pragma once

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
		matchType();
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
	void matchType();
private:
	eAssetType m_Type = eUnknown;
	uint32_t m_ID = UINT32_MAX;
};

namespaceEnd(gear)
