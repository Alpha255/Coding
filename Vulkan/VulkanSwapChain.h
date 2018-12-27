#pragma once

#include "VulkanObject.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanPool.h"

class VulkanSurface : public VulkanObject<VkSurfaceKHR>
{
public:
	void Create(::HWND hWnd);

	inline const VkSurfaceCapabilitiesKHR &GetCapabilities() const
	{
		assert(IsValid());
		return m_Capabilities;
	}

	inline const std::vector<VkPresentModeKHR> &GetPresentMode() const
	{
		assert(IsValid());
		return m_PresentMode;
	}

	inline const std::vector<VkSurfaceFormatKHR> &GetFormats() const
	{
		assert(IsValid());
		return m_Formats;
	}
protected:
private:
	VkSurfaceCapabilitiesKHR m_Capabilities = {};
	std::vector<VkPresentModeKHR> m_PresentMode;
	std::vector<VkSurfaceFormatKHR> m_Formats;
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	void Create(::HWND hWnd, uint32_t uWidth, uint32_t uHeight, bool bWindowed = true);
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
protected:
	void CreateFrameBuffer(uint32_t width, uint32_t height, VkRenderPass renderPass);
private:
	VulkanSurface m_Surface;
	VkExtent2D m_Size = {};
	VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	VkFormat m_ColorSurfaceFormat = VK_FORMAT_UNDEFINED;
	VkFormat m_DepthSurfaceFormat = VK_FORMAT_UNDEFINED;
	bool m_bVSync = false;
	bool m_bFullScreen = false;
};
