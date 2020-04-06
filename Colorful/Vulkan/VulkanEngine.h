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

	inline void appendResource(rGpuResource::eResourceType type, rGpuResource *resource)
	{
		m_GpuResourcePool.append(type, resource);
	}

	class enumTranslator
	{
	public:
		static VkPolygonMode toPolygonMode(eRPolygonMode mode);
		static VkCullModeFlags toCullMode(eRCullMode mode);
		static VkFrontFace toFrontFace(eRFrontFace frontFace);
		static VkLogicOp toLogicOp(eLogicOp op);
		static VkBlendOp toBlendOp(eRBlendOp op);
		static VkBlendFactor toBlendFactor(eRBlendFactor factor);
		static VkColorComponentFlags toColorComponentFlags(uint32_t colorMask);
		static VkCompareOp toCompareOp(eRCompareOp op);
		static VkStencilOp toStencilOp(eRStencilOp op);
	};
protected:
	class vkGpuResourcePool
	{
	public:
		inline void append(rGpuResource::eResourceType type, rGpuResource *resource)
		{
			m_Pool[type].emplace_back(resource);
		}

		void releaseAll(const vkDevice &device);
	protected:
		template<class T> void release(rGpuResource *resource, const vkDevice &device)
		{
			T *realType = static_cast<T *>(resource);
			assert(realType);
			realType->destroy(device);
		}
	private:
		std::array<std::vector<rGpuResource *>, rGpuResource::eResourceType_MaxEnum> m_Pool;
	};
private:
	vkInstance m_Instance;
	vkDebugUtilsMessenger m_DebugUtilsMessenger;
	vkDevice m_Device;
	vkPhysicalDevice m_PhysicalDevice;
	vkDeviceQueue m_GraphicsQueue;
	vkDeviceQueue m_ComputeQueue;
	vkDeviceQueue m_TransferQueue;
	vkSwapchain m_Swapchain;
	vkGpuResourcePool m_GpuResourcePool;
};