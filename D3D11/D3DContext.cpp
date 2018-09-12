#include "D3DContext.h"

void D3DContext::SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxRenderTargetViews);

	if (renderTarget != m_State.RenderTargetViews[slot])
	{
		m_State.RenderTargetViews[slot] = renderTarget.Get();

		m_State.RenderTargetsInUse |= (1U << slot);

		m_State.SetDirty(D3DContextState::eRenderTargetView, true);
	}
}

void D3DContext::SetDepthStencilView(const D3DDepthStencilView &depthStencilView)
{
	if (depthStencilView != m_State.DepthStencilView)
	{
		m_State.DepthStencilView = depthStencilView.Get();

		m_State.SetDirty(D3DContextState::eDepthStencilView, true);
	}
}

void D3DContext::SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxVertexStream);

	if (m_State.VertexStrides[slot] != stride ||
		m_State.VertexOffsets[slot] != offset ||
		vertexBuffer != m_State.VertexBuffers[slot])
	{
		if (vertexBuffer != m_State.VertexBuffers[slot])
		{
			m_State.VertexBuffersInUse |= (1U << slot);
		}

		m_State.VertexStrides[slot] = stride;
		m_State.VertexOffsets[slot] = offset;
		m_State.VertexBuffers[slot] = vertexBuffer.Get();

		m_State.SetDirty(D3DContextState::eVertexBuffer, true);
	}
}

void D3DContext::SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
{
	if (m_State.IndexFormat != fmt ||
		m_State.IndexOffset != offset ||
		indexBuffer != m_State.IndexBuffer)
	{
		m_State.IndexFormat = fmt;
		m_State.IndexOffset = offset;
		m_State.IndexBuffer = indexBuffer.Get();

		m_State.SetDirty(D3DContextState::eIndexBuffer, true);
	}
}

void D3DContext::SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxShaderResourceViews && targetShader < D3DShader::eShaderTypeCount);

	if (shaderResourceView != m_State.ShaderResourceViews[targetShader][slot])
	{
		m_State.ShaderResourceViews[targetShader][slot] = shaderResourceView.Get();

		m_State.ShaderResourceViewsInUse[targetShader] |= (1 << slot);

		m_State.SetDirty(D3DContextState::eShaderResourceView, true, targetShader);
	}
}

void D3DContext::SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxUnorderedAccessViews && targetShader == D3DShader::eComputeShader);

	if (unorderedAccessView != m_State.UnorderedAccessViews[slot])
	{
		m_State.UnorderedAccessViews[slot] = unorderedAccessView.Get();

		m_State.UnorderedAccessViewsInUse |= (1U << slot);

		m_State.SetDirty(D3DContextState::eUnorderedAccessView, true, targetShader);
	}
}

void D3DContext::SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxSamplers && targetShader < D3DShader::eShaderTypeCount);

	if (samplerState != m_State.SamplerStates[targetShader][slot])
	{
		m_State.SamplerStates[targetShader][slot] = samplerState.Get();

		m_State.SamplersInUse[targetShader] |= (1U << slot);

		m_State.SetDirty(D3DContextState::eSamplerState, true, targetShader);
	}
}

void D3DContext::SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxConstantBuffers && targetShader < D3DShader::eShaderTypeCount);

	if (constantBuffer != m_State.ConstantBuffers[targetShader][slot])
	{
		m_State.ConstantBuffers[targetShader][slot] = constantBuffer.Get();

		m_State.ConstantBuffersInUse[targetShader] |= (1U << slot);

		m_State.SetDirty(D3DContextState::eConstantBuffer, true, targetShader);
	}
}

void D3DContext::SetRasterizerState(const D3DRasterizerState &rasterizerState)
{
	if (rasterizerState != m_State.RasterizerState)
	{
		m_State.RasterizerState = rasterizerState.Get();

		m_State.SetDirty(D3DContextState::eRasterizerState, true);
	}
}

void D3DContext::SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef)
{
	if (m_State.StencilRef != stencilRef ||
		depthStencilState != m_State.DepthStencilState)
	{
		m_State.StencilRef = stencilRef;
		m_State.DepthStencilState = depthStencilState.Get();

		m_State.SetDirty(D3DContextState::eDepthStencilState, true);
	}
}

void D3DContext::SetBlendState(const D3DBlendState &blendState, Vec4 blendfactor, uint32_t mask)
{
	if (m_State.BlendMask != mask ||
		m_State.BlendFactor != blendfactor ||
		blendState != m_State.BlendState)
	{
		m_State.BlendMask = mask;
		m_State.BlendFactor = blendfactor;
		m_State.BlendState = blendState.Get();

		m_State.SetDirty(D3DContextState::eBlendState, true);
	}
}

void D3DContext::SetInputLayout(const D3DInputLayout &inputLayout)
{
	if (inputLayout != m_State.InputLayout)
	{
		m_State.InputLayout = inputLayout.Get();

		m_State.SetDirty(D3DContextState::eInputLayout, true);
	}
}

void D3DContext::SetVertexShader(const D3DVertexShader &vertexShader)
{
	if (vertexShader != m_State.VertexShader)
	{
		m_State.VertexShader = vertexShader.Get();

		m_State.SetDirty(D3DContextState::eVertexShader, true);
	}
}

void D3DContext::SetHullShader(const D3DHullShader &hullShader)
{
	if (hullShader != m_State.HullShader)
	{
		m_State.HullShader = hullShader.Get();

		m_State.SetDirty(D3DContextState::eHullShader, true);
	}
}

void D3DContext::SetDomainShader(const D3DDomainShader &domainShader)
{
	if (domainShader != m_State.DomainShader)
	{
		m_State.DomainShader = domainShader.Get();

		m_State.SetDirty(D3DContextState::eDomainShader, true);
	}
}

void D3DContext::SetPixelShader(const D3DPixelShader &pixelShader)
{
	if (pixelShader != m_State.PixelShader)
	{
		m_State.PixelShader = pixelShader.Get();

		m_State.SetDirty(D3DContextState::ePixelShader, true);
	}
}

void D3DContext::SetGeometryShader(const D3DGeometryShader &geometryShader)
{
	if (geometryShader != m_State.GeometryShader)
	{
		m_State.GeometryShader = geometryShader.Get();

		m_State.SetDirty(D3DContextState::eGeometryShader, true);
	}
}

void D3DContext::SetComputeShader(const D3DComputeShader &computeShader)
{
	if (computeShader != m_State.ComputeShader)
	{
		m_State.ComputeShader = computeShader.Get();

		m_State.SetDirty(D3DContextState::eComputeShader, true);
	}
}

void D3DContext::SetViewport(const D3DViewport &viewport, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxViewports);

	if (m_State.Viewports[slot] != viewport)
	{
		m_State.Viewports[slot] = viewport;

		m_State.ViewportsInUse |= (1U << slot);

		m_State.SetDirty(D3DContextState::eViewport, true);
	}
}

void D3DContext::SetScissorRect(const D3DRect &rect, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxScissorRects);

	if (m_State.ScissorRects[slot] != rect)
	{
		m_State.ScissorRects[slot] = rect;

		m_State.ScissorRectsInUse |= (1U << slot);

		m_State.SetDirty(D3DContextState::eScissorRect, true);
	}
}

void D3DContext::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
{
	if (m_State.PrimitiveTopology != primitive)
	{
		m_State.PrimitiveTopology = primitive;

		m_State.SetDirty(D3DContextState::ePrimitiveTopology, true);
	}

	m_State.CommitState(*this);

	m_Object->Draw(vertexCount, startVertex);
}

void D3DContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	if (m_State.PrimitiveTopology != primitive)
	{
		m_State.PrimitiveTopology = primitive;

		m_State.SetDirty(D3DContextState::ePrimitiveTopology, true);
	}

	m_State.CommitState(*this);

	m_Object->DrawIndexed(indexCount, startIndex, offset);
}