#pragma once

#include "Colorful/Vulkan/VulkanPipeline.h"
#include "Colorful/Vulkan/VulkanCommand.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanView.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanRenderPass.h"
#include "Colorful/Vulkan/VulkanPipeline.h"

class VulkanDevice : public GfxDevice
{
public:
	inline bool8_t isValid() const
	{
		return m_LogicalDevice.isValid();
	}

	vkSemaphore *createSemaphore() const;
	vkEvent *createEvent() const;

	uint32_t getMemoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const;

	inline vkCommandBuffer allocCommandBuffer(VkCommandBufferLevel level, bool8_t signaleFence = true) const
	{
		return m_CommandPool.alloc(m_LogicalDevice.Handle, level, signaleFence);
	}

	inline void freeCommandBuffer(vkCommandBuffer &commandBuffer) const
	{
		m_CommandPool.free(m_LogicalDevice.Handle, commandBuffer);
	}

	inline vkDescriptorSet allocDescriptorSet(const vkDescriptorSetLayout &layout) const
	{
		return m_DescriptorPool.alloc(m_LogicalDevice.Handle, layout);
	}

	inline const VkPhysicalDeviceLimits &getDeviceLimits() const
	{
		return m_DeviceLimits;
	}

	GfxShader *createShader(eRShaderUsage usage, const std::string &shaderName);
	rTexture *createTexture(const std::string &textureName);
	rTexture *createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void *data,
		size_t dataSize);
	GfxGpuBuffer *createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
	inline void destroyBuffer(GfxGpuBuffer *buffer)
	{
		m_GpuResourcePool->destroyBuffer(buffer);
	}
	GfxRenderSurface *createDepthStencilView(uint32_t width, uint32_t height, eRFormat format);
	GfxRenderPass *createRenderPass(const VulkanSwapchain &swapchain, GfxFrameBufferDesc &desc);
	rSampler *createSampler(const GfxSamplerDesc &desc);

	const GfxPipelineState *getGraphicsPipelineState(vkGraphicsPipeline *pipeline) const
	{
		return m_PipelinePool->getGraphicsPipelineState(pipeline);
	}

	inline vkGraphicsPipeline *getOrCreateGraphicsPipeline(const vkRenderPass &renderpass, const GfxPipelineState &graphicsPipelineState)
	{
		return m_PipelinePool->getOrCreateGraphicsPipeline(renderpass, graphicsPipelineState);
	}

	inline vkCommandBuffer *getActiveCommandBuffer()
	{
		return m_CommandPool.getActiveCommandBuffer(m_LogicalDevice.Handle);
	}
public:
	class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice>
	{
	};

	class VulkanLogicalDevice : public VulkanObject<VkDevice>
	{
	};

	void create(VkInstance instance);

	void destroy();

	VkPhysicalDevice physicalDevice() const
	{
		return m_PhysicalDevice.Handle;
	}

	VkDevice logicalDevice() const
	{
		return m_LogicalDevice.Handle;
	}
protected:
	class vkPipelinePool
	{
	public:
		vkPipelinePool(VkDevice device);

		vkGraphicsPipeline *getOrCreateGraphicsPipeline(const vkRenderPass &renderpass, const GfxPipelineState &graphicsPipelineState);

		inline void destroyAll()
		{
			for (auto &it : m_Pipelines)
			{
				it.second->destroy(m_Device);
			}
			m_Pipelines.clear();

			m_PipelineCache.destroy(m_Device);
		}

		const GfxPipelineState *getGraphicsPipelineState(vkGraphicsPipeline *pipeline) const
		{
			for (uint32_t i = 0u; i < m_Pipelines.size(); ++i)
			{
				if (m_Pipelines[i].second == pipeline)
				{
					return &m_Pipelines[i].first;
				}
			}

			return nullptr;
		}
	protected:
	private:
		const VulkanDevice &m_Device;
		///std::unordered_map<GfxPipelineState, vkPipeline *> m_Pipelines;
		std::vector<std::pair<GfxPipelineState, vkPipeline *>> m_Pipelines;
		vkPipelineCache m_PipelineCache;
	};
private:
	VkPhysicalDeviceFeatures m_DeviceFeatures{};
	VkPhysicalDeviceLimits m_DeviceLimits{};
	vkCommandPool m_CommandPool;
	vkDescriptorPool m_DescriptorPool;
	std::unique_ptr<vkPipelinePool> m_PipelinePool;
private:
	VulkanPhysicalDevice m_PhysicalDevice;
	VulkanLogicalDevice m_LogicalDevice;
};
