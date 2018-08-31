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

	inline T *MakeObject()
	{
		T *pNewObject = new T();
		m_Object.reset(pNewObject);
		return pNewObject;
	}

	inline bool IsValid() const
	{
		return m_Object.get() != nullptr && *m_Object != VK_NULL_HANDLE;
	}

	inline T *Get() const 
	{
		return m_Object.get();
	}

	inline const T &GetRef() const
	{
		return *m_Object;
	}

	inline void Reset()
	{
		if (m_Object)
		{
			m_Object.reset();
			m_Object = nullptr;
		}
	}

	inline VulkanObject(const VulkanObject &other)
	{
		if (m_Object != other.m_Object)
		{
			m_Object = other.m_Object;
		}
	}

	inline T * const operator->() const
	{
		assert(IsValid());
		return m_Object.get();
	}

	inline bool operator==(const VulkanObject &other) const
	{
		if (IsValid() && other.IsValid())
		{
			return (void*)m_Object.get() == (void*)other.Get();
		}
		else if (!IsValid() && !other.IsValid())
		{
			return true;
		}

		return false;
	}

	inline bool operator==(const T *other) const
	{
		return (void*)m_Object.get() == (void*)other;
	}

	inline bool operator!=(const VulkanObject &other) const
	{
		return !(*this == other);
	}

	inline bool operator!=(const T *other) const
	{
		return (void*)m_Object.get() != (void*)other;
	}
protected:
	std::shared_ptr<T> m_Object;
private:
};

class VulkanInstance : public VulkanObject<VkInstance> {};
class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice> {};

class VulkanDevice : public VulkanObject<VkDevice>
{
public:
	inline ~VulkanDevice()
	{
		Destory();
	}
protected:
	inline void Destory()
	{
		vkDeviceWaitIdle(*m_Object);
		vkDestroyDevice(*m_Object, nullptr);
	}
private:
};

class VulkanSurface : public VulkanObject<VkSurfaceKHR> 
{
public:
	inline ~VulkanSurface()
	{
		Destory();
	}

	void Create(const VulkanInstance &inst, ::HWND window);
protected:
	inline void Destory()
	{
		vkDestroySurfaceKHR(m_Inst.GetRef(), *m_Object, nullptr);
	}
private:
	VulkanInstance m_Inst;
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	inline ~VulkanSwapchain()
	{
		Destory();
	}

	void Create(const VulkanPhysicalDevice &vkpDevice, const VulkanDevice &device, const VulkanInstance &inst, 
		uint32_t width, uint32_t height, ::HWND window);
protected:
	inline void Destory()
	{
		vkDestroySwapchainKHR(m_Device.GetRef(), *m_Object, nullptr);
	}

	std::array<uint32_t, 2> ValidPresentingSupport(const VulkanPhysicalDevice &vkpDevice);
private:
	VulkanDevice m_Device;
	VulkanSurface m_Surface;
};
