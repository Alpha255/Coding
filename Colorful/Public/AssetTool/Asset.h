#pragma once

#include "Colorful/Public/GfxResource.h"

#define STB_IMAGE_IMPLEMENTATION

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

	std::shared_ptr<byte8_t> SharedBinary = nullptr;
	const void* Binary = nullptr;

	std::vector<std::vector<Image>> Images;
};

using AssetID = uint32_t;
DeclareShared(Asset)
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
		eModel,
		eSound,
		eLevel,
		eMaterial,
		eAssetType_MaxEnum
	};

	Asset(const std::string& filePath, eAssetType type, AssetID assetID)
		: File(filePath)
		, m_Type(type)
		, m_ID(assetID)
	{
	}

	Asset(const std::string& name);

	inline eAssetType type() const
	{
		return m_Type;
	}

	inline AssetID ID() const
	{
		return m_ID;
	}
protected:
private:
	eAssetType m_Type = eUnknown;
	AssetID m_ID = std::numeric_limits<uint32_t>().max();
};

namespaceEnd(AssetTool)
