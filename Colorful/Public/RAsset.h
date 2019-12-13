#pragma once

#include "Colorful/Public/RInterface.h"

namespaceStart(rAsset)

struct rShaderBinary
{
	std::shared_ptr<byte> Binary = nullptr;
	size_t Size = 0ull;
};

struct rTextureBinary
{
	struct rExtent
	{
		uint32_t Width = 0u;
		uint32_t Height = 0u;
	};

	eRTextureType Type = eRTextureType_MaxEnum;
	eRFormat Format = eRFormat_MaxEnum;

	uint32_t Depth = 0u;
	uint32_t MipLevels = 0u;
	uint32_t ArrayLayers = 0u;

	std::vector<std::vector<rExtent>> Extents;
	std::vector<std::vector<size_t>> Sizes;
	
	size_t Size = 0ull;
	std::shared_ptr<byte> Binary = nullptr;
};

class rShaderCache
{
public:
	rShaderBinary getShaderBinary(eRShaderUsage, const std::string &, appConfig::eRenderEngine)
	{
		return rShaderBinary();
	}
protected:
private:
};

class rAssetBucket : public singleton<rAssetBucket>
{
	singletonDeclare(rAssetBucket);
public:
	inline void initialize(appConfig::eRenderEngine engine)
	{
		m_Engine = engine;
		m_Bucket.initialize();
	}

	rShaderBinary getShaderBinary(eRShaderUsage usage, const std::string &shaderName);

	rTexture *createTexture(const std::string &textureName, const rDevice &devicePtr);

	void finalize() {}
protected:
private:
	rShaderCache m_ShaderCache;
	assetBucket m_Bucket;
	appConfig::eRenderEngine m_Engine = appConfig::eD3D11;
};

namespaceEnd(rAsset)
