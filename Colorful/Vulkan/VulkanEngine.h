#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"

class vkEngine : public rEngine, public singleton<vkEngine>
{
	singletonDeclare(vkEngine);
public:
	void initialize(uint64_t windowHandle, const appConfig &config) override final;
	void finalize() override final {}

	void logError(uint32_t result) const override final;

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
	vkDebugReportCallbackPtr m_DebugReportCallback = nullptr;
	vkDevicePtr m_Device = nullptr;
	vkPhysicalDevicePtr m_PhysicalDevice = nullptr;
	vkDeviceQueuePtr m_GraphicsQueue = nullptr;
	vkDeviceQueuePtr m_ComputeQueue = nullptr;
	vkDeviceQueuePtr m_TransferQueue = nullptr;
	vkContextPtr m_IMContext = nullptr;
};