#include "Box.h"
#include "Camera.h"
#include "VulkanShader.h"
#include "VulkanEngine.h"

void AppBox::Initialize()
{
	m_Camera->SetViewRadius(5.0f);

	m_Box.CreateAsCube(1.0f);

	m_VS.Create("VulkanBoxVS.glsl");
	m_PS.Create("VulkanBoxPS.glsl");

	VkPipelineShaderStageCreateInfo shaderStage[2] = { m_VS.GetShaderStage(), m_PS.GetShaderStage() };

	m_Context.Create(shaderStage, _countof(shaderStage));

	VulkanEngine::Instance().GetSwapchain().CreateFrameBuffer(m_Width, m_Height, m_Context.m_RenderTargetView.Get());

	m_Wait.Create();
	m_Fence.Create();

	m_CmdPool.Create(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	m_CmdBuffer = m_CmdPool.AllocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1U);

	m_VertexBuffer.Create(m_Box.m_Vertices.size() * sizeof(Geometry::Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	m_IndexBuffer.Create(m_Box.m_Indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	m_VertexBuffer.Update(m_Box.m_Vertices.data(), m_Box.m_Vertices.size() * sizeof(Geometry::Vertex));
	m_IndexBuffer.Update(m_Box.m_Indices.data(), m_Box.m_Indices.size() * sizeof(uint32_t));
}

void AppBox::RenderScene()
{
	uint32_t index = 0;
	VKCheck(vkAcquireNextImageKHR(VulkanEngine::Instance().GetDevice().Get(), VulkanEngine::Instance().GetSwapchain().Get(), UINT64_MAX, m_Wait.Get(), VK_NULL_HANDLE, &index));

	VkCommandBufferInheritanceInfo inherit = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
	inherit.framebuffer = VK_NULL_HANDLE;
	inherit.renderPass = m_Context.m_RenderTargetView.Get();

	VkCommandBufferBeginInfo beginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		&inherit
	};
	VKCheck(vkBeginCommandBuffer(m_CmdBuffer.Get(), &beginInfo));

	vkCmdBindPipeline(m_CmdBuffer.Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context.Get());

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_CmdBuffer.Get(), 0U, 1U, m_VertexBuffer.GetPtr(), offsets);
	vkCmdBindIndexBuffer(m_CmdBuffer.Get(), m_IndexBuffer.Get(), 0U, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(m_CmdBuffer.Get(), m_Box.m_Indices.size(), 1U, 0U, 0, 0U);

	VkClearValue clearValues[2];
	clearValues[0].color.float32[0] = 0.66f;
	clearValues[0].color.float32[1] = 0.33f;
	clearValues[0].color.float32[2] = 0.44f;
	clearValues[0].color.float32[3] = 1.0f;
	clearValues[1].depthStencil.depth = 1.0f;
	clearValues[1].depthStencil.stencil = 0;
	VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	renderPassBeginInfo.renderPass = m_Context.m_RenderTargetView.Get();
	renderPassBeginInfo.framebuffer = VulkanEngine::Instance().GetSwapchain().GetImage(index).FrameBuffer;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = m_Width;
	renderPassBeginInfo.renderArea.extent.height = m_Height;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.clearValueCount = 2;

	vkCmdBeginRenderPass(m_CmdBuffer.Get(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	VkViewport viewport = {};
	viewport.height = (float)m_Height;
	viewport.width = (float)m_Width;
	viewport.minDepth = (float) 0.0f;
	viewport.maxDepth = (float) 1.0f;
	vkCmdSetViewport(m_CmdBuffer.Get(), 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.extent.width = m_Width;
	scissor.extent.height = m_Height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(m_CmdBuffer.Get(), 0, 1, &scissor);

	vkCmdExecuteCommands(m_CmdBuffer.Get(), 1, m_CmdBuffer.GetPtr());
	vkCmdEndRenderPass(m_CmdBuffer.Get());

	VkResult result = vkEndCommandBuffer(m_CmdBuffer.Get());

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	VkPipelineStageFlags waitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = m_CmdBuffer.GetPtr();
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = m_Wait.GetPtr();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStageFlags;
	result = vkQueueSubmit(VulkanEngine::Instance().GetDevice().GetQueue(), 1, &submitInfo, m_Fence.Get());

	VkPresentInfoKHR present;
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.pNext = nullptr;
	present.swapchainCount = 1;
	present.pSwapchains = VulkanEngine::Instance().GetSwapchain().GetPtr(); 
	present.pImageIndices = &index; 
	present.pWaitSemaphores = nullptr;
	present.waitSemaphoreCount = 0;
	present.pResults = nullptr;
	do
	{
		result = vkWaitForFences(VulkanEngine::Instance().GetDevice().Get(), 1U, m_Fence.GetPtr(), VK_TRUE, 100000000);
	} while (result == VK_TIMEOUT);
	VKCheck(vkQueuePresentKHR(VulkanEngine::Instance().GetDevice().GetQueue(), &present));
}