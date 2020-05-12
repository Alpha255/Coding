#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"

class vkPipelineLayout : public VulkanDeviceObject<VkPipelineLayout>
{
public:
	void create(VkDevice device, const vkDescriptorSetLayout &descriptorSetLayout);
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

class vkPipelineCache : public VulkanDeviceObject<VkPipelineCache>
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

class vkPipeline : public VulkanDeviceObject<VkPipeline>
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

class vkGraphicsPipeline : public vkPipeline
{
public:
	void create(
		VkDevice device,
		const class VulkanRenderPass &renderpass,
		const vkPipelineCache &cache,
		const GfxPipelineState &state);

	void destroy(VkDevice device) override final
	{
		m_PipelineLayout.destroy(device);
		m_DescriptorSetLayout.destroy(device);
		vkPipeline::destroy(device);
	}

	void bind(const class VulkanCommandBuffer &cmdBuffer);
protected:
	VkPipelineRasterizationStateCreateInfo getRasterizationState(const GfxRasterizerStateDesc &stateDesc) const;
	VkPipelineDepthStencilStateCreateInfo getDepthStencilState(const GfxDepthStencilStateDesc &stateDesc) const;
	VkPipelineColorBlendStateCreateInfo getColorBlendState(
		std::vector<VkPipelineColorBlendAttachmentState> &attachments, 
		const GfxBlendStateDesc &stateDesc) const;

	void setupDescriptorSet(VkDevice device, const GfxPipelineState &state);
private:
	vkPipelineLayout m_PipelineLayout;
	vkDescriptorSetLayout m_DescriptorSetLayout;
	vkDescriptorSet m_DescriptorSet;
};

class vkComputePipeline : public vkPipeline, public GfxComputePipeline
{
};

class vkRayTracingPipeline : public vkPipeline
{
};
