#include "VulkanEngine.h"

std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> VulkanEngine::s_Instance;
::HMODULE VulkanEngine::s_Library = nullptr;

void VulkanEngine::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	m_Instance.Create();

	m_Device.Create();

	for (uint32_t i = 0U; i < m_CommandPools.size(); ++i)
	{
		m_CommandPools[i].Create((VulkanCommandPool::ePoolType)i);
	}

	m_Swapchain.Initialize(hWnd, width, height, bWindowed);

	m_RenderPass.Create();

	m_Swapchain.Create();

	m_Context.Initialize();

	VulkanStaticState::Initialize();
}

void VulkanEngine::Finalize()
{
	VulkanStaticState::Finalize();

	m_Context.Finalize();

	m_RenderPass.Destory();
	m_Swapchain.Destory();

	for (uint32_t i = 0U; i < m_CommandPools.size(); ++i)
	{
		m_CommandPools[i].Destory();
	}

	m_Device.Destory();

	m_Instance.Destory();
}

void vkEngine::initialize(uint64_t, const gear::appConfig &)
{
	m_Instance = std::make_shared<vkInstance>();
	m_Instance->create();

	m_DebugReportCallback = std::make_shared<vkDebugReportCallback>();
	m_DebugReportCallback->create(m_Instance);

	std::vector<vkPhysicalDevicePtr> physicalDevicePtrs;
	std::vector<VkPhysicalDevice> physicalDeviceHandles;

	uint32_t count = 0U;
	rVerifyVk(vkEnumeratePhysicalDevices(&(*m_Instance), &count, nullptr));
	assert(count > 0U);
	physicalDevicePtrs.resize(count);
	physicalDeviceHandles.resize(count);
	rVerifyVk(vkEnumeratePhysicalDevices(&(*m_Instance), &count, physicalDeviceHandles.data()));
	for (uint32_t i = 0u; i < count; ++i)
	{
		physicalDevicePtrs[i] = std::make_shared<vkPhysicalDevice>();
		physicalDevicePtrs[i]->reset(physicalDeviceHandles[i]);
	}
	
	m_Device = std::make_shared<vkDevice>();
	m_Device->create(physicalDevicePtrs);

	//uint32_t graphicsQueueIndex = 0u;
	//uint32_t computeQueueIndex = 0u;
	//uint32_t transferQueueIndex = 0u;

	for each (auto handle in physicalDeviceHandles)
	{
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, queueFamilyProperties.data());

		for (uint32_t i = 0U; i < queueFamilyProperties.size(); ++i)
		{
			if (queueFamilyProperties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
			{
				///m_QueueFamilyIndex = i;
				break;
			}
		}
	}
	//assert(m_QueueFamilyIndex != UINT32_MAX);
	//Reset(m_PhysicalDevices[m_QueueFamilyIndex]);
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
