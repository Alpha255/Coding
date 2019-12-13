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

rTexture *rAssetBucket::createTexture(const std::string &textureName, const rDevice &)
{
	auto textureAssetPtr = m_Bucket.getAsset(textureName);
	assert(textureAssetPtr->getType() == assetFile::eWICTexture || 
		textureAssetPtr->getType() == assetFile::eDDSTexture || 
		textureAssetPtr->getType() == assetFile::eVulkanTexture);

	if (m_Engine == appConfig::eD3D11)
	{

	}
	else if (m_Engine == appConfig::eVulkan)
	{
		rTextureBinary textureBinary = assetTool::getTextureBinary(m_Engine, textureAssetPtr);
	}

	return nullptr;
}

namespaceEnd(rAsset)
