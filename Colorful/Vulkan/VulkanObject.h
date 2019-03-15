#pragma once

#include "VulkanTypes.h"

template <typename Left, typename Right>
inline bool IsEqual(const Left &left, const Right &right)
{
	return memcmp(&left, &right, sizeof(Right)) == 0;
}

template <typename T> class IVulkanObject
{
public:
	IVulkanObject() = default;
	virtual ~IVulkanObject() = default;

	virtual void Destory() = 0;
protected:
	inline void MakeObject(T object)
	{
		if (object)
		{
			m_Object.reset((void *)(object), std::bind(&IVulkanObject::Destory, this));
		}
	}

	std::shared_ptr<void> m_Object = nullptr;
private:
};

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

	inline VulkanObject(const T other)
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

	inline void operator=(const T other)
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

	inline bool operator==(const T other) const
	{
		return m_Handle == other;
	}

	inline bool operator!=(const VulkanObject &other) const
	{
		return this->m_Handle != other.m_Handle;
	}

	inline bool operator!=(const T other) const
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
	void Create(const char *pApplicationName = "VulkanApplication", const char *pEngineName = "VulkanEngine");

	inline void Destory()
	{
		assert(IsValid());

		vkDestroyInstance(m_Handle, nullptr);

		Reset();
	}
protected:
private:
	std::vector<VkLayerProperties> m_LayerProperties;
	std::vector<VkExtensionProperties> m_ExtensionProperties;
	VkDebugReportCallbackEXT m_DebugReportCallback = VK_NULL_HANDLE;
};

class VulkanLogicalDevice : public VulkanObject<VkDevice> {};
class VulkanQueue : public VulkanObject<VkQueue> {};

class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice> 
{
public:
	void Create();
	inline void Destory()
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
		return m_Features;
	}

	inline const VkPhysicalDeviceProperties &GetDeviceProperties() const
	{
		assert(IsValid());
		return m_Properties;
	}

	inline const VkPhysicalDeviceMemoryProperties &GetDeviceMemoryProperties() const
	{
		assert(IsValid());
		return m_MemoryProperties;
	}

	inline const std::vector<VkLayerProperties> &GetLayerProperties() const
	{
		assert(IsValid());
		return m_LayerProperties;
	}

	inline const std::vector<VkExtensionProperties> &GetExtensionProperties() const
	{
		assert(IsValid());
		return m_ExtensionProperties;
	}

	uint32_t GetOptimalSurfaceFormat(uint32_t flags, bool bDepthSurface) const;
protected:
	void VerifyDeviceFeatures(VkPhysicalDeviceFeatures &enabledFeatures);
private:
	std::vector<VkPhysicalDevice> m_PhysicalDevices;

	VkPhysicalDeviceFeatures m_Features = {};
	VkPhysicalDeviceProperties m_Properties = {};
	VkPhysicalDeviceMemoryProperties m_MemoryProperties = {};

	std::vector<VkLayerProperties> m_LayerProperties;
	std::vector<VkExtensionProperties> m_ExtensionProperties;

	uint32_t m_QueueFamilyIndex = UINT_MAX;
};

class VulkanDevice : public VulkanObject<VkDevice>
{
public:
	void Create();
	inline void Destory()
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

	inline VkQueue GetQueue() const
	{
		assert(m_DeviceQueue.IsValid());
		return m_DeviceQueue.Get();
	}
protected:
private:
	VulkanPhysicalDevice m_PhysicalDevice;
	VulkanQueue m_DeviceQueue;
};

class VulkanSemaphore : public VulkanObject<VkSemaphore>
{
public:
	void Create();
	void Destory();
protected:
private:
};

class VulkanFence : public VulkanObject<VkFence>
{
public:
	enum eFenceState
	{
		eUnSignaled,
		eSignaled
	};

	void Create(eFenceState state = eUnSignaled);
	void Destory();
protected:
private:
};

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

class VulkanDescriptorSetLayout : public VulkanObject<VkDescriptorSetLayout>
{
public:
	void Create(std::vector<VkSampler> &samplers, uint32_t targetShader);
	void Destory();
protected:
private:
};

typedef VulkanObject<VkDescriptorSet> VulkanDescriptorSet;
