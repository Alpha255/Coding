#pragma once

#include "VulkanObject.h"
#include "VulkanSwapChain.h"
#include "VulkanShader.h"
#include "VulkanView.h"
#include "VulkanState.h"
#include "Util/D3DMath.h"

class VulkanContext : public VulkanObject<VkPipeline>
{
public:
	///VulkanRenderTargetView m_RenderTargetView;
	//void SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot = 0U);
	//void SetDepthStencilView(const D3DDepthStencilView &depthStencilView);
	//void SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader);
	//void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader = D3DShader::eComputeShader);

	void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U);
	void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset);

	void SetSamplerState(const VulkanSamplerState &samplerState, uint32_t slot, uint32_t targetShader);
	void SetRasterizerState(const VulkanRasterizerState &rasterizerState);
	void SetDepthStencilState(const VulkanDepthStencilState &depthStencilState, uint32_t stencilRef);
	//void SetBlendState(const D3DBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF);

	///void SetInputLayout(const D3DInputLayout &inputLayout);
	void SetVertexShader(const VulkanVertexShader &vertexShader);
	//void SetHullShader(const D3DHullShader &hullShader);
	//void SetDomainShader(const D3DDomainShader &domainShader);
	void SetPixelShader(const VulkanPixelShader &pixelShader);
	//void SetGeometryShader(const D3DGeometryShader &geometryShader);
	//void SetComputeShader(const D3DComputeShader &computeShader);

	//void SetConstantBuffer(const VulkanBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader);

	//void SetViewport(const D3DViewport &viewport, uint32_t slot = 0U);
	//void SetScissorRect(const D3DRect &rect, uint32_t slot = 0U);

	void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive);

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

	void Create(const VkPipelineShaderStageCreateInfo *pShaderStage, const size_t shaderCount);
protected:
private:
	VkPipelineLayout m_Layout;
};
