#if 0
#include "Colorful/Vulkan/VulkanEngine.h"
#include "Colorful/Vulkan/VulkanImage.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "VulkanRenderer.h"

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

	m_ImGuiRenderer = std::make_unique<ImGuiRenderer>();

	if (m_ActiveCmdBuffer == nullptr)
	{
		m_ActiveCmdBuffer = VulkanQueueManager::instance()->allocGfxCommandBuffer();
		assert(m_ActiveCmdBuffer);
	}
}

void VulkanEngine::present()
{
	VulkanBufferPool::instance()->delayFree();

	VulkanQueueManager::instance()->queueGfxCommand(m_ActiveCmdBuffer);

	VulkanQueueManager::instance()->submitQueuedCommands(m_Swapchain->presentCompleteSemaphore()->Handle);

	m_Swapchain->present(VulkanQueueManager::instance()->renderCompleteSemaphore()->Handle);

	VulkanQueueManager::instance()->gfxQueue()->waitIdle();

	m_CurPipelineState.reset();
}

void VulkanEngine::bindGfxPipelineState(const GfxPipelineState* state)
{
	assert(state);

	if (m_CurPipelineState.GfxPipelineState != state)
	{
		m_CurPipelineState.Dirty = true;
		m_CurPipelineState.GfxPipelineState = state;
		assert(m_CurPipelineState.GfxPipelineState->FrameBuffer);
		m_CurPipelineState.FrameBuffer = std::static_pointer_cast<VulkanFrameBuffer>(state->FrameBuffer);
		m_CurPipelineState.GfxPipeline = VulkanPipelinePool::instance()->getOrCreateGfxPipeline(m_CurPipelineState.FrameBuffer->renderPass(), state);
	}

	if (!m_ActiveCmdBuffer->isInsideRenderPass())
	{
		VkClearValue clearValue[2u]{};
		clearValue[0].color =
		{
			m_CurPipelineState.GfxPipelineState->ClearValue.Color.x,
			m_CurPipelineState.GfxPipelineState->ClearValue.Color.y,
			m_CurPipelineState.GfxPipelineState->ClearValue.Color.z,
			m_CurPipelineState.GfxPipelineState->ClearValue.Color.w,
		};
		clearValue[1].depthStencil =
		{
			m_CurPipelineState.GfxPipelineState->ClearValue.Depth,
			m_CurPipelineState.GfxPipelineState->ClearValue.Stencil
		};

		VkRenderPassBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			m_CurPipelineState.FrameBuffer->renderPass(),
			m_CurPipelineState.FrameBuffer->Handle,
			{
				{
					0,
					0,
				},
				{
					m_CurPipelineState.FrameBuffer->width(),
					m_CurPipelineState.FrameBuffer->height(),
				}
			},
			_countof(clearValue),
			clearValue
		};

		m_ActiveCmdBuffer->beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
}

void VulkanEngine::setDynamicStates()
{
	m_CurPipelineState.Dynamic.setViewport(m_CurPipelineState.GfxPipelineState->Viewport);
	if (m_CurPipelineState.Dynamic.isDirty(CurrentPipelineState::DynamicState::eViewport))
	{
		VkViewport viewport
		{
			m_CurPipelineState.GfxPipelineState->Viewport.x,
			m_CurPipelineState.GfxPipelineState->Viewport.y,
			m_CurPipelineState.GfxPipelineState->Viewport.z,
			m_CurPipelineState.GfxPipelineState->Viewport.w,
			m_CurPipelineState.GfxPipelineState->Viewport.minDepth(),
			m_CurPipelineState.GfxPipelineState->Viewport.maxDepth()
		};
		vkCmdSetViewport(m_ActiveCmdBuffer->Handle, 0u, 1u, &viewport);
	}

	m_CurPipelineState.Dynamic.setScissor(m_CurPipelineState.GfxPipelineState->Scissor);
	if (m_CurPipelineState.Dynamic.isDirty(CurrentPipelineState::DynamicState::eScissor))
	{
		VkRect2D scissor
		{
			{
				(int32_t)m_CurPipelineState.GfxPipelineState->Scissor.x,
				(int32_t)m_CurPipelineState.GfxPipelineState->Scissor.y
			},
			{
				(uint32_t)m_CurPipelineState.GfxPipelineState->Scissor.z,
				(uint32_t)m_CurPipelineState.GfxPipelineState->Scissor.w
			}
		};
		vkCmdSetScissor(m_ActiveCmdBuffer->Handle, 0u, 1u, &scissor);
	}
}

void VulkanEngine::prepareForDraw()
{
	assert(m_CurPipelineState.GfxPipeline);

	m_CurPipelineState.GfxPipeline->updateDescriptorSet(m_CurPipelineState.GfxPipelineState);
	auto descriptorSet = m_CurPipelineState.GfxPipeline->descriptorSet();

	if (m_CurPipelineState.Dirty)
	{
		if (m_CurPipelineState.GfxPipelineState->Wireframe)
		{
			vkCmdBindPipeline(m_ActiveCmdBuffer->Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_CurPipelineState.GfxPipeline->wireframePipeline());
		}
		else
		{
			vkCmdBindPipeline(m_ActiveCmdBuffer->Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_CurPipelineState.GfxPipeline->Handle);
		}

		vkCmdBindDescriptorSets(m_ActiveCmdBuffer->Handle,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_CurPipelineState.GfxPipeline->layout(),
			0u,
			1u,
			&descriptorSet,
			0u,
			nullptr);

		m_CurPipelineState.CurDescriptorSet = descriptorSet;
		m_CurPipelineState.Dirty = false;
	}
	else
	{
		if (m_CurPipelineState.CurDescriptorSet != descriptorSet)
		{
			vkCmdBindDescriptorSets(m_ActiveCmdBuffer->Handle,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_CurPipelineState.GfxPipeline->layout(),
				0u,
				1u,
				&descriptorSet,
				0u,
				nullptr);
			m_CurPipelineState.CurDescriptorSet = descriptorSet;
		}
	}

	setDynamicStates();
}

void VulkanEngine::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex)
{
	prepareForDraw();

	m_CurPipelineState.Dynamic.setVertexBuffer(m_CurPipelineState.GfxPipelineState->VertexBuffer);
	if (m_CurPipelineState.Dynamic.isDirty(CurrentPipelineState::DynamicState::eVertexBuffer))
	{
		assert(m_CurPipelineState.GfxPipelineState->VertexBuffer);
		auto vertexBuffer = static_cast<VulkanBufferPtr>(m_CurPipelineState.GfxPipelineState->VertexBuffer);
		VkDeviceSize offsets[1u]{};
		vkCmdBindVertexBuffers(m_ActiveCmdBuffer->Handle, 0u, 1u, &vertexBuffer->Handle, offsets);
	}

	vkCmdDraw(
		m_ActiveCmdBuffer->Handle,
		vertexCount,
		instanceCount,
		firstVertex,
		1u);
}

void VulkanEngine::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset)
{
	prepareForDraw();

	m_CurPipelineState.Dynamic.setVertexBuffer(m_CurPipelineState.GfxPipelineState->VertexBuffer);
	if (m_CurPipelineState.Dynamic.isDirty(CurrentPipelineState::DynamicState::eVertexBuffer))
	{
		assert(m_CurPipelineState.GfxPipelineState->VertexBuffer);
		auto vertexBuffer = static_cast<VulkanBufferPtr>(m_CurPipelineState.GfxPipelineState->VertexBuffer);
		VkDeviceSize offsets[1u]{};
		vkCmdBindVertexBuffers(m_ActiveCmdBuffer->Handle, 0u, 1u, &vertexBuffer->Handle, offsets);
	}

	m_CurPipelineState.Dynamic.setIndexBuffer(m_CurPipelineState.GfxPipelineState->IndexBuffer);
	if (m_CurPipelineState.Dynamic.isDirty(CurrentPipelineState::DynamicState::eIndexBuffer))
	{
		assert(m_CurPipelineState.GfxPipelineState->IndexBuffer);
		auto indexBuffer = static_cast<VulkanBufferPtr>(m_CurPipelineState.GfxPipelineState->IndexBuffer);
		vkCmdBindIndexBuffer(m_ActiveCmdBuffer->Handle,
			indexBuffer->Handle,
			0u,
			m_CurPipelineState.GfxPipelineState->IndexType == eRIndexType::eUInt16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	}

	vkCmdDrawIndexed(
		m_ActiveCmdBuffer->Handle,
		indexCount,
		instanceCount,
		firstIndex,
		vertexOffset,
		1u);
}

void VulkanEngine::beginDebugMarker(const char8_t* name, Vec4 color)
{
	if (m_Device.isDebugMakerAvaliable() && m_ActiveCmdBuffer)
	{
		VkDebugMarkerMarkerInfoEXT info
		{
			VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
			nullptr,
			name,
			{ color.x, color.y, color.z, color.w }
		};
		vkCmdDebugMarkerBeginEXT(m_ActiveCmdBuffer->Handle, &info);
	}
}

void VulkanEngine::insertDebugMarker(const char8_t* name, Vec4 color)
{
	if (m_Device.isDebugMakerAvaliable() && m_ActiveCmdBuffer)
	{
		VkDebugMarkerMarkerInfoEXT info
		{
			VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
			nullptr,
			name,
			{ color.x, color.y, color.z, color.w }
		};
		vkCmdDebugMarkerInsertEXT(m_ActiveCmdBuffer->Handle, &info);
	}
}

void VulkanEngine::endDebugMarker()
{
	if (m_Device.isDebugMakerAvaliable() && m_ActiveCmdBuffer)
	{
		vkCmdDebugMarkerEndEXT(m_ActiveCmdBuffer->Handle);
	}
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

#endif

#include "Colorful/Vulkan/VulkanRenderer.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"

NAMESPACE_START(Gfx)

void VulkanRender::createDevice()
{
#if defined(USE_VK_LOADER)
	VulkanLoader::loadGlobalFuncs();
#endif

	m_Instance = std::make_unique<VulkanInstance>();
#if defined(USE_VK_LOADER)
	VulkanLoader::loadInstanceFuncs(m_Instance->get());
#endif

	m_Device = std::make_unique<VulkanDevice>(m_Instance->get());
#if defined(USE_VK_LOADER)
	VulkanLoader::loadDeviceFuncs(m_Device->get());
#endif

	m_ResourceManager = std::make_unique<VulkanResourceManager>(m_Device->get(), m_Device->physicalDevice());
	///GResourceManager = m_ResourceManager.get();

	VulkanMemoryAllocator::initialize(m_Device->get(), m_Device->physicalDevice());
}

void VulkanRender::createSwapchain(uint64_t instance, uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync)
{
	m_Swapchain = std::make_unique<VulkanSwapchain>(
		instance, 
		windowHandle, 
		width, 
		height, 
		fullscreen, 
		VSync,
		false,
		m_Instance->get(),
		m_Device->physicalDevice(),
		m_Device->get());
}

void VulkanRender::toggleFullScreen(bool8_t fullscreen)
{
	(void)fullscreen;
}

void VulkanRender::toggleVSync(bool8_t VSync)
{
	(void)VSync;
}

void VulkanRender::finalize()
{
#if defined(USE_VK_LOADER)
	VulkanLoader::free();
#endif

	VulkanMemoryAllocator::instance()->finalize();
}

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		ptr.reset(new VulkanRender());
	}
}

NAMESPACE_END(Gfx)
