#include "VulkanEngine.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

void vkDevice::vkGpuResourcePool::destory(rGpuResource *resource)
{
	assert(resource);
}

void vkDevice::vkGpuResourcePool::destoryAll()
{
}

rShader *vkDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	auto shader = new vkShader(*this, usage, shaderBinary);
	m_GpuResourcePool->push<vkGpuResourcePool::eShader>(shader);

	return shader;
}

rTexture *vkDevice::createTexture(const std::string &textureName)
{
	auto textureBinary = rAsset::rAssetBucket::instance().getTextureBinary(textureName);
	auto texture = new vkTexture(*this, textureBinary);
	m_GpuResourcePool->push<vkGpuResourcePool::eTexture>(texture);

	return texture;
}

rBuffer *vkDevice::createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void * pData)
{
	auto buffer = new vkGpuBuffer(*this, bindFlags, usage, size, pData);
	m_GpuResourcePool->push<vkGpuResourcePool::eBuffer>(buffer);

	return buffer;
}

vkFence *vkDevice::createFence(vkFence::eFenceStatus status, bool8_t)
{
	vkFence *fencePtr = new vkFence(*this, status);
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
