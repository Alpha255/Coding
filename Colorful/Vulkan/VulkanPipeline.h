#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanCommand.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanPipelineLayout)
class VulkanPipelineLayout final : public VkObject<VkPipelineLayout_T>, public VkDeviceResource
{
public:
	VulkanPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);

	void destroy(VkDevice device) override final
	{
		/// The pipeline layout represents a sequence of descriptor sets with each having a specific layout. 
		/// This sequence of layouts is used to determine the interface between shader stages and shader resources. 
		/// Each pipeline is created using a pipeline layout.
		assert(device);
		vkDestroyPipelineLayout(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_SHARED_PTR(VulkanPipelineCache)
class VulkanPipelineCache final : public VkObject<VkPipelineCache_T>, public VkDeviceResource
{
public:
	VulkanPipelineCache(VkDevice device);

	void destroy(VkDevice device) override final
	{
		assert(device);
		vkDestroyPipelineCache(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_SHARED_PTR(VulkanPipeline)
class VulkanPipeline : public VkObject<VkPipeline_T>, public VkDeviceResource
{
public:
	void destroy(VkDevice device) override
	{
		assert(device);
		vkDestroyPipeline(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_SHARED_PTR(VulkanGraphicsPipeline)
class VulkanGraphicsPipeline final : public VulkanPipeline
{
public:
	VulkanGraphicsPipeline(VkDevice device, VkPipelineCache pipelineCache, const GraphicsPipelineState& state);

	void destroy(VkDevice device) override final
	{
		assert(device);
		m_PipelineLayout->destroy(device);
		m_DescriptorSetLayout->destroy(device);
		VulkanPipeline::destroy(device);
		m_WireframePipeline->destroy(device);
		///m_DescriptorSets.clear();
	}

	///void updateShaderResources(const GfxPipelineState* state);
protected:
private:
	VulkanPipelineLayoutPtr m_PipelineLayout;
	VulkanDescriptorSetLayoutPtr m_DescriptorSetLayout;
	VulkanDescriptorSetPtr m_DescriptorSet;
	VulkanPipelinePtr m_WireframePipeline;
};

DECLARE_SHARED_PTR(VulkanComputePipeline)
class VulkanComputePipeline final : public VulkanPipeline
{
};

DECLARE_SHARED_PTR(VulkanRayTracingPipeline)
class VulkanRayTracingPipeline final : public VulkanPipeline
{
};

NAMESPACE_END(Gfx)
