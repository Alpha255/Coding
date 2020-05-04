#pragma once

#include "Colorful/Public/RInterface.h"

namespaceStart(rAsset)

struct rShaderBinary
{
	std::shared_ptr<byte8_t> Binary = nullptr;
	size_t Size = 0ull;

	rShader::rReflectionInfos Reflections;
};

struct rTextureBinary
{
	struct rImage
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
	const byte8_t *Binary = nullptr;

	std::vector<std::vector<rImage>> Images;
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

	rTextureBinary getTextureBinary(const std::string &textureName);

	void finalize() {}
protected:
private:
	rShaderCache m_ShaderCache;
	assetBucket m_Bucket;
	appConfig::eRenderEngine m_Engine = appConfig::eD3D11;
};

namespaceEnd(rAsset)
