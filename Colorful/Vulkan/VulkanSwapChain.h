#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(VulkanSurface)
class VulkanSurface : public VkObject<VkSurfaceKHR_T>
{
public:
	VulkanSurface(uint64_t appInstance, uint64_t windowHandle, VkInstance instance);

	~VulkanSurface()
	{
		vkDestroySurfaceKHR(m_Instance, get(), VK_MEMORY_ALLOCATOR);
	}
protected:
private:
	VkSurfaceFormatKHR m_SurfaceFormat{};
	VkSurfaceCapabilitiesKHR m_SurfaceCapabilities{};
	std::vector<VkPresentModeKHR> m_PresentModes{};
	const VkInstance m_Instance;
};

DECLARE_UNIQUE_PTR(VulkanSwapchain)
class VulkanSwapchain : public VkObject<VkSwapchainKHR_T>
{
public:
	VulkanSwapchain(
		uint64_t windowHandle, 
		uint32_t width,
		uint32_t height,
		bool8_t VSync,
		bool8_t fullscreen,
		const VkPhysicalDevice physicalDevice,
		const VkDevice device);

#if 0
	void recreate();

	void destroy(VkInstance instance);

	inline void resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		recreate();
	}

	uint32_t acquireNextFrame();

	void present(VkSemaphore renderCompleteSephore) const;

	inline const VulkanSemaphorePtr& presentCompleteSemaphore() const
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

	inline VulkanFrameBufferPtr backBuffer() const
	{
		return m_BackBuffers[m_CurrentFrameIndex];
	}
protected:
	void destroyBackBuffers();
private:
	const VkDevice m_LogicDevice;
	const VkPhysicalDevice m_PhysicalDevice;

	VulkanSurface m_Surface;
	std::vector<VulkanImageViewPtr> m_BackBufferImages;
	VulkanImageViewPtr m_DepthStencil;
	VulkanSemaphorePtr m_PresentCompleteSemaphore = nullptr;
	std::vector<VulkanFrameBufferPtr> m_BackBuffers;

	bool8_t m_VSync = false;
	bool8_t m_FullScreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_CurrentFrameIndex = 0u;
#endif
};

NAMESPACE_END(Gfx)
