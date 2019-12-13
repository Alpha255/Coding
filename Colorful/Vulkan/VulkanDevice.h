#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

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

class vkCommandBufferArray
{
public:
	vkCommandBufferArray(VkCommandBufferLevel level, uint32_t count)
		: m_Level(level)
	{
		m_CommandBuffers.resize(count);
	}

	inline VkCommandBuffer get(uint32_t index = 0u) const
	{
		assert(m_CommandBuffers.size() > 0u && index < m_CommandBuffers.size());
		return m_CommandBuffers[index];
	}

	void begin();
	void end(bool8_t submit = true);
	void queueSubmit();
protected:
	inline void destory()
	{
		m_CommandBuffers.clear();
		m_Level = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
	}
private:
	friend class vkCommandPool;

	std::vector<VkCommandBuffer> m_CommandBuffers;
	VkCommandBufferLevel m_Level = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
};

class vkCommandPool : public vkDeviceObject<VkCommandPool>
{
public:
	void create(const class vkDevice &device, uint32_t queueIndex);
	void destroy(const class vkDevice &device) override final;

	vkCommandBufferArray allocCommandBuffers(const class vkDevice &device, VkCommandBufferLevel level, uint32_t count) const;
	void freeCommandBuffers(const class vkDevice &device, vkCommandBufferArray &commandBuffers) const;
protected:
private:
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
	vkCommandPool m_CommandPool;
};

class vkDeviceQueue : public vkObject<VkQueue>
{
public:
	void create(uint32_t queueFamilyIndex, const vkDevice &device)
	{
		assert(!isValid() && device.isValid());

		m_FamilyIndex = queueFamilyIndex;
		VkQueue handle = VK_NULL_HANDLE;
		vkGetDeviceQueue(*device, queueFamilyIndex, 0u, &handle);
		reset(handle);
	}
protected:
private:
	uint32_t m_FamilyIndex = UINT32_MAX;
};
