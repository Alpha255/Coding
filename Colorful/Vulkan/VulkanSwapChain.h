#pragma once

#include "VulkanView.h"
#include "VulkanAsync.h"

class VulkanSurface : public VulkanObject<VkSurfaceKHR>
{
public:
	void Create(::HWND hWnd);

	inline const VkSurfaceCapabilitiesKHR &GetCapabilities() const
	{
		assert(IsValid());
		return m_SurfaceCapabilities;
	}

	inline const std::vector<VkPresentModeKHR> &GetPresentModes() const
	{
		assert(IsValid());
		return m_PresentModes;
	}

	inline const std::vector<VkSurfaceFormatKHR> &GetFormats() const
	{
		assert(IsValid());
		return m_SurfaceFormats;
	}

	void Destory() override;
protected:
private:
	VkSurfaceCapabilitiesKHR m_SurfaceCapabilities = {};
	std::vector<VkPresentModeKHR> m_PresentModes;
	std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	struct VulkanBackBuffer
	{
		VulkanRenderTargetView RenderTargetView;
		VulkanFence PresentFence;
		VulkanCommandBuffer CommandBuffer;
		VulkanFrameBuffer FrameBuffer;
	};

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen);

	void Create();

	void Resize(uint32_t width, uint32_t height);

	void Flush();

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

	inline VkFormat GetColorFormat() const
	{
		assert(m_ColorFormat != VK_FORMAT_UNDEFINED);
		return m_ColorFormat;
	}

	inline VkFormat GetDepthStencilFormat() const
	{
		assert(m_DepthStencilFormat != VK_FORMAT_UNDEFINED);
		return m_DepthStencilFormat;
	}

	inline VkExtent2D GetSize() const
	{
		assert(IsValid());
		return m_Size;
	}

	inline std::vector<VulkanBackBuffer> GetBackBuffers() const
	{
		return m_BackBuffers;
	}

	void Destory() override;
protected:
private:
	VulkanSurface m_Surface;
	VkExtent2D m_Size = {};
	VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
	VkFormat m_ColorFormat = VK_FORMAT_UNDEFINED;
	VkFormat m_DepthStencilFormat = VK_FORMAT_UNDEFINED;
	std::vector<VulkanBackBuffer> m_BackBuffers;
	VulkanSemaphore m_PresentSemaphore;
	VulkanSemaphore m_WaitSemaphore;
	VulkanDepthStencilView m_DepthStencilView;
	uint32_t m_CurrentBackBufferIndex = 0U;
	::HWND m_hWnd = nullptr;

	bool m_bVSync = false;
	bool m_bFullScreen = false;
};
