#include "VulkanDevice.h"

rShaderPtr vkDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName, std::shared_ptr<rDevice>(this));
	return nullptr;
}
