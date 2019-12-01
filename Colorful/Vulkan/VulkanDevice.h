#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

class vkInstance : public vkObject<VkInstance>
{
public:
	void create();

	void destory();
};

class vkDebugUtilsMessenger : public vkObject<VkDebugUtilsMessengerEXT>
{
public:
	void create(const vkInstance &instance, bool8_t verbose);

	void destory(const vkInstance &instance);
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

	void destory();
public:
	rShader *createShader(eRShaderUsage usage, const std::string &shaderName) override final;
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
