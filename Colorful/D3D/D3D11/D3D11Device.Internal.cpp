#include "D3D11Device.h"
#include "Colorful/Public/RAsset.h"

rShader *d3d11Device::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	return nullptr;
}

rTexture *d3d11Device::createTexture(const std::string &)
{
	return nullptr;
}