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

class VulkanPipelineLayout : public VulkanObject<VkPipelineLayout>
{
public:
	void Create(const VulkanDescriptorSetLayout &descriptorSetLayout);
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

	inline void SetLayout(const VulkanPipelineLayout &layout)
	{
		m_Layout = layout;
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

		m_CurPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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

		m_DescriptorPool.Create();
	}

	inline void SetRenderTargetView(const VulkanRenderTargetView &, uint32_t = 0U)
	{
	}

	inline void SetDepthStencilView(const VulkanDepthStencilView &)
	{
	}

	inline void SetClearColor(const Vec4 &color)
	{
		memcpy(m_ClearColor[0].color.float32, &color, sizeof(Vec4));
		m_ClearColor[1].depthStencil = { 1.0f, 0U };
	}

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
		SetImageView(shaderResourceView, slot, targetShader);
	}

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
		assert(slot < eMaxVertexBuffers);
		if (m_VertexBuffers[slot].Buffer != vertexBuffer)
		{
			m_VertexBuffers[slot].Buffer = vertexBuffer;
			m_VertexBuffers[slot].Stride = stride;
			m_VertexBuffers[slot].Offset = offset;
		}
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t format, uint32_t offset)
	{
		if (m_IndexBuffer.Buffer != indexBuffer)
		{
			m_IndexBuffer.Buffer = indexBuffer;
			m_IndexBuffer.Format = (format == eR16_UInt ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
			m_IndexBuffer.Offset = offset;
		}
	}

	inline void SetRasterizerState(const VulkanRasterizerState &rasterizerState)
	{
		m_StateInfos.RasterizationInfo = rasterizerState.Get();
	}

	inline void SetDepthStencilState(const VulkanDepthStencilState &, uint32_t)
	{
		///m_StateInfos.DepthStencilInfo = depthStencilState.Get();
	}

	inline void SetBlendState(const VulkanBlendState &blendState, Vec4, uint32_t)
	{
		m_StateInfos.ColorBlendInfo = blendState.Get();
	}

	inline void SetSamplerState(const VulkanSamplerState &sampler, uint32_t slot, eRShaderType targetShader)
	{
		SetSampler(sampler, slot, targetShader);
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

	inline void SetViewport(const VulkanViewport &viewport, uint32_t slot = 0U)
	{
		if (m_Viewports[slot] != viewport)
		{
			m_Viewports[slot] = viewport;
			m_Viewports[slot].SetSlot(slot);

			//++m_StateInfos.ViewportInfo.viewportCount;
			//m_StateInfos.ViewportInfo.pViewports = m_Viewports.data();
		}
	}

	inline void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U)
	{
		if (m_Scissors[slot] != rect)
		{
			m_Scissors[slot] = rect;
			m_Scissors[slot].SetSlot(slot);

			//++m_StateInfos.ViewportInfo.scissorCount;
			//m_StateInfos.ViewportInfo.pScissors = m_Scissors.data();
		}
	}

	void SetUniformBuffer(const VulkanBuffer &uniformBuffer, uint32_t slot, eRShaderType targetShader);

	void BuildPipline();

	void BuildDescriptorSet();

	void Finalize();

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
		VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;
		std::vector<VkDynamicState> DynamicStates;

		inline VulkanStateInfos()
		{
			TessellationInfo = VkPipelineTessellationStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
				nullptr,
				0U,
				0U
			};

			ViewportInfo = VkPipelineViewportStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				nullptr,
				0U,
				1U,
				nullptr,
				1U,
				nullptr
			};

			RasterizationInfo = VkPipelineRasterizationStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				nullptr,
				0U,
				VK_FALSE,
				VK_TRUE,
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_NONE,
				VK_FRONT_FACE_COUNTER_CLOCKWISE,
				VK_FALSE,
				0.0f, 
				0.0f, 
				0.0f,
				1.0f
			};

			MultisampleInfo = VkPipelineMultisampleStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				nullptr,
				0U,
				VK_SAMPLE_COUNT_1_BIT,
				VK_FALSE,
				0.0f,
				nullptr,
				VK_FALSE,
				VK_FALSE
			};

			DepthStencilInfo = VkPipelineDepthStencilStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				nullptr,
				0U,
				VK_FALSE,
				VK_FALSE,
				VK_COMPARE_OP_ALWAYS,
				VK_FALSE,
				VK_FALSE,
				VkStencilOpState(),
				VkStencilOpState(),
				0.0f,
				1.0f
			};

			ColorBlendAttachmentState = VkPipelineColorBlendAttachmentState
			{
				VK_FALSE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				0xF
			};

			ColorBlendInfo = VkPipelineColorBlendStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				nullptr,
				0U,
				VK_FALSE,
				VK_LOGIC_OP_CLEAR,
				1U,
				&ColorBlendAttachmentState,
				{ 0.0f, 0.0f, 0.0f, 0.0f }
			};

			DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
			DynamicInfo = VkPipelineDynamicStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				nullptr,
				0U,
				(uint32_t)DynamicStates.size(),
				DynamicStates.data()
			};
		}
	};

	struct VulkanDescriptorSetInfos
	{
		VulkanDescriptorPool::eDescriptorType Type = VulkanDescriptorPool::ePoolTypeCount;
		uint32_t Slot = 0U;
		VulkanImageView Image;
		VulkanSamplerState Sampler;
		VulkanBuffer UniformBuffer;
		eRShaderType ShaderType;
	};

	struct VulkanVertexBuffer
	{
		VulkanBuffer Buffer;
		uint32_t Stride = 0U;
		uint32_t Offset = 0U;
	};

	struct VulkanIndexBuffer
	{
		VulkanBuffer Buffer;
		VkIndexType Format;
		uint32_t Offset = 0U;
	};

	enum eLimits
	{
		eMaxViewports = 16U,
		eMaxScissors = 16U,
		eMaxVertexBuffers = 2U
	};

	bool IsSamePipelineState(const VkGraphicsPipelineCreateInfo &left, const VkGraphicsPipelineCreateInfo &right);

	void SetImageView(const VulkanImageView &imageView, uint32_t slot, eRShaderType targetShader);
	void SetSampler(const VulkanSamplerState &sampler, uint32_t slot, eRShaderType targetShader);

	inline VkShaderStageFlags GetShaderStage(eRShaderType type) const
	{
		VkShaderStageFlags shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		switch (type)
		{
		case eVertexShader:
			shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case ePixelShader:
			shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		default:
			assert(0);
			break;
		}
		
		return shaderStage;
	}
private:
	VkGraphicsPipelineCreateInfo m_CurPipelineInfo = {};
	VulkanPipeline m_CurPipline;
	VulkanPipelineCache m_Cache;
	VulkanDescriptorPool m_DescriptorPool;
	VulkanIndexBuffer m_IndexBuffer;
	VkClearValue m_ClearColor[2U];
	std::vector<VulkanPipeline> m_Pipelines;
	std::vector<VulkanPipelineLayout> m_PipelineLayouts;
	std::vector<VulkanDescriptorSet> m_DescriptorSets;
	std::vector<VulkanDescriptorSetInfos> m_DescriptorSetInfos;
	std::array<VulkanViewport, eMaxViewports> m_Viewports;
	std::array<VulkanRect, eMaxScissors> m_Scissors;
	std::array<VulkanVertexBuffer, eMaxVertexBuffers> m_VertexBuffers;
	std::array<std::array<VulkanImageView, VulkanDescriptorPool::eMaxSamplers>, eRShaderTypeCount> m_ImageViews;
	std::array<std::array<VulkanSamplerState, VulkanDescriptorPool::eMaxSamplers>, eRShaderTypeCount> m_Samplers;
	std::array<std::array<VulkanBuffer, VulkanDescriptorPool::eMaxUniformBuffers>, eRShaderTypeCount> m_UniformBuffers;
	uint32_t m_ResourceBinding = 0U;
	VulkanStateInfos m_StateInfos = {};
	bool m_Dirty = false;
};
