#pragma once

#include "VulkanObject.h"
#include "VulkanSwapChain.h"
#include "VulkanShader.h"
#include "VulkanView.h"
#include "VulkanState.h"
#include "VulkanPool.h"
#include "VulkanTexture.h"
#include "Util/D3DMath.h"

//class VulkanPipelineLayout : public VulkanObject<VkPipelineLayout>
//{
//public:
//	void Create(bool bUseTex);
//protected:
//private:
//};

//class VulkanPipelineCache : public VulkanObject<VkPipelineCache>
//{
//public:
//protected:
//private:
//};

class VulkanContext : public VulkanObject<VkPipeline>
{
public:
	inline void SetRenderTargetView(const VulkanRenderTargetView &renderTarget, uint32_t slot = 0U)
	{

	}

	inline void SetDepthStencilView(const VulkanDepthStencilView &depthStencilView)
	{

	}

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{

	}

	//void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader = D3DShader::eComputeShader);

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
		m_VIBindingDesc.binding = slot;
		m_VIBindingDesc.stride = stride;
		m_VIBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{

	}

	void SetSamplerState(const VulkanSamplerState &samplerState, uint32_t slot, uint32_t targetShader);
	inline void SetRasterizerState(const VulkanRasterizerState &rasterizerState)
	{
		m_RasterizationState = rasterizerState.Get();
	}

	inline void SetDepthStencilState(const VulkanDepthStencilState &depthStencilState, uint32_t stencilRef)
	{
		m_DepthStencilState = depthStencilState.Get();
	}
	void SetBlendState(const VulkanBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF);

	inline void SetInputLayout(const VulkanInputLayout &inputLayout)
	{
		m_VertexInputAttrDesc = inputLayout.Get();
	}
	void SetVertexShader(const VulkanVertexShader &vertexShader);
	//void SetHullShader(const D3DHullShader &hullShader);
	//void SetDomainShader(const D3DDomainShader &domainShader);
	void SetPixelShader(const VulkanPixelShader &pixelShader);
	//void SetGeometryShader(const D3DGeometryShader &geometryShader);
	//void SetComputeShader(const D3DComputeShader &computeShader);

	void SetConstantBuffer(const VulkanBuffer &constantBuffer, uint32_t slot, eRShaderType targetShader);

	void SetViewport(const VulkanViewport &viewport, uint32_t slot = 0U);
	void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U);

	//void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive);
	//void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive);

	//inline void ClearRenderTargetView(D3DRenderTargetView &renderTarget, const Vec4 &color) const
	//{
	//	assert(renderTarget.IsValid() && IsValid());

	//	m_Object->ClearRenderTargetView(renderTarget.Get(), (float *)&color);
	//}

	//inline void ClearDepthStencilView(D3DDepthStencilView &depthStencil, uint32_t clearFlag, float depth, uint8_t stencil) const
	//{
	//	assert(depthStencil.IsValid() && IsValid());

	//	m_Object->ClearDepthStencilView(depthStencil.Get(), clearFlag, depth, stencil);
	//}

	void Create();
protected:
private:
	VkPipelineLayout m_Layout = {};
	VkGraphicsPipelineCreateInfo m_Info = {};
	VkVertexInputBindingDescription m_VIBindingDesc = {};
	VkPipelineDepthStencilStateCreateInfo m_DepthStencilState = {};
	VkPipelineRasterizationStateCreateInfo m_RasterizationState = {};
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttrDesc;
};
