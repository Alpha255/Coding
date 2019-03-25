#pragma once

#include "VulkanObject.h"
#include "VulkanSwapChain.h"
#include "VulkanShader.h"
#include "VulkanView.h"
#include "VulkanState.h"
#include "VulkanPool.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
///#include "VulkanContextState.h"

template <typename Left, typename Right>
inline bool IsEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return memcmp(&left, &right, sizeof(Right)) == 0;
}

class VulkanPipelineLayout : public VulkanObject<VkPipelineLayout>
{
public:
	void Create();
	void Destory() override;
protected:
private:
};

class VulkanPipelineCache : public VulkanObject<VkPipelineCache>
{
public:
	void Create();
	void Destory() override;
protected:
private:
};

class VulkanPipeline : public VulkanObject<VkPipeline>
{
public:
	void Create(const VkGraphicsPipelineCreateInfo &createInfo, const VulkanPipelineCache &cache);
	void Destory() override;

	inline const VkGraphicsPipelineCreateInfo &GetPipelineInfo() const
	{
		assert(IsValid());
		return m_CreateInfo;
	}
protected:
private:
	VulkanPipelineLayout m_Layout;
	VkGraphicsPipelineCreateInfo m_CreateInfo = {};
};

class VulkanContext
{
public:
	inline void Initialize()
	{
		m_Cache.Create();

		m_CurPipelineInfo.pStages = m_StateInfos.ShaderStageInfos.data();
		m_CurPipelineInfo.pVertexInputState = &m_StateInfos.VertexInputInfo;
		m_CurPipelineInfo.pInputAssemblyState = &m_StateInfos.InputAssemblyInfo;
		m_CurPipelineInfo.pTessellationState = &m_StateInfos.TessellationInfo;
		m_CurPipelineInfo.pViewportState = &m_StateInfos.ViewportInfo;
		m_CurPipelineInfo.pRasterizationState = &m_StateInfos.RasterizationInfo;
		m_CurPipelineInfo.pMultisampleState = &m_StateInfos.MultisampleInfo;
		m_CurPipelineInfo.pDepthStencilState = &m_StateInfos.DepthStencilInfo;
		m_CurPipelineInfo.pColorBlendState = &m_StateInfos.ColorBlendInfo;
		m_CurPipelineInfo.pDynamicState = &m_StateInfos.DynamicInfo;
		m_CurPipelineInfo.flags = 0U;
		m_CurPipelineInfo.basePipelineIndex = -1;
		m_CurPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	}

	void BuildPipline();

	void Finalize();

	inline void SetRenderTargetView(const VulkanRenderTargetView &renderTargetView, uint32_t slot = 0U)
	{
	}

	inline void SetDepthStencilView(const VulkanDepthStencilView &depthStencilView)
	{
	}

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
	}

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t format, uint32_t offset)
	{
	}

	inline void SetSamplerState(const VulkanSamplerState &samplerState, uint32_t slot, uint32_t targetShader)
	{
	}

	inline void SetRasterizerState(const VulkanRasterizerState &rasterizerState)
	{
		m_StateInfos.RasterizationInfo = rasterizerState.Get();
	}

	inline void SetDepthStencilState(const VulkanDepthStencilState &depthStencilState, uint32_t)
	{
		m_StateInfos.DepthStencilInfo = depthStencilState.Get();
	}

	inline void SetBlendState(const VulkanBlendState &blendState, Vec4, uint32_t)
	{
		m_StateInfos.ColorBlendInfo = blendState.Get();
	}

	inline void SetInputLayout(const VulkanInputLayout &inputLayout)
	{
		m_StateInfos.VertexInputInfo = inputLayout.Get();
	}

	inline void SetVertexShader(const VulkanVertexShader &vertexShader)
	{
		m_StateInfos.ShaderStageInfoArray[eVertexShader] = std::make_pair(vertexShader.GetShaderStage(), true);
	}

	inline void SetPixelShader(const VulkanPixelShader &pixelShader)
	{
		m_StateInfos.ShaderStageInfoArray[ePixelShader] = std::make_pair(pixelShader.GetShaderStage(), true);
	}

	void SetConstantBuffer(const VulkanBuffer &constantBuffer, uint32_t slot, eRShaderType targetShader)
	{
	}

	inline void SetViewport(const VulkanViewport &viewport, uint32_t slot = 0U)
	{
	}

	inline void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U)
	{
	}

	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive);
protected:
	struct VulkanStateInfos
	{
		std::array<std::pair<VkPipelineShaderStageCreateInfo, bool>, eRShaderTypeCount> ShaderStageInfoArray;
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStageInfos;
		VkPipelineVertexInputStateCreateInfo VertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
		VkPipelineTessellationStateCreateInfo TessellationInfo;
		VkPipelineViewportStateCreateInfo ViewportInfo;
		VkPipelineRasterizationStateCreateInfo RasterizationInfo;
		VkPipelineMultisampleStateCreateInfo MultisampleInfo;
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
		VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
		VkPipelineDynamicStateCreateInfo DynamicInfo;
	};

	bool IsSamePipelineState(const VkGraphicsPipelineCreateInfo &left, const VkGraphicsPipelineCreateInfo &right);
private:
	VkGraphicsPipelineCreateInfo m_CurPipelineInfo = {};
	VulkanPipeline m_CurPipline;
	VulkanPipelineCache m_Cache;
	std::vector<VulkanPipeline> m_Pipelines;
	VulkanStateInfos m_StateInfos = {};
};
