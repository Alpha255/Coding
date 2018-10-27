#pragma once

#include "Common.h"

class ResourceFile
{
public:
	enum eType
	{
		eShader,
		eTexture,
		eSDKMesh,
		eTxtMesh,
		eObjMesh,
		eSound,
		eLevelData,
		eTypeCount
	};

	ResourceFile(const char *pFileName);
	~ResourceFile() = default;

	inline bool IsValid() const
	{
		return m_Type != eTypeCount;
	}

	inline size_t GetSize() const
	{
		assert(IsValid());
		return m_Size;
	}

	inline std::string GetFilePath() const
	{
		assert(IsValid());
		return m_Path;
	}

	uint8_t *Load();
protected:
	void GetPath(const char *pFileName);
	eType GetType(const char *pFileName);
private:
	size_t m_Size = 0U;
	eType m_Type = eTypeCount;
	std::string m_Path;
};
