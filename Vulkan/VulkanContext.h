#pragma once

#include "VulkanObject.h"
#include "VulkanSwapChain.h"
#include "VulkanShader.h"
#include "VulkanView.h"
#include "VulkanState.h"
#include "VulkanPool.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanContextState.h"

//class VulkanPipelineLayout : public VulkanObject<VkPipelineLayout>
//{
//public:
//	void Create(bool bUseTex);
//protected:
//private:
//};
//
//class VulkanPipelineCache : public VulkanObject<VkPipelineCache>
//{
//public:
//protected:
//private:
//};

class VulkanContext : public VulkanObject<VkPipeline>
{
public:
	inline void Initialize()
	{
		m_State.Initialize();
	}

	inline void SetRenderTargetView(const VulkanRenderTargetView &renderTarget, uint32_t slot = 0U)
	{

	}

	inline void SetDepthStencilView(const VulkanDepthStencilView &depthStencilView)
	{

	}

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
		assert(slot < m_State.ShaderResourceViews.size());

		if (shaderResourceView.Get() != m_State.ShaderResourceViews[slot])
		{
			m_State.ShaderResourceViews[slot] = shaderResourceView.Get();

			m_State.SetDirty(true);
		}
	}

	//void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader = D3DShader::eComputeShader);

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
		if (vertexBuffer.Get() != m_State.VertexBuffers[slot].Buffer)
		{
			m_State.VertexBuffers[slot].VertexInputBindingDescription.binding = slot;
			m_State.VertexBuffers[slot].VertexInputBindingDescription.stride = stride;
			m_State.VertexBuffers[slot].VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			m_State.VertexBuffers[slot].Buffer = vertexBuffer.Get();

			m_State.SetDirty(true);
		}
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
		if (indexBuffer.Get() != m_State.IndexBuffer.Buffer)
		{
			m_State.IndexBuffer.Buffer = indexBuffer.Get();

			if (fmt == eR16_UInt)
			{
				m_State.IndexBuffer.Format = VK_INDEX_TYPE_UINT16;
			}
			else if (fmt == eR32_UInt)
			{
				m_State.IndexBuffer.Format = VK_INDEX_TYPE_UINT32;
			}
			else
			{
				assert(0);
			}

			m_State.IndexBuffer.Offset = offset;

			m_State.SetDirty(true);
		}
	}

	inline void SetSamplerState(const VulkanSamplerState &samplerState, uint32_t slot, uint32_t targetShader)
	{
		assert(targetShader < eRShaderTypeCount && slot < m_State.SamplerBindInfos[targetShader].Samplers.size());

		if (samplerState.Get() != m_State.SamplerBindInfos[targetShader].Samplers[slot])
		{
			m_State.SamplerBindInfos[targetShader].Samplers[slot] = samplerState.Get();

			m_State.SetDirty(true);
		}
	}

	inline void SetRasterizerState(const VulkanRasterizerState &rasterizerState)
	{
		if (!IsEqual(rasterizerState.Get(), m_State.RasterizationState))
		{
			m_State.RasterizationState = rasterizerState.Get();

			m_State.SetDirty(true);
		}
	}

	inline void SetDepthStencilState(const VulkanDepthStencilState &depthStencilState, uint32_t stencilRef)
	{
		if (!IsEqual(depthStencilState.Get(), m_State.DepthStencilState))
		{
			///m_State.DepthStencilState = depthStencilState.Get();

			///m_State.SetDirty(true);
		}
	}

	inline void SetBlendState(const VulkanBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF)
	{
		if (!IsEqual(blendState.Get(), m_State.ColorBlendState))
		{
			///m_State.ColorBlendState = blendState.Get();

			///m_State.SetDirty(true);
		}
	}

	inline void SetInputLayout(const VulkanInputLayout &inputLayout)
	{
		if (!IsEqual(inputLayout.Get(), m_State.VertexBuffers[0].VertexInputAttributeDescription))
		{
			m_State.VertexBuffers[0].VertexInputAttributeDescription = inputLayout.Get();

			m_State.SetDirty(true);
		}
	}

	inline void SetVertexShader(const VulkanVertexShader &vertexShader) 
	{
		if (vertexShader.Get() != m_State.VertexShader)
		{
			m_State.VertexShader = vertexShader.Get();

			m_State.SetDirty(true);
		}
	}
	//void SetHullShader(const D3DHullShader &hullShader);
	//void SetDomainShader(const D3DDomainShader &domainShader);
	inline void SetPixelShader(const VulkanPixelShader &pixelShader)
	{
		if (pixelShader.Get() != m_State.PixelShader)
		{
			m_State.PixelShader = pixelShader.Get();

			m_State.SetDirty(true);
		}
	}
	//void SetGeometryShader(const D3DGeometryShader &geometryShader);
	//void SetComputeShader(const D3DComputeShader &computeShader);

	void SetConstantBuffer(const VulkanBuffer &constantBuffer, uint32_t slot, eRShaderType targetShader) 
	{

	}

	inline void SetViewport(const VulkanViewport &viewport, uint32_t slot = 0U) 
	{
		assert(slot < m_State.Viewports.size());
		
		if (!IsEqual(viewport, m_State.Viewports[slot]))
		{
			m_State.Viewports[slot] = viewport;

			m_State.SetDirty(true);
		}
	}

	inline void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U)
	{
		assert(slot < m_State.ScissorRects.size());

		if (!IsEqual(rect, m_State.ScissorRects[slot]))
		{
			m_State.ScissorRects[slot] = rect;

			m_State.SetDirty(true);
		}
	}

	//void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive);
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

	void Recreate();
	void Destory(bool bDestoryState = false);

	inline VkPipelineLayout GetLayout() const
	{
		return m_Layout;
	}
protected:
private:
	VkPipelineLayout m_Layout = {};
	VulkanContextState m_State;
};
