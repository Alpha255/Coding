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

	inline bool IsValid() const
	{
		return m_Object != VK_NULL_HANDLE;
	}

	inline T * const Get()
	{
		return &m_Object;
	}

	inline const T &GetRef() const
	{
		return m_Object;
	}

	inline VulkanObject(const VulkanObject &other)
	{
		if (m_Object != other.m_Object)
		{
			m_Object = other.m_Object;
		}
	}

	inline bool operator==(const VulkanObject &other) const
	{
		return this->m_Object == other.m_Object;
	}

	inline bool operator==(const T &other) const
	{
		return m_Object == other;
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
	T m_Object = VK_NULL_HANDLE;
private:
};

class VulkanInstance : public VulkanObject<VkInstance> {};
class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice> {};

class VulkanDevice : public VulkanObject<VkDevice>
{
public:
	inline void Destory()
	{
		vkDeviceWaitIdle(m_Object);
		vkDestroyDevice(m_Object, nullptr);
	}
protected:
private:
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	void Create(::HWND hWnd, uint32_t width, uint32_t height);
protected:
	class VulkanSurface : public VulkanObject<VkSurfaceKHR>
	{
	public:
		void Create(::HWND hWnd);
	protected:
	private:
	};

	///std::array<uint32_t, 2> ValidPresentingSupport();
private:
	VulkanSurface m_Surface;
};
