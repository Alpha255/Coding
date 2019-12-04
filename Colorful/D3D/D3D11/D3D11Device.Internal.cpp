#include "D3D11Device.h"
#include "Colorful/Public/RAsset.h"

rShaderPtr d3d11Device::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	return nullptr;
}