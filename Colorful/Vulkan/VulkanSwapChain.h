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

	void Destory() override
	{
		assert(IsValid());
		Reset();
	}
protected:
private:
	VkSurfaceCapabilitiesKHR m_SurfaceCapabilities = {};
	std::vector<VkPresentModeKHR> m_PresentModes;
	std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
};

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	void Create(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen);

	void Resize(uint32_t width, uint32_t height);

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

	//inline uint32_t GetColorSurfaceFormat() const
	//{
	//	return m_ColorSurfaceFormat;
	//}

	//inline uint32_t GetDepthSurfaceFormat() const
	//{
	//	return m_DepthSurfaceFormat;
	//}

	//inline VulkanCommandBuffer GetCurCommandBuffer() const
	//{
	//	return m_BackBuffers[m_CurBackBufferIndex].CommandBuffer;
	//}

	void Destory() override;
protected:
	struct VulkanBackBuffer
	{
		VulkanRenderTargetView RenderTargetView;
		VulkanFence PresentFence;
		VulkanSemaphore ImageAcquireSemaphore;
		VulkanSemaphore RenderCompleteSemaphore;
		VulkanCommandBuffer CommandBuffer;
		///VulkanFrameBuffer FrameBuffer;
	};
private:
	VulkanSurface m_Surface;
	VkExtent2D m_Size = {};
	//VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
	//VkFormat m_ColorSurfaceFormat = VK_FORMAT_UNDEFINED;
	//VkFormat m_DepthSurfaceFormat = VK_FORMAT_UNDEFINED;
	//VulkanDepthStencilView m_DepthStencilView;
	std::vector<VulkanBackBuffer> m_BackBuffers;
	//VulkanImage m_DepthStencilImage;
	uint32_t m_Index = 0U;
	bool m_bVSync = false;
	bool m_bFullScreen = false;
};
