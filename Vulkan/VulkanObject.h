#pragma once

#include "VulkanTypes.h"

template <typename T> class VulkanObject
{
public:
	VulkanObject() = default;
	virtual ~VulkanObject() = default;

	inline T Get() const
	{
		assert(IsValid());
		return m_Handle;
	}

	inline VulkanObject(const VulkanObject &other)
	{
		m_Handle = other.m_Handle;
	}

	inline VulkanObject(const T &other)
	{
		m_Handle = other;
	}

	inline T *operator&()
	{
		return &m_Handle;
	}

	inline const T *operator&() const
	{
		assert(IsValid());
		return &m_Handle;
	}

	inline void operator=(const VulkanObject &other)
	{
		assert(!IsValid());
		m_Handle = other.m_Handle;
	}

	inline void operator=(const T &other)
	{
		assert(!IsValid());
		m_Handle = other;
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
		return this->m_Handle != other.m_Handle;
	}

	inline bool operator!=(const T &other) const
	{
		return m_Handle != other;
	}

	inline void Reset(const T other = VK_NULL_HANDLE)
	{
		m_Handle = other;
	}
protected:
	T m_Handle = VK_NULL_HANDLE;
private:
};

class VulkanInstance : public VulkanObject<VkInstance>
{
public:
	VulkanInstance() = default;
	~VulkanInstance()
	{
		Destory();
	}

	void Create(const char *pApplicationName = "VulkanApplication", const char *pEngineName = "VulkanEngine");

	inline void Destory()
	{
		vkDestroyInstance(m_Handle, nullptr);
		Reset();
	}
protected:
private:
	std::vector<VkLayerProperties> m_LayerProperties;
	std::vector<VkExtensionProperties> m_ExtensionProperties;
};

typedef VulkanObject<VkPhysicalDevice> VulkanPhysicalDevice;
class VulkanLogicalDevice : public VulkanObject<VkDevice> {};
class VulkanQueue : public VulkanObject<VkQueue> {};

class VulkanDevice
{
public:
	VulkanDevice() = default;
	~VulkanDevice() = default;

	inline void Create()
	{
		GetPhysicalDevice();

		CreateLogicalDevice();
	}

	inline VkDevice GetLogicalDevice() const
	{
		assert(m_LogicalDevice.IsValid());
		return m_LogicalDevice.Get();
	}

	inline VkPhysicalDevice GetPhysicalDevice(uint32_t index = 0U) const
	{
		assert(index < m_PhysicalDevices.size() && m_PhysicalDevices.at(index).IsValid());
		return m_PhysicalDevices.at(index).Get();
	}

	inline VkQueue GetQueue() const
	{
		assert(m_DeviceQueue.IsValid());
		return m_DeviceQueue.Get();
	}

	inline uint32_t GetQueueFamilyIndex() const
	{
		assert(m_QueueFamilyIndex != UINT_MAX);
		return m_QueueFamilyIndex;
	}

	inline const VkPhysicalDeviceMemoryProperties &GetPhysicalDeviceMemoryProperties() const
	{
		assert(m_PhysicalDevices[m_QueueFamilyIndex].IsValid());
		return m_PhysicalMemoryProperties;
	}

	uint32_t GetOptimalSurfaceFormat(uint32_t flags, bool bDepthSurface) const;
protected:
	void GetPhysicalDevice();
	void CreateLogicalDevice();
	void VerifyPhysicalFeatures(VkPhysicalDeviceFeatures &enableFeatures);
private:
	uint32_t m_QueueFamilyIndex = UINT_MAX;
	std::vector<VulkanPhysicalDevice> m_PhysicalDevices;
	VulkanLogicalDevice m_LogicalDevice;
	VulkanQueue m_DeviceQueue;
	std::vector<VkLayerProperties> m_LayerProperties;
	std::vector<VkExtensionProperties> m_ExtensionProperties;
	VkPhysicalDeviceFeatures m_EnabledFeatures = {};
	VkPhysicalDeviceProperties m_PhysicalProperties = {};
	VkPhysicalDeviceMemoryProperties m_PhysicalMemoryProperties = {};
};

//class VulkanRenderPass : public VulkanObject<VkRenderPass>
//{
//public:
//	void Create(bool depth, uint32_t colorFormat, uint32_t depthFormat, bool clear = true, uint32_t imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
//protected:
//private:
//};

class VulkanSemaphore : public VulkanObject<VkSemaphore>
{
public:
	VulkanSemaphore() = default;
	inline ~VulkanSemaphore()
	{
		Destory();
	}

	void Create();

	void Destory();
protected:
private:
};

class VulkanFence : public VulkanObject<VkFence>
{
public:
	VulkanFence() = default;
	inline ~VulkanFence()
	{
		Destory();
	}

	void Create();

	void Destory();
protected:
private:
};

//class VulkanDescriptorSetLayout : public VulkanObject<VkDescriptorSetLayout>
//{
//public:
//	void Create(bool bUseTex);
//protected:
//private:
//};
//
//class VulkanPipelineLayout : public VulkanObject<VkPipelineLayout>
//{
//public:
//	void Create(bool bUseTex);
//protected:
//private:
//};
//
//class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
//{
//public:
//	void Create(bool bUseTex);
//protected:
//private:
//};
//
//class VulkanPipelineCache : public VulkanObject<VkPipelineCache>
//{
//public:
//protected:
//private:
//};

class VulkanViewport : public VkViewport
{
public:
	inline VulkanViewport(
		float w,
		float h,
		float depthMin = 0.0f,
		float depthMax = 1.0f,
		float left = 0.0f,
		float top = 0.0f)
	{
		width = w;
		height = h;
		x = left;
		y = top;
		minDepth = depthMin;
		maxDepth = depthMax;
	}
protected:
private:
};

class VulkanRect : public VkRect2D
{
public:
	inline VulkanRect() = default;

	inline VulkanRect(const ::RECT &rect)
	{
		offset.x = (uint32_t)rect.left;
		offset.y = (uint32_t)rect.top;

		extent.width = (uint32_t)(rect.right - rect.left);
		extent.height = (uint32_t)(rect.bottom - rect.top);
	}
};
