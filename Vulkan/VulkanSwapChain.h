#pragma once

#include "VulkanView.h"

class VulkanSurface : public VulkanObject<VkSurfaceKHR>
{
public:
	void Create(::HWND hWnd);

	inline const VkSurfaceCapabilitiesKHR &GetCapabilities() const
	{
		assert(IsValid());
		return m_Capabilities;
	}

	inline const std::vector<VkPresentModeKHR> &GetPresentModes() const
	{
		assert(IsValid());
		return m_PresentModes;
	}

	inline const std::vector<VkSurfaceFormatKHR> &GetFormats() const
	{
		assert(IsValid());
		return m_Formats;
	}
protected:
private:
	VkSurfaceCapabilitiesKHR m_Capabilities = {};
	std::vector<VkPresentModeKHR> m_PresentModes;
	std::vector<VkSurfaceFormatKHR> m_Formats;
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	void Create(::HWND hWnd, uint32_t uWidth, uint32_t uHeight, bool bFullScreen);
	void Resize(uint32_t uWidth, uint32_t uHeight);

	inline void SetVSync(bool bVSync)
	{
		assert(IsValid());
		m_bVSync = bVSync;
	}

	inline void SetFullScreen(bool bFullScreen)
	{
		assert(IsValid());
		m_bFullScreen = bFullScreen;
	}

	void Flush();
protected:
	struct VulkanBackBuffer
	{
		VulkanRenderTargetView RenderTargetView;
		VulkanFence PresentFence;
		VulkanSemaphore AcquireSemaphore;
		VulkanSemaphore RenderSemaphore;
	};
private:
	VulkanSurface m_Surface;
	VkExtent2D m_Size = {};
	VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
	VkFormat m_ColorSurfaceFormat = VK_FORMAT_UNDEFINED;
	VkFormat m_DepthSurfaceFormat = VK_FORMAT_UNDEFINED;
	VulkanDepthStencilView m_DepthStencilView;
	std::vector<VulkanBackBuffer> m_BackBuffers;
	uint32_t m_CurBackBufferIndex = 0U;
	bool m_bVSync = false;
	bool m_bFullScreen = false;
};
