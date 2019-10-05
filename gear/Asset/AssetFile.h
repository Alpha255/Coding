#pragma once

#include "gear/System/FileIO.h"

namespaceStart(gear)

class assetFile : public fileIO
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
		: fileIO(filePath)
	{
		getType();
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
	void getType();
private:
	eType m_Type = eUnknown;
	uint32_t m_ID = UINT32_MAX;
};

namespaceEnd(gear)
