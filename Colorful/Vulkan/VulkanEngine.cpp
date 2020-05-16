#include "VulkanEngine.h"
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"

void VulkanEngine::initialize(uint64_t windowHandle, const Gear::Configurations& config)
{
#if defined(UsingVkLoader)
	VulkanLoader::initializeGlobalFunctionTable();
#endif

	m_Instance.create(config.VulkanValidationVerbose);

	m_Device.create(m_Instance.Handle);

	m_Swapchain = std::make_shared<VulkanSwapchain>(
		windowHandle,
		config.WindowWidth,
		config.WindowHeight,
		config.VSync,
		config.FullScreen,
		m_Instance.Handle,
		m_Device.physicalDevice(),
		m_Device.logicalDevice());
}

void VulkanEngine::logError(uint32_t result)
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

	Logger::instance().log(Logger::eError, "Failed to invoke VulkanAPI, VKResult = %s", errorMsg.c_str());
	assert(0);
}

void VulkanEngine::present()
{
	///m_GraphicsQueue.submit(m_Swapchain);
	///VulkanQueueManager::instance()->gfxQueue()->submit(m_Swapchain);
}

template<class TVector> void free(TVector& vector, VkDevice device)
{
	for each (auto it in vector)
	{
		it->destroy(device);
	}
	vector.clear();
}

void VulkanEngine::freeResources()
{
	free(m_RenderPassList, m_Device.logicalDevice());
	free(m_ImageViewList, m_Device.logicalDevice());
	free(m_SamplerList, m_Device.logicalDevice());
}

//void VulkanEngine::createOpaqueRenderPass()
//{
//	GfxFrameBufferDesc frameBufferDesc{};
//	frameBufferDesc.Width = m_Swapchain->width();
//	frameBufferDesc.Height = m_Swapchain->height();
//	frameBufferDesc.DepthSurface = createDepthStencilView(frameBufferDesc.Width, frameBufferDesc.Height, eD24_UNorm_S8_UInt);
//
//	m_OpaqueRenderPass = m_Device.createRenderPass(m_Swapchain, frameBufferDesc);
//}

void VulkanEngine::finalize()
{
	m_Device.waitIdle();

	freeResources();

	m_Swapchain->destroy(m_Instance.Handle);

	m_Device.destroy();

	m_Instance.destroy();

#if defined(UsingVkLoader)
	VulkanLoader::finalize();
#endif
}
