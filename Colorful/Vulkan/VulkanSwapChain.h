#pragma once

#include "Colorful/Vulkan/VulkanView.h"

class vkSwapchain : public vkObject<VkSwapchainKHR>
{
public:
	void create(
		uint64_t appInstance, 
		uint64_t windowHandle, 
		uint32_t width,
		uint32_t height,
		bool8_t vSync,
		bool8_t fullscreen,
		const class vkInstance &instance,
		const class vkPhysicalDevice &physicalDevice,
		class vkDevice &device);

	void recreate(
		uint32_t width, 
		uint32_t height, 
		bool8_t vSync, 
		bool8_t fullscreen,
		const class vkPhysicalDevice &physicalDevice,
		class vkDevice &device);

	void destroy(const class vkInstance &instance, const class vkDevice &device);

	inline void resize(uint32_t width, uint32_t height, const class vkPhysicalDevice &physicalDevice, class vkDevice &device)
	{
		recreate(width, height, m_bVSync, m_bFullScreen, physicalDevice, device);
	}

	uint32_t acquireBackBuffer(const class vkDevice &device);
	inline const std::vector<vkBackBuffer> &getBackBuffers() const
	{
		return m_BackBuffers;
	}

	void present(const class vkDeviceQueue &queue, const class vkSemaphore &renderCompleteSephore) const;
	inline class vkSemaphore * getPresentCompleteSemaphore() const
	{
		return m_PresentCompleteSemaphore;
	}
protected:
	struct vkSurface : public vkObject<VkSurfaceKHR>
	{
		void create(uint64_t appInstance, uint64_t windowHandle, const class vkInstance &instance);

		void destroy(const class vkInstance &instance);

		VkSurfaceFormatKHR SurfaceFormat = {};
		VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};
		std::vector<VkPresentModeKHR> PresentModes;
	};

	void clearBackBuffers(const class vkDevice &device);
private:
	vkSurface m_Surface;
	std::vector<vkBackBuffer> m_BackBuffers;
	class vkSemaphore *m_PresentCompleteSemaphore = nullptr;
	bool8_t m_bVSync = false;
	bool8_t m_bFullScreen = false;
public:
	uint32_t m_CurrentFrameIndex = 0u;
};
