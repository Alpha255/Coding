#include "VulkanEngine.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

GfxShader *VulkanDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	auto shader = new vkShader(*this, usage, shaderBinary);
	m_GpuResourcePool->push<vkGpuResourcePool::eShader>(shader);

	return shader;
}

rTexture *VulkanDevice::createTexture(const std::string &textureName)
{
	auto textureBinary = rAsset::rAssetBucket::instance().getTextureBinary(textureName);
	vkImage image(*this, textureBinary);
	auto imageView = new vkImageView();
	imageView->create(*this, image, image.getFormat(), VK_IMAGE_ASPECT_COLOR_BIT);

	m_GpuResourcePool->push<vkGpuResourcePool::eImageView>(imageView);

	return imageView;
}

rTexture * VulkanDevice::createTexture(
	eRTextureType type, 
	eRFormat format, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels, 
	uint32_t arrayLayers, 
	const void *data, 
	size_t dataSize)
{
	auto imageView = new vkImageView();
	imageView->create(*this, type, format, width, height, mipLevels, arrayLayers, data, dataSize);

	m_GpuResourcePool->push<vkGpuResourcePool::eImageView>(imageView);

	return imageView;
}

GfxGpuBuffer *VulkanDevice::createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void * pData)
{
	auto buffer = new vkGpuBuffer(*this, bindFlags, usage, size, pData);
	m_GpuResourcePool->push<vkGpuResourcePool::eBuffer>(buffer);

	return buffer;
}

vkFence *VulkanDevice::createFence(vkFence::eFenceStatus status) const
{
	vkFence *fencePtr = new vkFence(*this, status);
	assert(fencePtr);

	return fencePtr;
}

vkSemaphore *VulkanDevice::createSemaphore() const
{
	vkSemaphore *semaphorePtr = new vkSemaphore(*this);
	assert(semaphorePtr);

	return semaphorePtr;
}

vkEvent *VulkanDevice::createEvent() const
{
	vkEvent *eventPtr = new vkEvent(*this);
	assert(eventPtr);

	return eventPtr;
}

GfxRenderSurface *VulkanDevice::createDepthStencilView(uint32_t width, uint32_t height, eRFormat format)
{
	vkDepthStencilView *depthStencilView = new vkDepthStencilView();
	depthStencilView->create(*this, width, height, vkEngine::enumTranslator::toFormat(format));

	m_GpuResourcePool->push<vkGpuResourcePool::eImageView>(depthStencilView);
	return depthStencilView;
}

VulkanDevice::vkPipelinePool::vkPipelinePool(const VulkanDevice &device)
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

GfxRenderPass *VulkanDevice::createRenderPass(const vkSwapchain &swapchain, GfxFrameBufferDesc &desc)
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

	std::vector<vkFrameBuffer> frameBuffers;

	if (usingBackBuffer)
	{
		auto &backBuffers = swapchain.getBackBuffers();
		desc.ColorSurface[0] = (GfxRenderSurface *)&backBuffers[0];
	}

	vkRenderPass *renderPass = new vkRenderPass();
	renderPass->create(*this, desc);

	if (usingBackBuffer)
	{
		auto &backBuffers = swapchain.getBackBuffers();
		for (uint32_t i = 0u; i < backBuffers.size(); ++i)
		{
			desc.ColorSurface[0] = (GfxRenderSurface *)&backBuffers[i];
			vkFrameBuffer frameBuffer;
			frameBuffer.create(*this, *renderPass, desc);
			frameBuffers.emplace_back(std::move(frameBuffer));
		}
	}
	else
	{
		vkFrameBuffer frameBuffer;
		frameBuffer.create(*this, *renderPass, desc);
		frameBuffers.emplace_back(std::move(frameBuffer));
	}

	renderPass->bindFrameBuffers(frameBuffers);

	m_GpuResourcePool->push<vkGpuResourcePool::eRenderPass>(renderPass);

	return renderPass;
}

rSampler *VulkanDevice::createSampler(const GfxSamplerDesc &desc)
{
	auto sampler = new vkSampler(*this, desc);
	m_GpuResourcePool->push<vkGpuResourcePool::eSampler>(sampler);

	return sampler;
}

void VulkanDevice::vkGpuResourcePool::destoryAll()
{
	for (auto &resource : m_Resources)
	{
		eResourceType type = getResourceType(resource.first);

		switch (type)
		{
		case eShader:
			destroyResource<eShader>(resource.second);
			break;
		case eBuffer:
			destroyResource<eBuffer>(resource.second);
			break;
		case eImageView:
			destroyResource<eImageView>(resource.second);
			break;
		case eRenderPass:
			destroyResource<eRenderPass>(resource.second);
			break;
		case eSampler:
			destroyResource<eSampler>(resource.second);
			break;
		default:
			assert(0);
			break;
		}
	}

	m_Resources.clear();
}
