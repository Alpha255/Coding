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

	m_Context.Finalize();

	m_Device.Destory();

	m_Instance.Destory();
}
