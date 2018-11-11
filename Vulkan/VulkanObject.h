#pragma once

#include "Common.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define VKCheck(func) if (VK_SUCCESS != (func)) { assert(0); }

template <typename T> class VulkanObject
{
public:
	VulkanObject() = default;
	inline virtual ~VulkanObject() = default;

	inline T Get() const
	{
		assert(IsValid());
		return m_Handle;
	}

	inline VulkanObject(const VulkanObject &other)
	{
		if (m_Handle != other.m_Handle)
		{
			m_Handle = other.m_Handle;
		}
	}

	inline bool IsValid() const
	{
		return m_Handle != VK_NULL_HANDLE;
	}

	inline bool operator==(const VulkanObject &other) const
	{
		return this->m_Handle == other.m_Handle;
	}

	inline bool operator==(const T &other) const
	{
		return m_Handle == other;
	}

	inline bool operator!=(const VulkanObject &other) const
	{
		return !(*this == other);
	}

	inline bool operator!=(const T &other) const
	{
		return !(*this == other);
	}
protected:
	T m_Handle = VK_NULL_HANDLE;
private:
};

class VulkanInstance : public VulkanObject<VkInstance>
{
public:
	void Create(const char *pInstName = "VulkanInstance");

	inline void Destory()
	{
		vkDestroyInstance(m_Handle, nullptr);
	}

	inline const std::vector<VkLayerProperties> &GetLayerProperties() const
	{
		return m_LayerProperties;
	}
protected:
	void InitLayerProperties();

private:
	std::vector<VkLayerProperties> m_LayerProperties;
	std::vector<VkExtensionProperties> m_ExtensionProperties;
};

class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice> 
{
public:
	void Create();

	inline const std::vector<VkQueueFamilyProperties> &GetQueueFamilyProperties() const
	{
		return m_QueueFamilyProperties;
	}

	inline const VkPhysicalDeviceMemoryProperties &GetDeviceMemoryProperties() const
	{
		return m_DeviceMemoryProperties;
	}

protected:
	void InitExtensionProperties();
private:
	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	std::vector<VkExtensionProperties> m_ExtensionProperties;
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties = {};
	VkPhysicalDeviceProperties m_DeviceProperties = {};
};

class VulkanDevice : public VulkanObject<VkDevice>
{
public:
	void Create();
protected:
private:
	uint32_t m_GraphicQueueFamilyIndex = UINT32_MAX;
	uint32_t m_QueuePresentQueueFamilyIndex = UINT32_MAX;
};
