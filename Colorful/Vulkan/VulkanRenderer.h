#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"

NAMESPACE_START(Gfx)

class VulkanRenderer final : public GfxRenderer
{
public:
	void initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) override final;

	void toggleFullScreen(bool8_t fullscreen) override final;
	void toggleVSync(bool8_t VSync) override final;

	void finalize() override final;

	GfxDevice* device() override final
	{
		return m_Device.get();
	}
protected:
private:
	VulkanInstancePtr m_Instance = nullptr;
	VulkanDebugUtilsMessengerPtr m_DebugUtilsMessenger = nullptr;
	VulkanDevicePtr m_Device = nullptr;
	VulkanSwapchainPtr m_Swapchain = nullptr;
};

NAMESPACE_END(Gfx)
