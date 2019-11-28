#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

class vkInstance : public vkObject<VkInstance>
{
public:
	void create();

	void destory();
};
typedef std::shared_ptr<vkInstance> vkInstancePtr;

class vkDebugUtilsMessenger : public vkObject<VkDebugUtilsMessengerEXT>
{
public:
	void create(const vkInstancePtr &instancePtr, bool8_t verbose);

	void destory(const vkInstancePtr &instancePtr);
};
typedef std::shared_ptr<vkDebugUtilsMessenger> vkDebugUtilsMessengerPtr;

class vkPhysicalDevice : public vkObject<VkPhysicalDevice>
{
public:
	static std::vector<std::shared_ptr<vkPhysicalDevice>> enumeratePhysicalDevices(const vkInstancePtr &instancePtr);
};
typedef std::shared_ptr<vkPhysicalDevice> vkPhysicalDevicePtr;

class vkDevice : public vkObject<VkDevice>, public rDevice
{
public:
	uint32_t create(
		const std::vector<vkPhysicalDevicePtr> &physicalDevicePtrs,
		uint32_t &graphicsQueueIndex,
		uint32_t &computeQueueIndex,
		uint32_t &transferQueueIndex
	);

	void waitIdle();

	void destory();

	rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) override final;
};
typedef std::shared_ptr<vkDevice> vkDevicePtr;

class vkDeviceQueue : public vkObject<VkQueue>
{
public:
	vkDeviceQueue(uint32_t queueFamilyIndex, const vkDevicePtr &devicePtr)
		: m_FamilyIndex(queueFamilyIndex)
	{
		VkQueue handle = VK_NULL_HANDLE;
		vkGetDeviceQueue(&(*devicePtr), queueFamilyIndex, 0u, &handle);
		reset(handle);
	}
protected:
private:
	uint32_t m_FamilyIndex = UINT32_MAX;
};
typedef std::shared_ptr<vkDeviceQueue> vkDeviceQueuePtr;
