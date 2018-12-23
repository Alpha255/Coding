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

	m_Wait.Create();
	m_Fence.Create();

	m_CmdPool.Create(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	m_CmdBuffer = m_CmdPool.AllocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1U);
}

void AppBox::RenderScene()
{
	uint32_t index = 0;
	VKCheck(vkAcquireNextImageKHR(VulkanEngine::Instance().GetDevice().Get(), VulkanEngine::Instance().GetSwapchain().Get(), UINT64_MAX, m_Wait.Get(), VK_NULL_HANDLE, &index));

	VkCommandBufferBeginInfo beginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		0U,
		nullptr
	};
	VKCheck(vkBeginCommandBuffer(m_CmdBuffer.Get(), &beginInfo));

	vkCmdBindPipeline(m_CmdBuffer.Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context.Get());

	VkClearColorValue color = {};
	color.float32[0] = 0.0f;
	color.float32[1] = 0.125f;
	color.float32[2] = 0.3f;
	color.float32[3] = 1.0f;
	VkImageSubresourceRange range = {};
	range.levelCount = ~0;
	range.layerCount = ~0;
	range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkCmdClearColorImage(m_CmdBuffer.Get(), VulkanEngine::Instance().GetSwapchain().GetImage(index).Image, VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);

	VKCheck(vkEndCommandBuffer(m_CmdBuffer.Get()));

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	VkPipelineStageFlags waitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = m_CmdBuffer.GetPtr();
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = m_Wait.GetPtr();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStageFlags;
	VkResult result = vkQueueSubmit(VulkanEngine::Instance().GetDevice().GetQueue(), 1, &submitInfo, m_Fence.Get());

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