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

void VulkanEngine::bindGfxPipelineState(GfxPipelineState* state)
{
	if (m_ActiveCmdBuffer == nullptr)
	{
		m_ActiveCmdBuffer = VulkanQueueManager::instance()->allocGfxCommandBuffer();
	}

	m_CurrentPipelineState = state;
	assert(m_CurrentPipelineState->FrameBuffer);

	auto frameBuffer = std::static_pointer_cast<VulkanFrameBuffer>(m_CurrentPipelineState->FrameBuffer);
	m_CurrentPipeline = VulkanPipelinePool::instance()->getOrCreateGfxPipeline(frameBuffer->renderPass(), *m_CurrentPipelineState);
	m_CurrentPipeline->setupDescriptorSet(m_Device.logicalDevice(), m_CurrentPipelineState);
}

void VulkanEngine::prepareForDraw()
{
	if (!m_ActiveCmdBuffer->isInsideRenderPass())
	{
		VkClearValue clearValue[2u]{};
		clearValue[0].color =
		{
			m_CurrentPipelineState->ClearValue.Color.x,
			m_CurrentPipelineState->ClearValue.Color.y,
			m_CurrentPipelineState->ClearValue.Color.z,
			m_CurrentPipelineState->ClearValue.Color.w,
		};
		clearValue[1].depthStencil =
		{
			m_CurrentPipelineState->ClearValue.Depth,
			m_CurrentPipelineState->ClearValue.Stencil
		};

		auto frameBuffer = std::static_pointer_cast<VulkanFrameBuffer>(m_CurrentPipelineState->FrameBuffer);
		VkRenderPassBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			frameBuffer->renderPass(),
			frameBuffer->Handle,
			{
				{
					0,
					0,
				},
				{
					frameBuffer->width(),
					frameBuffer->height(),
				}
			},
			ARRAYSIZE(clearValue),
			clearValue
		};

		m_ActiveCmdBuffer->beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	assert(m_CurrentPipeline);
	m_CurrentPipeline->bind(m_ActiveCmdBuffer);

	///if (m_CurrentPipeline.second->isDirty(GfxPipelineState::eVertexBuffer))
	{
		assert(m_CurrentPipelineState->VertexBuffer);
		auto vertexBuffer = static_cast<VulkanBufferPtr>(m_CurrentPipelineState->VertexBuffer);
		VkDeviceSize offsets[1u]{};
		vkCmdBindVertexBuffers(m_ActiveCmdBuffer->Handle, 0u, 1u, &vertexBuffer->Handle, offsets);
	}

	///if (m_CurrentPipeline.second->isDirty(GfxPipelineState::eIndexBuffer))
	{
		assert(m_CurrentPipelineState->IndexBuffer);
		auto indexBuffer = static_cast<VulkanBufferPtr>(m_CurrentPipelineState->IndexBuffer);
		vkCmdBindIndexBuffer(m_ActiveCmdBuffer->Handle, indexBuffer->Handle, 0u, m_CurrentPipelineState->IndexType == eRIndexType::eUInt16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	}

	if (m_CurrentPipelineState->isDirty(GfxPipelineState::eViewport))
	{
		VkViewport viewport
		{
			m_CurrentPipelineState->Viewport.x,
			m_CurrentPipelineState->Viewport.y,
			m_CurrentPipelineState->Viewport.z,
			m_CurrentPipelineState->Viewport.w,
			0.0f,
			1.0f
		};
		vkCmdSetViewport(m_ActiveCmdBuffer->Handle, 0u, 1u, &viewport);
	}

	if (m_CurrentPipelineState->isDirty(GfxPipelineState::eScissor))
	{
		VkRect2D scissor
		{
			{
				(int32_t)m_CurrentPipelineState->Scissor.x,
				(int32_t)m_CurrentPipelineState->Scissor.y
			},
			{
				(uint32_t)m_CurrentPipelineState->Scissor.z,
				(uint32_t)m_CurrentPipelineState->Scissor.w
			}
		};
		vkCmdSetScissor(m_ActiveCmdBuffer->Handle, 0u, 1u, &scissor);
	}

	m_CurrentPipelineState->clearDirty();

	VulkanQueueManager::instance()->queueGfxCommand(m_ActiveCmdBuffer);
}

void VulkanEngine::drawIndexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset)
{
	prepareForDraw();

	vkCmdDrawIndexed(
		m_ActiveCmdBuffer->Handle,
		indexCount,
		1u,
		firstIndex,
		vertexOffset,
		1u);
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
