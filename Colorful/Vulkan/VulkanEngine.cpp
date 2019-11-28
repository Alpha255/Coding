#include "VulkanEngine.h"

void vkEngine::initialize(uint64_t windowHandle, const appConfig &config)
{
	m_Instance = std::make_shared<vkInstance>();
	m_Instance->create();

#if defined(_DEBUG)
	m_DebugUtilsMessenger = std::make_shared<vkDebugUtilsMessenger>();
	m_DebugUtilsMessenger->create(m_Instance, config.VulkanValidationVerbose);
#endif

	auto physicalDevicePtrs = vkPhysicalDevice::enumeratePhysicalDevices(m_Instance);
	assert(physicalDevicePtrs.size() == 1u); /// Only allow single video card for now...

	uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t computeQueueFamilyIndex = UINT32_MAX;
	uint32_t transferQueueFamilyIndex = UINT32_MAX;
	m_Device = std::make_shared<vkDevice>();
	uint32_t gpuIndex = m_Device->create(
		physicalDevicePtrs, 
		graphicsQueueFamilyIndex,
		computeQueueFamilyIndex,
		transferQueueFamilyIndex);
	assert(graphicsQueueFamilyIndex != UINT32_MAX);

	m_GraphicsQueue = std::make_shared<vkDeviceQueue>(graphicsQueueFamilyIndex, m_Device);
	if (computeQueueFamilyIndex != UINT32_MAX)
	{
		m_ComputeQueue = std::make_shared<vkDeviceQueue>(computeQueueFamilyIndex, m_Device);
	}
	if (transferQueueFamilyIndex != UINT32_MAX)
	{
		m_TransferQueue = std::make_shared<vkDeviceQueue>(transferQueueFamilyIndex, m_Device);
	}

	assert(gpuIndex != UINT32_MAX);
	m_PhysicalDevice = physicalDevicePtrs[gpuIndex];

	m_SwapChain = std::make_shared<vkSwapchain>();
	m_SwapChain->create(
		getAppInstance(),
		windowHandle,
		config.WindowWidth,
		config.WindowHeight,
		config.VSync,
		config.FullScreen,
		m_Instance,
		m_PhysicalDevice,
		m_Device);
}

void vkEngine::logError(uint32_t result) const
{
	std::string errorMsg;

#define vkResultCaseMessage(enumValue)               \
case enumValue:                                      \
	errorMsg = std::string(enumToString(enumValue)); \
	break;

	switch (result)
	{
		vkResultCaseMessage(VK_SUCCESS);
		vkResultCaseMessage(VK_NOT_READY);
		vkResultCaseMessage(VK_TIMEOUT);
		vkResultCaseMessage(VK_EVENT_SET);
		vkResultCaseMessage(VK_EVENT_RESET);
		vkResultCaseMessage(VK_INCOMPLETE);
		vkResultCaseMessage(VK_ERROR_OUT_OF_HOST_MEMORY);
		vkResultCaseMessage(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		vkResultCaseMessage(VK_ERROR_INITIALIZATION_FAILED);
		vkResultCaseMessage(VK_ERROR_DEVICE_LOST);
		vkResultCaseMessage(VK_ERROR_MEMORY_MAP_FAILED);
		vkResultCaseMessage(VK_ERROR_LAYER_NOT_PRESENT);
		vkResultCaseMessage(VK_ERROR_EXTENSION_NOT_PRESENT);
		vkResultCaseMessage(VK_ERROR_FEATURE_NOT_PRESENT);
		vkResultCaseMessage(VK_ERROR_INCOMPATIBLE_DRIVER);
		vkResultCaseMessage(VK_ERROR_TOO_MANY_OBJECTS);
		vkResultCaseMessage(VK_ERROR_FORMAT_NOT_SUPPORTED);
		vkResultCaseMessage(VK_ERROR_FRAGMENTED_POOL);
		vkResultCaseMessage(VK_ERROR_OUT_OF_POOL_MEMORY);
		vkResultCaseMessage(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		vkResultCaseMessage(VK_ERROR_SURFACE_LOST_KHR);
		vkResultCaseMessage(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		vkResultCaseMessage(VK_SUBOPTIMAL_KHR);
		vkResultCaseMessage(VK_ERROR_OUT_OF_DATE_KHR);
		vkResultCaseMessage(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		vkResultCaseMessage(VK_ERROR_VALIDATION_FAILED_EXT);
		vkResultCaseMessage(VK_ERROR_INVALID_SHADER_NV);
		vkResultCaseMessage(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
		vkResultCaseMessage(VK_ERROR_FRAGMENTATION_EXT);
		vkResultCaseMessage(VK_ERROR_NOT_PERMITTED_EXT);
		vkResultCaseMessage(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
		default:
			errorMsg = "Unknown error code.";
			assert(0);
			break;
	}

#undef vkResultCaseMessage

	logger::instance().log(logger::eError, "Faile to invoke VulkanAPI, error message: ", errorMsg.c_str());
}

void vkEngine::finalize()
{
	m_Device->waitIdle();

	m_SwapChain->destory(m_Instance, m_Device);

	m_Device->destory();

#if defined(DEBUG)
	m_DebugUtilsMessenger->destory(m_Instance);
#endif

	m_Instance->destory();
}
