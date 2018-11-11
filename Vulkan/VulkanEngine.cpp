#include "VulkanEngine.h"

std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> VulkanEngine::s_Instance;

void VulkanEngine::Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	m_Instance.Create();

	m_PhysicalDevice.Create();

	m_Swapchain.Init(hWnd);

	m_Device.Create();

	m_Swapchain.Create(width, height, bWindowed);

	m_Inited = true;
}
