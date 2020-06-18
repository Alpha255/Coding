#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanCommand.h"

class VulkanPipelineLayout : public VulkanDeviceObject<VkPipelineLayout>
{
public:
	void create(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);

	void destroy(VkDevice device) override final
	{
		/// The pipeline layout represents a sequence of descriptor sets with each having a specific layout. 
		/// This sequence of layouts is used to determine the interface between shader stages and shader resources. 
		/// Each pipeline is created using a pipeline layout.
		if (isValid())
		{
			vkDestroyPipelineLayout(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};

class VulkanPipelineCache : public VulkanDeviceObject<VkPipelineCache>
{
public:
	void create(VkDevice device);
	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			vkDestroyPipelineCache(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};

class VulkanPipeline : public VulkanDeviceObject<VkPipeline>
{
public:
	void destroy(VkDevice device) override
	{
		if (isValid())
		{
			vkDestroyPipeline(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};
using VulkanPipelinePtr = std::shared_ptr<VulkanPipeline>;

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
	VulkanGraphicsPipeline(
		VkDevice device,
		VkRenderPass renderPass,
		VkPipelineCache pipelineCache,
		const GfxPipelineState* state);

	void destroy(VkDevice device) override final
	{
		m_PipelineLayout.destroy(device);
		m_DescriptorSet.destroy(device);
		VulkanPipeline::destroy(device);
	}

	void updateDescriptorSet(VkDevice device, const GfxPipelineState* state);

	VkDescriptorSet descriptorSet() const
	{
		assert(m_DescriptorSet.isValid());
		return m_DescriptorSet.Handle;
	}

	VkPipelineLayout layout() const
	{
		assert(m_PipelineLayout.isValid());
		return m_PipelineLayout.Handle;
	}
protected:
	VkPipelineRasterizationStateCreateInfo makeRasterizationState(const GfxRasterizerStateDesc& stateDesc) const;
	VkPipelineDepthStencilStateCreateInfo makeDepthStencilState(const GfxDepthStencilStateDesc& stateDesc) const;
	VkPipelineColorBlendStateCreateInfo makeColorBlendState(
		std::vector<VkPipelineColorBlendAttachmentState>& attachments, 
		const GfxBlendStateDesc& stateDesc) const;
private:
	VulkanPipelineLayout m_PipelineLayout;
	VulkanDescriptorSet m_DescriptorSet;
};
using VulkanGraphicsPipelinePtr = std::shared_ptr<VulkanGraphicsPipeline>;

class VulkanComputePipeline : public VulkanPipeline
{
};
using VulkanComputePipelinePtr = std::shared_ptr<VulkanComputePipeline>;

class VulkanRayTracingPipeline : public VulkanPipeline
{
};

class VulkanPipelinePool : public LazySingleton<VulkanPipelinePool>
{
	lazySingletonDeclare(VulkanPipelinePool);
public:
	VulkanGraphicsPipelinePtr getOrCreateGfxPipeline(VkRenderPass renderPass, const GfxPipelineState* state);
	void cleanup() override final;
protected:
	VulkanPipelinePool(const VkDevice device)
		: m_Device(device)
	{
		m_PipelineCache.create(device);
	}
private:
	const VkDevice m_Device;
	VulkanPipelineCache m_PipelineCache;
	std::vector<std::pair<const GfxPipelineState*, VulkanGraphicsPipelinePtr>> m_GfxPipelines;
};
