#include "VulkanEngine.h"
#include "VulkanImage.h"
#include "VulkanShader.h"

GfxShader *VulkanDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	auto shader = new VulkanShader(m_LogicalDevice.Handle, usage, shaderBinary);

	return shader;
}

GfxTexture *VulkanDevice::createTexture(const std::string &textureName)
{
	auto textureBinary = rAsset::rAssetBucket::instance().getTextureBinary(textureName);
	VulkanImage image(m_LogicalDevice.Handle, textureBinary);
	auto imageView = new VulkanImageView();
	imageView->create(m_LogicalDevice.Handle, image, image.getFormat(), VK_IMAGE_ASPECT_COLOR_BIT);

	return imageView;
}

GfxTexture * VulkanDevice::createTexture(
	eRTextureType type, 
	eRFormat format, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels, 
	uint32_t arrayLayers, 
	const void *data, 
	size_t dataSize)
{
	auto imageView = new VulkanImageView();
	imageView->create(m_LogicalDevice.Handle, type, format, width, height, mipLevels, arrayLayers, data, dataSize);

	return imageView;
}

GfxGpuBuffer *VulkanDevice::createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void * pData)
{
	auto buffer = new VulkanGpuBuffer(m_LogicalDevice.Handle, bindFlags, usage, size, pData);

	return buffer;
}

VulkanSemaphore *VulkanDevice::createSemaphore() const
{
	VulkanSemaphore *semaphorePtr = new VulkanSemaphore(m_LogicalDevice.Handle);
	assert(semaphorePtr);

	return semaphorePtr;
}

vkEvent *VulkanDevice::createEvent() const
{
	vkEvent *eventPtr = new vkEvent(m_LogicalDevice.Handle);
	assert(eventPtr);

	return eventPtr;
}

GfxRenderSurface *VulkanDevice::createDepthStencilView(uint32_t width, uint32_t height, eRFormat format)
{
	VulkanDepthStencilView *depthStencilView = new VulkanDepthStencilView();
	depthStencilView->create(m_LogicalDevice.Handle, width, height, VulkanEnum::toFormat(format));
	return depthStencilView;
}

VulkanDevice::vkPipelinePool::vkPipelinePool(VkDevice device)
	: m_Device(device)
{
	m_PipelineCache.create(m_Device);
}

vkGraphicsPipeline *VulkanDevice::vkPipelinePool::getOrCreateGraphicsPipeline(const vkRenderPass &renderpass, const GfxPipelineState &state)
{
	assert(renderpass.isValid());

	for (auto &it : m_Pipelines)
	{
		if (it.first == state)
		{
			return (vkGraphicsPipeline *)it.second;
		}
	}

	auto graphicsPipeline = new vkGraphicsPipeline;
	graphicsPipeline->create(m_Device, renderpass, m_PipelineCache, state);

	m_Pipelines.emplace_back(std::make_pair(state, graphicsPipeline));

	return graphicsPipeline;
}

GfxRenderPass *VulkanDevice::createRenderPass(const VulkanSwapchain &swapchain, GfxFrameBufferDesc &desc)
{
	assert(isValid() && swapchain.isValid());

	bool8_t usingBackBuffer = true;
	for (uint32_t i = 0u; i < eMaxRenderTargets; ++i)
	{
		if (desc.ColorSurface[i])
		{
			usingBackBuffer = false;
			break;
		}
	}

	std::vector<VulkanFrameBuffer> frameBuffers;

	if (usingBackBuffer)
	{
		auto &backBuffers = swapchain.getBackBuffers();
		desc.ColorSurface[0] = (GfxRenderSurface *)&backBuffers[0];
	}

	vkRenderPass *renderPass = new vkRenderPass();
	renderPass->create(m_LogicalDevice.Handle, desc);

	if (usingBackBuffer)
	{
		auto &backBuffers = swapchain.getBackBuffers();
		for (uint32_t i = 0u; i < backBuffers.size(); ++i)
		{
			desc.ColorSurface[0] = (GfxRenderSurface *)&backBuffers[i];
			VulkanFrameBuffer frameBuffer;
			frameBuffer.create(m_LogicalDevice.Handle, *renderPass, desc);
			frameBuffers.emplace_back(std::move(frameBuffer));
		}
	}
	else
	{
		VulkanFrameBuffer frameBuffer;
		frameBuffer.create(m_LogicalDevice.Handle, *renderPass, desc);
		frameBuffers.emplace_back(std::move(frameBuffer));
	}

	renderPass->bindFrameBuffers(frameBuffers);

	return renderPass;
}

GfxSampler *VulkanDevice::createSampler(const GfxSamplerDesc &desc)
{
	auto sampler = new VulkanSampler(m_LogicalDevice.Handle, desc);

	return sampler;
}
