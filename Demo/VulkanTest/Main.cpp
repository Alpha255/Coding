#include "VulkanTest.h"

extern VkCommandPool vkCommandPool;
extern VkCommandPool vkCommandPool_S;
extern VkDevice vkDevice;
extern VkQueue vkQueue;
extern VkSemaphore vkSemaphoreComplete;
extern VkSemaphore vkSemaphorePresent;
extern VkQueueFamilyIndex vkQueueFamilyIndex;
extern VkSubmitInfo vkSubmitInfo;
extern VkPipelineStageFlags vkSubmitPipelineStages;
extern uint32_t vkQueueNodeIndex;
extern bool vkPrepared;

int32_t WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int32_t /*nShow*/)
{
	CreateInstance();

	CreateDevice();

	vkCommandPool = CreateCommandPool(vkQueueFamilyIndex.Graphics);

	vkGetDeviceQueue(vkDevice, vkQueueFamilyIndex.Graphics, 0U, &vkQueue);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	check(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkSemaphoreComplete));
	check(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkSemaphorePresent));

	vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSubmitInfo.pWaitDstStageMask = &vkSubmitPipelineStages;
	vkSubmitInfo.waitSemaphoreCount = 1U;
	vkSubmitInfo.pWaitSemaphores = &vkSemaphorePresent;
	vkSubmitInfo.signalSemaphoreCount = 1U;
	vkSubmitInfo.pSignalSemaphores = &vkSemaphoreComplete;

	MakeWindow(L"VulkanTest", 1280U, 720U, 0U);

	vkCommandPool_S = CreateCommandPool(vkQueueNodeIndex);

	InitSwapchain(hInstance);
	CreateSwapchain();
	CreateCommandBuffers();
	CreateFence();
	CreateDepthStencilView();
	CreateRenderpass();
	CreatePipelineCache();
	CreateFrameBuffers();
	CreateShaders();

	InitImGUI();

	BuildCommandBuffers();

	vkPrepared = true;

	RenderLoop();

	return 0;
}