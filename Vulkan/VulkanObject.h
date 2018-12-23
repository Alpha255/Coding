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

	inline const T *GetPtr() const
	{
		assert(IsValid());
		return &m_Handle;
	}

	inline VulkanObject(const VulkanObject &other)
	{
		if (m_Handle != other.m_Handle)
		{
			m_Handle = other.m_Handle;
		}
	}

	inline void operator=(const VulkanObject &other)
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

	inline VkQueue GetQueue() const
	{
		return m_Queue;
	}
protected:
private:
	VkQueue m_Queue = 0;
};

class VulkanCommandBuffer : public VulkanObject<VkCommandBuffer>
{
public:
	void Create(const class VulkanCommandPool &pool, uint32_t level, uint32_t count);

	void Begin(uint32_t flags);

	void End();
protected:
private:
};

class VulkanRenderPass : public VulkanObject<VkRenderPass>
{
public:
	void Create(bool depth, uint32_t colorFormat, uint32_t depthFormat, bool clear = true, uint32_t imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
protected:
private:
};

class VulkanSemaphore : public VulkanObject<VkSemaphore>
{
public:
	void Create();
protected:
private:
};

class VulkanFence : public VulkanObject<VkFence>
{
public:
	void Create();
protected:
private:
};

class VulkanDescriptorSetLayout : public VulkanObject<VkDescriptorSetLayout>
{
public:
	void Create(bool bUseTex);
protected:
private:
};

class VulkanPipelineLayout : public VulkanObject<VkPipelineLayout>
{
public:
	void Create(bool bUseTex);
protected:
private:
};

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
public:
	void Create(bool bUseTex);
protected:
private:
};

class VulkanPipelineCache : public VulkanObject<VkPipelineCache>
{
public:
protected:
private:
};

class VulkanViewport : public VkViewport
{
public:
	VulkanViewport(
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
