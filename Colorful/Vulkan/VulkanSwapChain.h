#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(VulkanSurface)
class VulkanSurface final : public VkObject<VkSurfaceKHR_T>
{
public:
	VulkanSurface(uint64_t appInstance, uint64_t windowHandle, VkInstance instance);

	~VulkanSurface()
	{
		vkDestroySurfaceKHR(m_Instance, get(), VK_MEMORY_ALLOCATOR);
	}
protected:
private:
	const VkInstance m_Instance;
};

DECLARE_UNIQUE_PTR(VulkanSwapchain)
class VulkanSwapchain final : public VkObject<VkSwapchainKHR_T>
{
public:
	VulkanSwapchain(
		uint64_t appInstance,
		uint64_t windowHandle, 
		uint32_t width,
		uint32_t height,
		bool8_t fullscreen,
		bool8_t VSync,
		bool8_t sRGB,
		const VkInstance instance,
		const VkPhysicalDevice physicalDevice,
		const VkDevice device);

	~VulkanSwapchain()
	{
		destroyResources(); /// ???
		vkDestroySwapchainKHR(m_Device, get(), VK_MEMORY_ALLOCATOR);
	}

	void create();

	void resize(uint32_t width, uint32_t height)
	{
		if ((width && height) && (m_Width != width || m_Height != height))
		{
			m_Width = width;
			m_Height = height;

			recreate();
		}
	}
#if 0
	uint32_t acquireNextFrame();

	void present(VkSemaphore renderCompleteSephore) const;

	inline const VulkanSemaphorePtr& presentCompleteSemaphore() const
	{
		return m_PresentCompleteSemaphore;
	}

	inline VulkanFrameBufferPtr backBuffer() const
	{
		return m_BackBuffers[m_CurrentFrameIndex];
	}
protected:
	std::vector<VulkanImageViewPtr> m_BackBufferImages;
	VulkanImageViewPtr m_DepthStencil;
	VulkanSemaphorePtr m_PresentCompleteSemaphore = nullptr;
	std::vector<VulkanFrameBufferPtr> m_BackBuffers;
	uint32_t m_CurrentFrameIndex = 0u;
#endif
protected:
	void recreate();

	void createResources();
	void destroyResources();
private:
	bool8_t m_VSync = false;
	bool8_t m_FullScreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	VkFormat m_ColorFormat;

	const uint64_t m_AppInstance;
	const uint64_t m_WindowHandle;

	const VkInstance m_Instance;
	const VkPhysicalDevice m_PhysicalDevice;
	const VkDevice m_Device;

	VulkanSurfacePtr m_Surface;
};

NAMESPACE_END(Gfx)
