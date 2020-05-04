#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanQueue.h"

class vkEngine : public rEngine, public singleton<vkEngine>
{
	singletonDeclare(vkEngine);
public:
	void initialize(uint64_t windowHandle, const appConfig &config) override final;
	void finalize() override final;

	void logError(uint32_t result) const override final;

	inline void handleWindowResize(uint32_t width, uint32_t height, const appConfig &config) override final
	{
		if (m_Swapchain.isValid())
		{
			m_Swapchain.recreate(width, height, config.VSync, config.FullScreen, m_PhysicalDevice, m_Device);
		}
	}

	void present() override final;
	void waitDone(vkCommandBuffer *cmdBuffer);

	inline vkDevice &getDevice()
	{
		return m_Device;
	}
	inline vkDeviceQueue &getQueue()
	{
		return m_GraphicsQueue;
	}
public:
	inline rShader *createVertexShader(const std::string &shaderName) override final
	{
		return createShader<eVertexShader>(shaderName);
	}
	inline rShader *createFragmentShader(const std::string &shaderName) override final
	{
		return createShader<eFragmentShader>(shaderName);
	}

	inline rBuffer *createIndexBuffer(eRBufferUsage usage, size_t size, const void *data) override final
	{ 
		return m_Device.createBuffer(eIndexBuffer, usage, size, data);
	}
	inline rBuffer *createVertexBuffer(eRBufferUsage usage, size_t size, const void *data) override final
	{ 
		return m_Device.createBuffer(eVertexBuffer, usage, size, data);
	}
	inline rBuffer *createUniformBuffer(size_t size, const void *data) override final
	{
		return m_Device.createBuffer(eUniformBuffer, eGpuReadCpuWrite, size, data);
	}
	inline void destroyBuffer(rBuffer *buffer) override final
	{
		m_Device.destroyBuffer(buffer);
	}

	inline rRenderSurface *createDepthStencilView(uint32_t width, uint32_t height, eRFormat format) override final
	{
		return m_Device.createDepthStencilView(width, height, format);
	}

	inline rRenderPass *createRenderPass(rFrameBufferDesc &desc) override final
	{
		return m_Device.createRenderPass(m_Swapchain, desc);
	}
	void createOpaqueRenderPass() override final;

	inline rTexture *createTexture(const std::string &textureName) override final
	{
		return m_Device.createTexture(textureName);
	}
	inline rTexture *createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void *data,
		size_t dataSize) override final
	{
		return m_Device.createTexture(type, format, width, height, mipLevels, arrayLayers, data, dataSize);
	}

	inline rSampler *createSampler(const rSamplerDesc &desc) override final
	{
		return m_Device.createSampler(desc);
	}

	inline void updateGpuBuffer(rBuffer *buffer, const void *data, size_t size, size_t offset) override final
	{
		assert(buffer);
		auto uniformBuffer = static_cast<vkGpuBuffer *>(buffer);
		assert(uniformBuffer);
		uniformBuffer->update(m_Device, data, size, offset);
	}

	inline vkGraphicsPipeline *getOrCreateGraphicsPipeline(const vkRenderPass &renderpass, const rGraphicsPipelineState &state)
	{
		return m_Device.getOrCreateGraphicsPipeline(renderpass, state);
	}

	inline vkCommandBuffer *getActiveCommandBuffer()
	{
		return m_Device.getActiveCommandBuffer();
	}

	inline uint32_t acquireNextFrame()
	{
		return m_Swapchain.acquireBackBuffer(m_Device);
	}

	VkFormatProperties getFormatProperties(VkFormat format);
public:
	class enumTranslator
	{
	public:
		static VkPolygonMode toPolygonMode(eRPolygonMode mode);
		static VkCullModeFlags toCullMode(eRCullMode mode);
		static VkFrontFace toFrontFace(eRFrontFace frontFace);
		static VkLogicOp toLogicOp(eRLogicOp op);
		static VkBlendOp toBlendOp(eRBlendOp op);
		static VkBlendFactor toBlendFactor(eRBlendFactor factor);
		static VkColorComponentFlags toColorComponentFlags(uint32_t colorMask);
		static VkCompareOp toCompareOp(eRCompareOp op);
		static VkStencilOp toStencilOp(eRStencilOp op);
		static VkFormat toFormat(eRFormat format);
		static VkShaderStageFlagBits toShaderStage(eRShaderUsage usage);
		static VkPrimitiveTopology toPrimitiveTopology(eRPrimitiveTopology primitiveTopology);
		static VkDescriptorType toDescriptorType(eRDescriptorType type);
		static VkFilter toFilter(eRFilter filter);
		static VkSamplerAddressMode toAddressMode(eRSamplerAddressMode addressMode);
		static VkBorderColor toBorderColor(eRBorderColor borderColor);
	};
protected:
	template<eRShaderUsage Usage>
	inline rShader *createShader(const std::string &shaderName)
	{
		return m_Device.createShader(Usage, shaderName);
	}
private:
	vkInstance m_Instance;
	vkDebugUtilsMessenger m_DebugUtilsMessenger;
	vkDevice m_Device;
	vkPhysicalDevice m_PhysicalDevice;
	vkDeviceQueue m_GraphicsQueue;
	vkDeviceQueue m_ComputeQueue;
	vkDeviceQueue m_TransferQueue;
	vkSwapchain m_Swapchain;
};
