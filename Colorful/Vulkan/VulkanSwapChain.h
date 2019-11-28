#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"

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
		const vkInstancePtr &instancePtr,
		const vkPhysicalDevicePtr &physicalDevicePtr,
		const vkDevicePtr &devicePtr);

	void recreate(uint32_t width, uint32_t height, bool8_t vSync, bool8_t fullscreen, const vkDevicePtr &devicePtr);

	inline void resize(uint32_t width, uint32_t height, const vkDevicePtr &devicePtr)
	{
		recreate(width, height, m_VSync, m_FullScreen, devicePtr);
	}
protected:
	struct vkSurface : public vkObject<VkSurfaceKHR>
	{
		void create(uint64_t appInstance, uint64_t windowHandle, const vkInstancePtr &instancePtr);

		VkSurfaceFormatKHR SurfaceFormat = {};
		VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};
		std::vector<VkPresentModeKHR> PresentModes;
	};
	typedef std::shared_ptr<vkSurface> vkSurfacePtr;
private:
	vkSurfacePtr m_Surface = nullptr;
	bool8_t m_VSync = false;
	bool8_t m_FullScreen = false;
};
typedef std::shared_ptr<vkSwapchain> vkSwapChainPtr;
