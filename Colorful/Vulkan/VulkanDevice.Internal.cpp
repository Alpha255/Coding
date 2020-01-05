#include "VulkanEngine.h"
#include "VulkanTexture.h"

void vkCommandPool::create(const vkDevice &device, uint32_t queueIndex)
{
	assert(!isValid() && device.isValid());

	/// queueFamilyIndex designates a queue family as described in section Queue Family Properties. 
	/// All command buffers allocated from this command pool must be submitted on queues from the same queue family.
	VkCommandPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		queueIndex
	};

	VkCommandPool handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateCommandPool(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

vkCommandBufferArray vkCommandPool::allocCommandBuffers(const vkDevice &device, VkCommandBufferLevel level, uint32_t count) const
{
	assert(isValid() && device.isValid() && count);

	VkCommandBufferAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		**this,
		level,
		count
	};

	vkCommandBufferArray commandBuffers(level, count);
	rVerifyVk(vkAllocateCommandBuffers(*device, &allocateInfo, commandBuffers.m_CommandBuffers.data()));

	return commandBuffers;
}

void vkCommandBufferArray::begin()
{

}

void vkCommandBufferArray::end(bool8_t)
{

}

void vkCommandBufferArray::queueSubmit()
{

}

void vkCommandPool::freeCommandBuffers(const vkDevice &device, vkCommandBufferArray &commandBuffers) const
{
	assert(isValid() && device.isValid());

	vkFreeCommandBuffers(*device, **this, (uint32_t)commandBuffers.m_CommandBuffers.size(), commandBuffers.m_CommandBuffers.data());
	commandBuffers.destory();
}

void vkCommandPool::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyCommandPool(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkDescriptorPool::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorPool(*device, **this, vkMemoryAllocator);
		reset();
	}
}

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
