#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanRenderPass.h"
#include "Colorful/Vulkan/VulkanTexture.h"

NAMESPACE_START(Gfx)

class VulkanSurface final : public VkDeviceObject<VkSurfaceKHR_T>
{
public:
	VulkanSurface(uint64_t windowHandle);
	~VulkanSurface();
protected:
private:
};

class VulkanSwapchain final : public VkDeviceObject<VkSwapchainKHR_T>
{
public:
	VulkanSwapchain(
		uint64_t windowHandle, 
		uint32_t width,
		uint32_t height,
		bool8_t fullscreen,
		bool8_t VSync,
		bool8_t sRGB);
	~VulkanSwapchain();

	void resize(uint32_t width, uint32_t height)
	{
		if ((width && height) && (m_Width != width || m_Height != height))
		{
			m_Width = width;
			m_Height = height;

			recreate();
		}
	}

	inline uint32_t width() const
	{
		return m_Width;
	}

	inline uint32_t height() const
	{
		return m_Height;
	}

	inline uint32_t currentImageIndex() const
	{
		return m_CurImageIndex;
	}

	inline VkFormat colorFormat() const
	{
		return m_ColorFormat;
	}

	inline VkFormat depthStencilFormat() const
	{
		return m_DepthStencilFormat;
	}

	VkFramebuffer currentFramebuffer(VkRenderPass renderPass);

	void present();
protected:
	void create();
	void recreate();
	VkColorSpaceKHR getProperFormats();
private:
	struct VkBackBuffer
	{
		std::vector<VulkanTexturePtr> ColorTargets;
		VulkanTexturePtr DepthStencil = nullptr;

		std::vector<VulkanFramebufferPtr> FrameBuffers;

		void clear()
		{
			ColorTargets.clear();
			FrameBuffers.clear();

			if (DepthStencil)
			{
				DepthStencil.reset();
			}
		}
	};

	bool8_t m_VSync = false;
	bool8_t m_FullScreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_CurImageIndex = 0u;
	VkFormat m_ColorFormat = VkFormat::VK_FORMAT_UNDEFINED;
	VkFormat m_DepthStencilFormat = VkFormat::VK_FORMAT_UNDEFINED;

	const uint64_t m_WindowHandle;

	VulkanSurfacePtr m_Surface;
	VulkanSemaphorePtr m_PresentComplete;
	VkBackBuffer m_BackBuffer;
};

NAMESPACE_END(Gfx)
