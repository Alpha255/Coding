#pragma once

#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "VulkanDescriptor.h"

class vkInstance : public vkObject<VkInstance>
{
public:
	void create();
	void destroy();
};

class vkDebugUtilsMessenger : public vkObject<VkDebugUtilsMessengerEXT>
{
public:
	void create(const vkInstance &instance, bool8_t verbose);
	void destroy(const vkInstance &instance);
};

class vkPhysicalDevice : public vkObject<VkPhysicalDevice>
{
public:
	static std::vector<vkPhysicalDevice> enumeratePhysicalDevices(const vkInstance &instance);
};

class vkDevice : public vkObject<VkDevice>, public rDevice
{
public:
	uint32_t create(
		const std::vector<vkPhysicalDevice> &physicalDevices,
		uint32_t &graphicsQueueIndex,
		uint32_t &computeQueueIndex,
		uint32_t &transferQueueIndex
	);

	void waitIdle();

	vkFence *createFence(vkFence::eFenceState state, bool8_t autoReset = false);
	vkSemaphore *createSemaphore(bool8_t autoReset = false);
	vkEvent *createEvent(bool8_t autoReset = false);

	void destroy();

	uint32_t getMemoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const;

	inline vkCommandBuffer allocCommandBuffer(VkCommandBufferLevel level) const
	{
		return m_CommandPool.allocCommandBuffer(*this, level);
	}

	inline void freeCommandBuffer(vkCommandBuffer &commandBuffer) const
	{
		m_CommandPool.freeCommandBuffer(*this, commandBuffer);
	}

	inline vkCommandBufferArray allocCommandBuffers(VkCommandBufferLevel level, uint32_t count) const
	{
		return m_CommandPool.allocCommandBuffers(*this, level, count);
	}

	inline void freeCommandBuffers(vkCommandBufferArray &commandBuffers) const
	{
		m_CommandPool.freeCommandBuffers(*this, commandBuffers);
	}
public:
	rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) override final;
	rTexturePtr createTexture(const std::string &textureName) override final;
protected:
private:
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	VkPhysicalDeviceFeatures m_DeviceFeatures{};
	VkPhysicalDeviceLimits m_DeviceLimits{};
	vkCommandPool m_CommandPool;
	vkPipelineCache m_PipelineCache;
};

class vkDeviceQueue : public vkObject<VkQueue>
{
public:
	void create(uint32_t queueFamilyIndex, const vkDevice &device)
	{
		assert(!isValid() && device.isValid());

		/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
		/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
		m_FamilyIndex = queueFamilyIndex;
		VkQueue handle = VK_NULL_HANDLE;
		vkGetDeviceQueue(*device, queueFamilyIndex, 0u, &handle);
		reset(handle);

		/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
	}
protected:
private:
	uint32_t m_FamilyIndex = UINT32_MAX;
};
