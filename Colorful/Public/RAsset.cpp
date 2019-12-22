#include "RAsset.h"
#include "AssetTool/AssetTool.h"

namespaceStart(rAsset)

rShaderBinary rAssetBucket::getShaderBinary(eRShaderUsage usage, const std::string &shaderName)
{
	assert(usage < eRShaderUsage_MaxEnum);
	rShaderBinary shaderBinary = m_ShaderCache.getShaderBinary(usage, shaderName, m_Engine);
	if (shaderBinary.Binary)
	{
		return shaderBinary;
	}

	auto shaderAssetPtr = m_Bucket.getAsset(shaderName);
	assert(shaderAssetPtr->getType() == assetFile::eShader);

	shaderBinary = assetTool::getShaderBinary(m_Engine, usage, shaderAssetPtr);
	assert(shaderBinary.Binary && shaderBinary.Size > 0ull);

	return shaderBinary;
}

rTextureBinary rAssetBucket::getTextureBinary(const std::string &textureName)
{
	auto textureAssetPtr = m_Bucket.getAsset(textureName);
	assert(textureAssetPtr->getType() == assetFile::eWICTexture || 
		textureAssetPtr->getType() == assetFile::eDDSTexture || 
		textureAssetPtr->getType() == assetFile::eVulkanTexture);

	rTextureBinary textureBinary;
	if (m_Engine == appConfig::eVulkan)
	{
		textureBinary = assetTool::getTextureBinary(m_Engine, textureAssetPtr);
	}
	else
	{
		assert(0);
	}

	return textureBinary;
}

namespaceEnd(rAsset)
