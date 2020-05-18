#pragma once

#include "Colorful/Public/GfxResource.h"

namespaceStart(AssetTool)

struct ShaderBinary
{
	std::shared_ptr<byte8_t> Binary = nullptr;
	size_t Size = 0ull;

	GfxShaderReflections Reflections;
};

struct TextureBinary
{
	struct Image
	{
		uint32_t Width = 0u;
		uint32_t Height = 0u;
		uint32_t Depth = 0u;
		size_t Size = 0u;
	};

	eRTextureType Type = eRTextureType_MaxEnum;
	uint32_t Format = eRFormat_MaxEnum;

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Depth = 0u;
	uint32_t MipLevels = 0u;
	uint32_t ArrayLayers = 0u;

	size_t Size = 0ull;

	std::shared_ptr<byte8_t> Binary = nullptr;

	std::vector<std::vector<Image>> Images;
};

class Asset : public File
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

	Asset(const std::string& filePath, eAssetType type, uint32_t assetID)
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
private:
	eAssetType m_Type = eUnknown;
	uint32_t m_ID = std::numeric_limits<uint32_t>().max();
};
using AssetPtr = std::shared_ptr<Asset>;

namespaceEnd(AssetTool)
