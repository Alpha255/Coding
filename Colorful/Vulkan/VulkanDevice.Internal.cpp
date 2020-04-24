#include "VulkanEngine.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

rShader *vkDevice::createShader(eRShaderUsage usage, const std::string &shaderName)
{
	/// try to get shader binary from cache at first

	auto shaderBinary = rAsset::rAssetBucket::instance().getShaderBinary(usage, shaderName);
	auto shader = new vkShader(*this, usage, shaderBinary);
	m_GpuResourcePool->push<vkGpuResourcePool::eShader>(shader);

	return shader;
}

rTexture *vkDevice::createTexture(const std::string &textureName)
{
	auto textureBinary = rAsset::rAssetBucket::instance().getTextureBinary(textureName);
	auto texture = new vkImage(*this, textureBinary);
	m_GpuResourcePool->push<vkGpuResourcePool::eTexture>(texture);

	return texture;
}

rBuffer *vkDevice::createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void * pData)
{
	auto buffer = new vkGpuBuffer(*this, bindFlags, usage, size, pData);
	m_GpuResourcePool->push<vkGpuResourcePool::eBuffer>(buffer);

	return buffer;
}

vkFence *vkDevice::createFence(vkFence::eFenceStatus status) const
{
	vkFence *fencePtr = new vkFence(*this, status);
	assert(fencePtr);

	return fencePtr;
}

vkSemaphore *vkDevice::createSemaphore() const
{
	vkSemaphore *semaphorePtr = new vkSemaphore(*this);
	assert(semaphorePtr);

	return semaphorePtr;
}

vkEvent *vkDevice::createEvent() const
{
	vkEvent *eventPtr = new vkEvent(*this);
	assert(eventPtr);

	return eventPtr;
}

rRenderSurface *vkDevice::createDepthStencilView(uint32_t width, uint32_t height, eRFormat format)
{
	vkDepthStencilView *depthStencilView = new vkDepthStencilView();
	depthStencilView->create(*this, width, height, vkEngine::enumTranslator::toFormat(format));

	m_GpuResourcePool->push<vkGpuResourcePool::eImageView>(depthStencilView);
	return depthStencilView;
}

vkPipeline *vkDevice::vkPipelinePool::getOrCreateGraphicsPipeline(const rGraphicsPipelineState &)
{
	return nullptr;
}

rRenderPass *vkDevice::createRenderPass(const vkSwapchain &swapchain, const rFrameBufferDesc &desc)
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

	if (usingBackBuffer)
	{

	}

	return nullptr;
}

void vkDevice::vkGpuResourcePool::destoryAll()
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
		default:
			assert(0);
			break;
		}
	}

	m_Resources.clear();
}
