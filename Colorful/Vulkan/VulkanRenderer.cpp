#include "Colorful/Vulkan/VulkanRenderer.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"
#include "Colorful/Vulkan/VulkanPipeline.h"
#include "Colorful/Vulkan/VulkanQueue.h"

NAMESPACE_START(Gfx)

void VulkanRenderer::initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync)
{
#if defined(USE_VK_LOADER)
	VulkanLoader::loadGlobalFuncs();
#endif

	m_Instance = create_unique<VulkanInstance>();

#if defined(USE_VK_LOADER)
	VulkanLoader::loadInstanceFuncs(m_Instance->get());
#endif

	m_Device = create_shared<VulkanDevice>(m_Instance->get());

#if defined(USE_VK_LOADER)
	VulkanLoader::loadDeviceFuncs(m_Device->get());
#endif

	VkResource::s_Device = m_Device;

	VkQueues::instance().initialize(
		m_Device->getQueueFamilyIndex<VulkanDevice::EQueueFamilyType::Graphics>(), 
		m_Device->getQueueFamilyIndex<VulkanDevice::EQueueFamilyType::Transfer>(),
		m_Device->getQueueFamilyIndex<VulkanDevice::EQueueFamilyType::Compute>());

#if defined(_DEBUG)
	m_DebugUtilsMessenger = std::make_unique<VulkanDebugUtilsMessenger>(true),
#endif

	VulkanMemoryAllocator::instance().initialize(m_Device->physicalDevice());

	m_Swapchain = create_unique<VulkanSwapchain>(
		windowHandle, 
		width, 
		height, 
		fullscreen, 
		VSync,
		false);

	VkRenderStateCache::instance().initialize(m_Swapchain.get());
}

void VulkanRenderer::toggleFullScreen(bool8_t fullscreen)
{
	(void)fullscreen;
}

void VulkanRenderer::toggleVSync(bool8_t VSync)
{
	(void)VSync;
}

void VulkanRenderer::finalize()
{
	m_Device->waitIdle();

	m_Device->clearResourceFactory();

	VkQueues::instance().finalize();

#if defined(_DEBUG)
	m_DebugUtilsMessenger.reset();
#endif

	VulkanMemoryAllocator::instance().finalize();
	VkRenderStateCache::instance().finalize();

#if defined(USE_VK_LOADER)
	m_Swapchain.reset();
	VkResource::s_Device.reset();
	m_Device.reset();
	m_Instance.reset();

	VulkanLoader::free();
#endif
}

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		ptr = std::make_unique<VulkanRenderer>();
	}
}

NAMESPACE_END(Gfx)
