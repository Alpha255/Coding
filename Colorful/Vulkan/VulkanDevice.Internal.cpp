#include "VulkanDevice.h"
#include "Colorful/Public/RAsset.h"

rShader *vkDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	return nullptr;
}

vkFence *vkDevice::createFence(vkFence::eFenceState state)
{
	vkFence *fencePtr = new vkFence(*this, state);
	assert(fencePtr);

	return fencePtr;
}
