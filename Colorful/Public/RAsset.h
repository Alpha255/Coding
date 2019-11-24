#pragma once

#include "Colorful/Public/RInterface.h"

namespaceStart(rAsset)

struct rShaderBinary
{
	std::shared_ptr<byte> Binary = nullptr;
	size_t Size = 0ull;
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

	rTexturePtr createTexture(const std::string &textureName, const rDevicePtr &devicePtr);

	void finalize() {}
protected:
private:
	rShaderCache m_ShaderCache;
	assetBucket m_Bucket;
	appConfig::eRenderEngine m_Engine = appConfig::eD3D11;
};

namespaceEnd(rAsset)
