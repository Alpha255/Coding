#pragma once

#include "Colorful/Vulkan/VulkanInstance.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanQueue.h"
#include "Colorful/Vulkan/VulkanEnum.h"

class VulkanEngine : public GfxEngine
{
public:
	static void logError(uint32_t result);

	void initialize(uint64_t windowHandle, const Gear::Configurations& config) override final;
	void finalize() override final;

	void handleWindowResize(uint32_t, uint32_t) override final;

	void present() override final;

	GfxShaderPtr createVertexShader(const std::string& shaderName) override final;
	GfxShaderPtr createFragmentShader(const std::string& shaderName) override final;

	GfxGpuBufferPtr createIndexBuffer(eRBufferUsage usage, size_t size, const void *data) override final;
	GfxGpuBufferPtr createVertexBuffer(eRBufferUsage usage, size_t size, const void *data) override final;

	GfxGpuBufferPtr createUniformBuffer(size_t size, const void *data) override final;

	GfxRenderSurfacePtr createDepthStencilView(uint32_t width, uint32_t height, eRFormat format) override final;

	GfxRenderPassPtr createRenderPass(GfxFrameBufferDesc &desc) override final;

	GfxTexturePtr createTexture(const std::string& textureName) override final;
	GfxTexturePtr createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void* data,
		size_t dataSize) override final;

	GfxSamplerPtr createSampler(const GfxSamplerDesc& desc) override final;

	template <typename T> static std::vector<const char8_t*> getSupportedProperties(
		const std::vector<T>& supportedProperties,
		const std::vector<const char8_t*>& targetProperties)
	{
		std::vector<const char8_t*> result;

		for each (auto propertyName in targetProperties)
		{
			for (auto it = supportedProperties.begin(); it != supportedProperties.end(); ++it)
			{
				if (_stricmp((const char8_t*)(&(*it)), propertyName) == 0)
				{
					result.emplace_back(propertyName);
					break;
				}
			}
		}

		return result;
	}
protected:
private:
	VulkanInstance m_Instance;
	VulkanDevice m_Device;
	VulkanSwapchainPtr m_Swapchain = nullptr;
};
