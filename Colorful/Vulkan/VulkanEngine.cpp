#include "Colorful/Vulkan/VulkanEngine.h"
#include "Colorful/Vulkan/VulkanImage.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanBuffer.h"

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

	m_ImGuiRenderer = std::make_unique<ImGuiRenderer>(this);

	m_DefaultTexture = createTexture("white.dds");
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
	VulkanBufferPool::instance()->delayFree();

	VulkanQueueManager::instance()->submitQueuedCommands(m_Swapchain->presentCompleteSemaphore()->Handle);

	m_Swapchain->present(VulkanQueueManager::instance()->renderCompleteSemaphore()->Handle);

	VulkanQueueManager::instance()->gfxQueue()->waitIdle();
}

template<class TVector> void free(TVector& vector, VkDevice device)
{
	for each (auto it in vector)
	{
		it->destroy(device);
	}
	vector.clear();
}

void VulkanEngine::bindGfxPipelineState(const GfxPipelineState& state)
{
	if (m_ActiveCmdBuffer == nullptr)
	{
		m_ActiveCmdBuffer = VulkanQueueManager::instance()->allocGfxCommandBuffer();
	}
	assert(m_ActiveCmdBuffer->state() == VulkanCommandBuffer::eInitial);


}

void VulkanEngine::drawIndexed(uint32_t indexCount, uint32_t startVertex, int32_t vertexOffset)
{
}

void VulkanEngine::freeResources()
{
	free(m_RenderPassList, m_Device.logicalDevice());
	free(m_ImageViewList, m_Device.logicalDevice());
	free(m_SamplerList, m_Device.logicalDevice());

	for (auto it = m_RenderPassList_Hash.begin(); it != m_RenderPassList_Hash.end(); ++it)
	{
		it->second->destroy(m_Device.logicalDevice());
	}
	m_RenderPassList_Hash.clear();
}

VulkanRenderPassPtr VulkanEngine::getOrCreateRenderPass(const GfxFrameBufferDesc& desc)
{
	/// For temporary
	size_t hash = 0u;
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (desc.ColorSurface[i])
		{
			auto imageView = std::static_pointer_cast<VulkanImageView>(desc.ColorSurface[i]);
			hash_combine(hash, (uint32_t)imageView->format());
		}
	}

	if (desc.DepthSurface)
	{
		auto imageView = std::static_pointer_cast<VulkanImageView>(desc.DepthSurface);
		hash_combine(hash, (uint32_t)imageView->format());
	}

	auto it = m_RenderPassList_Hash.find(hash);
	if (it != m_RenderPassList_Hash.end())
	{
		return it->second;
	}

	auto renderPass = std::make_shared<VulkanRenderPass>(m_Device.logicalDevice(), desc);
	m_RenderPassList_Hash.insert(std::make_pair(hash, renderPass));
}

//void VulkanEngine::createOpaqueRenderPass()
//{
	//VulkanRenderPassPtr vkRenderPass = nullptr;
	//if (m_OpaqueRenderPass)
	//{
	//	vkRenderPass = std::static_pointer_cast<VulkanRenderPass>(m_OpaqueRenderPass);
	//	vkRenderPass->destroyFrameBuffers(m_Device.logicalDevice());
	//}

	//auto& backBuffers = m_Swapchain->backBuffers();

	//GfxFrameBufferDesc frameBufferDesc{};
	//frameBufferDesc.Width = m_Swapchain->width();
	//frameBufferDesc.Height = m_Swapchain->height();
	//frameBufferDesc.DepthSurface = createDepthStencilView(frameBufferDesc.Width, frameBufferDesc.Height, eD24_UNorm_S8_UInt);
	//frameBufferDesc.ColorSurface[0] = std::static_pointer_cast<GfxRenderSurface>(backBuffers[0]);

	//if (!vkRenderPass)
	//{
	//	m_OpaqueRenderPass = createRenderPass(frameBufferDesc);
	//	vkRenderPass = std::static_pointer_cast<VulkanRenderPass>(m_OpaqueRenderPass);
	//}

	//std::vector<VulkanFrameBuffer> frameBuffers(backBuffers.size());
	//for (uint32_t i = 0u; i < backBuffers.size(); ++i)
	//{
	//	frameBufferDesc.ColorSurface[0] = std::static_pointer_cast<GfxRenderSurface>(backBuffers[i]);
	//	frameBuffers[i].create(m_Device.logicalDevice(), vkRenderPass->Handle, frameBufferDesc);
	//}
	//vkRenderPass->bindFrameBuffers(frameBuffers);
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
