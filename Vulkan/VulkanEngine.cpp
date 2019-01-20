#include "VulkanEngine.h"

std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> VulkanEngine::s_Instance;

void VulkanEngine::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	m_Instance.Create();

	m_Device.Create();

	for (uint32_t i = 0U; i < m_CommandPools.size(); ++i)
	{
		m_CommandPools[i].Create((VulkanCommandPool::ePoolType)i);
	}

	m_DefaultRenderPass.Create(
		true,
		m_Device.GetPhysicalDevice().GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, false),
		m_Device.GetPhysicalDevice().GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, true),
		true,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	m_Swapchain.Create(hWnd, width, height, bWindowed);

	m_Inited = true;
}

void VulkanEngine::FreeCommandBuffers()
{
	for each (auto &cmdBufferMap in m_CommandBuffers)
	{
		for each (auto &cmdBuffer in cmdBufferMap)
		{
			VulkanCommandBuffer buffer(cmdBuffer.second);
			FreeCommandBuffer(buffer);
		}
	}

	for each (auto &cmdBufferListMap in m_CommandBufferLists)
	{
		for each (auto &cmdBufferList in cmdBufferListMap)
		{
			VulkanCommandBufferList bufferList(cmdBufferList.second);
			FreeCommandBufferList(bufferList);
		}
	}
}

VulkanEngine::~VulkanEngine()
{
	m_Swapchain.Destory();
	///m_DefaultRenderPass.Destory();

	FreeCommandBuffers();

	for (uint32_t i = 0U; i < m_CommandPools.size(); ++i)
	{
		m_CommandPools[i].Destory();
	}

	m_Device.Destory();

	m_Instance.Destory();
}
