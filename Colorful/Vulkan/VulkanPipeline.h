#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanRenderPass.h"

NAMESPACE_START(Gfx)

class VulkanPipelineLayout final : public VkDeviceObject<VkPipelineLayout_T>
{
public:
	VulkanPipelineLayout(const PipelineResourceTable& desc);
	~VulkanPipelineLayout();

	VkDescriptorSetLayout descriptorSetLayout() const
	{
		assert(isValid());
		return m_DescriptorSetLayout;
	}
protected:
private:
	VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
};
DECLARE_SHARED_PTR(VulkanPipelineLayout)

class VulkanPipelineCache : public VkDeviceObject<VkPipelineCache_T>
{
public:
	VulkanPipelineCache();
	~VulkanPipelineCache();

	void load() {}
	void save() {}
};
DECLARE_SHARED_PTR(VulkanPipelineCache)

class VulkanPipeline : public VkDeviceObject<VkPipeline_T>
{
public:
	virtual ~VulkanPipeline();
};
DECLARE_SHARED_PTR(VulkanPipeline)

class VulkanGraphicsPipeline final : public VulkanPipeline
{
public:
	VulkanGraphicsPipeline(VkRenderPass renderPass, GraphicsPipelineState& state);
	~VulkanGraphicsPipeline();
protected:
private:
	VulkanPipelineLayoutPtr m_PipelineLayout;
	VulkanDescriptorSetPtr m_DescriptorSet;
	VkPipeline m_WireframePipeline = VK_NULL_HANDLE;
};
DECLARE_SHARED_PTR(VulkanGraphicsPipeline)

class VulkanComputePipeline final : public VulkanPipeline
{
};
DECLARE_SHARED_PTR(VulkanComputePipeline)

class VulkanRayTracingPipeline final : public VulkanPipeline
{
};
DECLARE_SHARED_PTR(VulkanRayTracingPipeline)

class VkRenderStateCache
{
public:
	static VkRenderStateCache& instance();

	void initialize(class VulkanSwapchain* swapchain);
	void finalize();

	VulkanRenderPassPtr getOrCreateRenderPass(const RenderPassDesc& desc);
	VulkanGraphicsPipelinePtr getOrCreateGraphicPipeline(VkRenderPass renderPass, GraphicsPipelineState& state);
	VulkanFramebufferPtr getOrCreateFrameBuffer(VkRenderPass renderPass, const FrameBufferDesc& desc);

	inline VkPipelineCache pipelineCache() const
	{
		assert(m_PipelineCache);
		return m_PipelineCache->get();
	}
protected:
private:
	VulkanPipelineCachePtr m_PipelineCache = nullptr;
	std::vector<std::pair<size_t, VulkanRenderPassPtr>> m_RenderPassCache;
	std::vector<std::pair<size_t, VulkanGraphicsPipelinePtr>> m_GfxPipelineCache;
	std::vector<std::pair<size_t, VulkanFramebufferPtr>> m_FrameBufferCache;
	std::vector<VulkanFramebufferPtr> m_BackBuffers;
	class VulkanSwapchain* m_Swapchain = nullptr;
};

NAMESPACE_END(Gfx)
