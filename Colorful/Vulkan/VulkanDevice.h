#pragma once

#include "VulkanObject.h"

class VulkanInstance : public VulkanObject<VkInstance>
{
public:
	void Create(const std::string &appName = "VulkanApplication", const std::string &engineName = "VulkanEngine");

	void Destory() override
	{
		assert(IsValid());

		vkDestroyInstance(m_Handle, nullptr);

		Reset();
	}
protected:
private:
#if 0
	VkDebugReportCallbackEXT m_DebugReportCallback = VK_NULL_HANDLE;
#endif
};

class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice>
{
public:
	void Create();
	
	void Destory() override
	{
		Reset();

		m_PhysicalDevices.clear();
	}

	inline uint32_t GetQueueFamilyIndex() const
	{
		assert(IsValid());
		return m_QueueFamilyIndex;
	}

	inline const VkPhysicalDeviceFeatures &GetDeviceFeatures() const
	{
		assert(IsValid());
		return m_DeviceFeatures;
	}

	inline const VkPhysicalDeviceProperties &GetDeviceProperties() const
	{
		assert(IsValid());
		return m_DeviceProperties;
	}

	inline const VkPhysicalDeviceMemoryProperties &GetDeviceMemoryProperties() const
	{
		assert(IsValid());
		return m_DeviceMemoryProperties;
	}

	///uint32_t GetOptimalSurfaceFormat(uint32_t flags, bool bDepthSurface) const;
protected:
private:
	std::vector<VkPhysicalDevice> m_PhysicalDevices;

	VkPhysicalDeviceFeatures m_DeviceFeatures = {};
	VkPhysicalDeviceProperties m_DeviceProperties = {};
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties = {};

	uint32_t m_QueueFamilyIndex = UINT32_MAX;
};

class VulkanQueue : public VulkanObject<VkQueue> 
{
public:
	void Destory() override
	{
	}
protected:
private:
};

class VulkanDevice : public VulkanObject<VkDevice>
{
public:
	void Create();

	void Destory() override
	{
		assert(IsValid());

		m_PhysicalDevice.Destory();

		m_DeviceQueue.Reset();

		vkDestroyDevice(m_Handle, nullptr);

		Reset();
	}

	inline const VulkanPhysicalDevice &GetPhysicalDevice() const
	{
		assert(m_PhysicalDevice.IsValid());
		return m_PhysicalDevice;
	}

	inline const VulkanQueue &GetQueue()
	{
		assert(m_DeviceQueue.IsValid());
		return m_DeviceQueue;
	}

	inline const VkPhysicalDeviceMemoryProperties &GetDeviceMemoryProperties() const
	{
		return m_PhysicalDevice.GetDeviceMemoryProperties();
	}
protected:
private:
	VulkanPhysicalDevice m_PhysicalDevice;
	VulkanQueue m_DeviceQueue;
};
