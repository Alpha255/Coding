#include "D3D12Device.h"
#include "Colorful/Public/RAsset.h"

GfxShader *d3d12Device::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	return nullptr;
}

GfxTexture *d3d12Device::createTexture(const std::string &)
{
	return nullptr;
}