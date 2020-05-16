#include "Colorful/Vulkan/VulkanEngine.h"

/*************************************************
	When defining a memory dependency, using only VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT would never 
	make any accesses available and/or visible because these stages do not access memory.

	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful for accomplishing layout transitions and queue 
	ownership operations when the required execution dependency is satisfied by other means - for example, semaphore operations between queues.


	A render pass represents a collection of attachments, subpasses, and dependencies between the subpasses, 
	and describes how the attachments are used over the course of the subpasses. 
	The use of a render pass in a command buffer is a render pass instance.

	A subpass represents a phase of rendering that reads and writes a subset of the attachments in a render pass. 
	Rendering commands are recorded into a particular subpass of a render pass instance.

	A subpass description describes the subset of attachments that is involved in the execution of a subpass. 
	Each subpass can read from some attachments as input attachments, write to some as color attachments or depth/stencil attachments, 
	and perform multisample resolve operations to resolve attachments. A subpass description can also include a set of preserve attachments, 
	which are attachments that are not read or written by the subpass but whose contents must be preserved throughout the subpass.

	The subpasses in a render pass all render to the same dimensions, 
	and fragments for pixel (x,y,layer) in one subpass can only read attachment contents written by previous subpasses at that same (x,y,layer) location.

	Subpass dependencies describe execution and memory dependencies between subpasses.

	A render pass describes the structure of subpasses and attachments independent of any specific image views for the attachments.
	A render pass and a framebuffer define the complete render target state for one or more subpasses as well as the algorithmic dependencies between the subpasses.
	A render pass must have at least one subpass.
**************************************************/

VulkanRenderPass::VulkanRenderPass(VkDevice device, const GfxFrameBufferDesc& desc)
{
	assert(!isValid());

	std::vector<VkAttachmentDescription> attachmentDescs;
	std::vector<VkAttachmentReference> colorRefs;
	uint32_t index = 0u;
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (desc.ColorSurface[i])
		{
			auto imageView = std::static_pointer_cast<VulkanImageView>(desc.ColorSurface[i]);
			assert(imageView);

			VkAttachmentDescription attachmentDesc
			{
				0u,
				imageView->getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			};
			attachmentDescs.emplace_back(std::move(attachmentDesc));

			VkAttachmentReference colorRef
			{
				index++,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};
			colorRefs.emplace_back(std::move(colorRef));
		}
	}

	VkAttachmentReference depthRef
	{
		index,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};


	if (desc.DepthSurface)
	{
		auto depthImageView = std::static_pointer_cast<VulkanImageView>(desc.DepthSurface);
		assert(depthImageView);

		VkAttachmentDescription attachmentDesc
		{
			0u,
			depthImageView->getFormat(),
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
		attachmentDescs.emplace_back(std::move(attachmentDesc));
	}

	VkSubpassDescription subpassDesc
	{
		0u,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0u,
		nullptr,
		(uint32_t)colorRefs.size(),
		colorRefs.data(),
		nullptr,
		desc.DepthSurface ? &depthRef : nullptr,
		0u,
		nullptr
	};

	std::vector<VkSubpassDependency> subpassDependencies
	{
		{
			VK_SUBPASS_EXTERNAL,
			0u,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		},
		{
			0u,
			VK_SUBPASS_EXTERNAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		}
	};

	VkRenderPassCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)attachmentDescs.size(),
		attachmentDescs.data(),
		1u,
		&subpassDesc,
		(uint32_t)subpassDependencies.size(),
		subpassDependencies.data()
	};

	GfxVerifyVk(vkCreateRenderPass(device, &createInfo, vkMemoryAllocator, &Handle));

	///m_CmdBuffer = device.allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
}

void VulkanRenderPass::pendingGfxPipline(const GfxPipelineState& state)
{
	///VulkanEngine::instance().getOrCreateGraphicsPipeline(*this, state);
}

void VulkanRenderPass::bindGfxPipeline(const GfxPipelineState& state)
{
	///m_CmdBuffer.waitFence();

	///m_CmdBuffer.resetCommand();

	if (!m_CmdBuffer.isInsideRenderPass())
	{
		VulkanAsyncPool::instance()->waitFence(m_CmdBuffer.fence());

		VkClearValue clearValue[2u]{};
		clearValue[0].color =
		{
			state.ClearValue.Color.x,
			state.ClearValue.Color.y,
			state.ClearValue.Color.z,
			state.ClearValue.Color.w,
		};
		clearValue[1].depthStencil =
		{
			state.ClearValue.Depth,
			state.ClearValue.Stencil
		};

		uint32_t frameIndex = 0u;
		///uint32_t frameIndex = VulkanEngine::instance().acquireNextFrame();
		assert(frameIndex < m_FrameBuffers.size());

		VkRenderPassBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			Handle,
			m_FrameBuffers[frameIndex].Handle,
			{
				{
					(int32_t)state.RenderArea.x,
					(int32_t)state.RenderArea.y,
				},
				{
					(uint32_t)state.RenderArea.z,
					(uint32_t)state.RenderArea.w,
				}
			},
			ARRAYSIZE(clearValue),
			clearValue
		};

		m_CmdBuffer.beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	///m_CurGfxPipeline = VulkanEngine::instance().getOrCreateGraphicsPipeline(*this, state);
	///m_CurGfxPipeline->bind(m_CmdBuffer);

	setDynamicGfxState(state);

	if (state.isDirty(GfxPipelineState::eVertexBuffer))
	{
		assert(state.VertexBuffer);
		auto vertexBuffer = static_cast<VulkanBuffer*>(state.VertexBuffer);
		VkDeviceSize offsets[1u]{};
		vkCmdBindVertexBuffers(m_CmdBuffer.Handle, 0u, 1u, &vertexBuffer->Handle, offsets);
	}

	if (state.isDirty(GfxPipelineState::eIndexBuffer))
	{
		assert(state.IndexBuffer);
		auto indexBuffer = static_cast<VulkanBuffer*>(state.IndexBuffer);
		vkCmdBindIndexBuffer(m_CmdBuffer.Handle, indexBuffer->Handle, 0u, state.IndexType == eRIndexType::eUInt16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	}

	VulkanQueueManager::instance()->gfxQueue()->queueCommandBuffer(&m_CmdBuffer);
}

void VulkanRenderPass::drawIndexed(const GfxPipelineState& state, uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset)
{
	assert(isValid() && m_CurGfxPipeline && m_CmdBuffer.isValid());

	setDynamicGfxState(state);

	vkCmdDrawIndexed(
		m_CmdBuffer.Handle,
		indexCount,
		1u,
		firstIndex,
		vertexOffset,
		1u);
}

void VulkanRenderPass::setDynamicGfxState(const GfxPipelineState& state)
{
	assert(m_CmdBuffer.isValid());

	///if (state.isDirty(GfxPipelineState::eViewport))
	{
		VkViewport viewport
		{
			state.Viewport.x,
			state.Viewport.y,
			state.Viewport.z,
			state.Viewport.w,
			state.Viewport.minDepth(),
			state.Viewport.maxDepth()
		};
		vkCmdSetViewport(m_CmdBuffer.Handle, 0u, 1u, &viewport);
	}

	///if (state.isDirty(GfxPipelineState::eScissor))
	{
		VkRect2D scissor
		{
			{
				(int32_t)state.Scissor.x,
				(int32_t)state.Scissor.y
			},
			{
				(uint32_t)state.Scissor.z,
				(uint32_t)state.Scissor.w
			}
		};
		vkCmdSetScissor(m_CmdBuffer.Handle, 0u, 1u, &scissor);
	}
}

void VulkanRenderPass::destroy(VkDevice device)
{
	if (isValid())
	{
		for (uint32_t i = 0u; i < m_FrameBuffers.size(); ++i)
		{
			m_FrameBuffers[i].destroy(device);
		}
		m_FrameBuffers.clear();

		vkDestroyRenderPass(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}
