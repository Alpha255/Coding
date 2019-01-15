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
		m_Device.GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, false),
		m_Device.GetOptimalSurfaceFormat(VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, true),
		true,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	m_Swapchain.Create(hWnd, width, height, bWindowed);

	m_Inited = true;
}
