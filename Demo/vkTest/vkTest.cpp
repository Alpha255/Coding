#include "vkTest.h"
#include "Colorful/Vulkan/VulkanEngine.h"

VulkanVertexShader vkVS;
VulkanPixelShader vkPS;

void vkTest::postInitialize()
{
	VulkanEngine::Instance().Initialize(m_hWnd, (uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, true);

	vkVS.Create("vkTestVS.shader", "vsMain");
	vkPS.Create("vkTestPS.shader", "psMain");
}

void vkTest::finalize()
{
	VulkanEngine::Instance().Finalize();
}

void vkTest::resizeWindow()
{
	VulkanEngine::Instance().Resize((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
}

void vkTest::renterToWindow()
{
}
