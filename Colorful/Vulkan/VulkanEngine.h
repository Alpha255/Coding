#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanLoader.h"

class vkEngine : public rEngine, public singleton<vkEngine>
{
	singletonDeclare(vkEngine);
public:
	void initialize(uint64_t windowHandle, const appConfig &config) override final;
	void finalize() override final;

	void logError(uint32_t result) const override final;

	inline void handleWindowResize(uint32_t width, uint32_t height, const appConfig &config) override final
	{
		if (m_Swapchain.isValid())
		{
			m_Swapchain.recreate(width, height, config.VSync, config.FullScreen, m_PhysicalDevice, m_Device);
		}
	}

	inline rDevice *getDevice() override final
	{
		assert(m_Device.isValid());
		return &m_Device;
	}
protected:
	class vkLoader
	{
	public:
		static void initializeGlobalFunctionTable();
		static void initializeInstanceFunctionTable(const vkInstance &instance);
		static void initializeDeviceFunctionTable(const vkDevice &device);

		static void finalize();
	};
private:
	vkInstance m_Instance;
	vkDebugUtilsMessenger m_DebugUtilsMessenger;
	vkDevice m_Device;
	vkPhysicalDevice m_PhysicalDevice;
	vkDeviceQueue m_GraphicsQueue;
	vkDeviceQueue m_ComputeQueue;
	vkDeviceQueue m_TransferQueue;
	vkContext m_IMContext;
	vkSwapchain m_Swapchain;
};