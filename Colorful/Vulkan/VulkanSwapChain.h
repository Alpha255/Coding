#pragma once

#include "Colorful/Vulkan/VulkanView.h"

class VulkanSwapchain : public VulkanObject<VkSwapchainKHR>
{
public:
	VulkanSwapchain(
		uint64_t windowHandle, 
		uint32_t width,
		uint32_t height,
		bool8_t VSync,
		bool8_t fullscreen,
		VkInstance instance,
		const VkPhysicalDevice physicalDevice,
		const VkDevice device);

	void recreate();

	void destroy(VkInstance instance);

	inline void resize(uint32_t width, uint32_t height)
	{
		if (width != m_Width || height != height)
		{
			///recreate(width, height, m_bVSync, m_bFullScreen, physicalDevice, device);
		}
	}

	uint32_t acquireNextFrame();
	inline const std::vector<VulkanImageView> &getBackBuffers() const
	{
		return m_BackBuffers;
	}

	void present(const class VulkanSemaphore &renderCompleteSephore) const;
	inline class VulkanSemaphore * getPresentCompleteSemaphore() const
	{
		return m_PresentCompleteSemaphore;
	}

	inline uint32_t width() const
	{
		return m_Width;
	}

	inline uint32_t height() const
	{
		return m_Height;
	}
protected:
	struct VulkanSurface : public VulkanObject<VkSurfaceKHR>
	{
		void create(uint64_t windowHandle, VkInstance instance);

		void destroy(VkInstance instance);

		VkSurfaceFormatKHR SurfaceFormat = {};
		VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};
		std::vector<VkPresentModeKHR> PresentModes;
	};

	void clearBackBuffers();
private:
	VulkanSurface m_Surface;
	std::vector<VulkanImageView> m_BackBuffers;
	class VulkanSemaphore *m_PresentCompleteSemaphore = nullptr;
	bool8_t m_VSync = false;
	bool8_t m_FullScreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
public:
	uint32_t m_CurrentFrameIndex = 0u;
	const VkDevice m_LogicDevice;
	const VkPhysicalDevice m_PhysicalDevice;
};
using VulkanSwapchainPtr = std::shared_ptr<VulkanSwapchain>;
