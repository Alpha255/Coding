#pragma once

#include "Gear/Public/Extension/File.h"

namespaceStart(gear)

class assetFile : public file
{
public:
	enum eType
	{
		eUnknown,
		eTexture,
		eShader,
		eStaticMesh,
		eSound,
		eLevel
	};

	assetFile() = default;
	assetFile(const std::string &filePath)
		: file(filePath)
	{
		matchType();
	}

	inline eType getType() const
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
	eType m_Type = eUnknown;
	uint32_t m_ID = UINT32_MAX;
};

namespaceEnd(gear)
