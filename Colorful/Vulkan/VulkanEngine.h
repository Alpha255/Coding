#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"

class vkEngine : public rEngine, public singleton<vkEngine>
{
	singletonDeclare(vkEngine);
public:
	void initialize(uint64_t windowHandle, const appConfig &config) override final;
	void finalize() override final;

	void logError(uint32_t result) const override final;

	inline void handleWindowResize(uint32_t width, uint32_t height, const appConfig &config) override final
	{
		if (m_SwapChain->isValid())
		{
			m_SwapChain->recreate(width, height, config.VSync, config.FullScreen, m_PhysicalDevice, m_Device);
		}
	}

	inline rDevicePtr getDevice() const override final
	{
		assert(m_Device);
		return m_Device;
	}

	inline rContextPtr getIMContext() const override final
	{
		assert(m_IMContext);
		return m_IMContext;
	}
protected:
private:
	vkInstancePtr m_Instance = nullptr;
	vkDebugUtilsMessengerPtr m_DebugUtilsMessenger = nullptr;
	vkDevicePtr m_Device = nullptr;
	vkPhysicalDevicePtr m_PhysicalDevice = nullptr;
	vkDeviceQueuePtr m_GraphicsQueue = nullptr;
	vkDeviceQueuePtr m_ComputeQueue = nullptr;
	vkDeviceQueuePtr m_TransferQueue = nullptr;
	vkContextPtr m_IMContext = nullptr;
	vkSwapChainPtr m_SwapChain = nullptr;
};