#pragma once

#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"

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

public:
	inline rShader *createVertexShader(const std::string &shaderName) override final
	{
		return createShader<eVertexShader>(shaderName);
	}
	inline rShader *createFragmentShader(const std::string &shaderName) override final
	{
		return createShader<eFragmentShader>(shaderName);
	}
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
