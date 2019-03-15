#pragma once

#include "VulkanPool.h"

struct VulkanContextState
{	
public:
	inline bool IsDirty() const
	{
		return m_bDirty;
	}

	inline void SetDirty(bool bDirty)
	{
		m_bDirty = bDirty;
	}

	inline void ClearState()
	{
		memset(this, 0, sizeof(VulkanContextState));
	}

	inline void Destory()
	{
		DescriptorPool.Destory();
	}

	void Initialize();

	void UpdateDescriptorSets();

	void CommitState(class VulkanContext &ctx, uint32_t indexCount, uint32_t startIndex, int32_t offset);

	friend class VulkanContext;

protected:

	struct VulkanVertexBuffer
	{
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkVertexInputBindingDescription VertexInputBindingDescription = {};
		std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescription;
	};

	struct VulkanIndexBuffer
	{
		VkBuffer Buffer = VK_NULL_HANDLE;
		uint32_t Format = 0U;
		uint32_t Offset = 0U;
	};

	struct VulkanSamplerBindInfo
	{
		std::vector<VkSampler> Samplers;
		VulkanDescriptorSet DescriptorSet = VK_NULL_HANDLE;
		VulkanDescriptorSetLayout DescriptorSetLayout;
	};

private:
	std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
	VkPipelineVertexInputStateCreateInfo VertexInputState = {};
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {};
	VkPipelineTessellationStateCreateInfo TessellationState = {};
	VkPipelineViewportStateCreateInfo ViewportState = {};
	VkPipelineRasterizationStateCreateInfo RasterizationState = {};
	VkPipelineMultisampleStateCreateInfo MultisampleState = {};
	VkPipelineDepthStencilStateCreateInfo DepthStencilState = {};
	VkPipelineColorBlendStateCreateInfo ColorBlendState = {};
	VkPipelineDynamicStateCreateInfo DynamicState = {};

	VkShaderModule VertexShader = VK_NULL_HANDLE;
	VkShaderModule PixelShader = VK_NULL_HANDLE;

	std::vector<VkViewport> Viewports;
	std::vector<VkRect2D> ScissorRects;
	std::vector<VulkanVertexBuffer> VertexBuffers;
	std::array<VulkanSamplerBindInfo, eRShaderTypeCount> SamplerBindInfos;
	std::vector<VkDescriptorSet> DescriptorSets;
	std::vector<VkImageView> ShaderResourceViews;
	VulkanIndexBuffer IndexBuffer;

	VulkanDescriptorPool DescriptorPool;

	bool m_bDirty = true;
};
