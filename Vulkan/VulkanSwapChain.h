#pragma once

#include "VulkanObject.h"

class VulkanSurface : public VulkanObject<VkSurfaceKHR>
{
public:
	void Create(::HWND hWnd);
protected:
private:
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	void Init(::HWND hWnd);

	void Create(uint32_t width, uint32_t height, bool bWindowed = true);

	inline uint32_t GetGraphicsQueueFamilyIndex() const
	{
		assert(m_GraphicsQueueFamilyIndex != UINT32_MAX);
		return m_GraphicsQueueFamilyIndex;
	}

	inline uint32_t GetPresentQueueFamilyIndex() const
	{
		assert(m_PresentQueueFamilyIndex != UINT32_MAX);
		return m_PresentQueueFamilyIndex;
	}
protected:
private:
	VulkanSurface m_Surface;
	uint32_t m_GraphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t m_PresentQueueFamilyIndex = UINT32_MAX;
};
