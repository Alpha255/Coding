#include "VulkanEngine.h"
#include "VulkanTexture.h"

rShaderPtr vkDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	return nullptr;
}

rTexturePtr vkDevice::createTexture(const std::string &textureName)
{
	auto textureBinary = rAsset::rAssetBucket::instance().getTextureBinary(textureName);
	rTexturePtr texturePtr(new vkTexture(*this, textureBinary));

	return texturePtr;
}

vkFence *vkDevice::createFence(vkFence::eFenceState state, bool8_t)
{
	vkFence *fencePtr = new vkFence(*this, state);
	assert(fencePtr);

	return fencePtr;
}

vkSemaphore *vkDevice::createSemaphore(bool8_t)
{
	vkSemaphore *semaphorePtr = new vkSemaphore(*this);
	assert(semaphorePtr);

	return semaphorePtr;
}

vkEvent *vkDevice::createEvent(bool8_t)
{
	vkEvent *eventPtr = new vkEvent(*this);
	assert(eventPtr);

	return eventPtr;
}
