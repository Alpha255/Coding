#pragma once

#include "Base.h"

class AssetFile
{
public:
	enum eType
	{
		eShader,
		eShaderBinary,
		eTexture,
		eSDKMesh,
		eTxtMesh,
		eObjMesh,
		eSound,
		eLevelData,
		eTypeCount
	};

	AssetFile(const std::string &fileName);
	~AssetFile() = default;

	inline size_t GetSize() const
	{
		return m_Size;
	}

	inline std::string GetPath() const
	{
		return m_Path;
	}

	inline std::string GetRoot() const
	{
		return m_Root;
	}

	inline eType GetType() const
	{
		return m_Type;
	}

	std::shared_ptr<uint8_t> Load();
protected:
	bool TryToFindAssetFile();
	void GetAssetType();
private:
	size_t m_Size = 0U;
	eType m_Type = eTypeCount;
	std::string m_Path;
	std::string m_Root;
	std::shared_ptr<uint8_t> m_Data;
};
