#include "D3D11Context.h"

void D3D11Context::SetRenderTargetView(const D3D11RenderTargetView &renderTarget, uint32_t slot)
{
	assert(slot < D3D11ContextState::eMaxRenderTargetViews);

	if (renderTarget != m_State.RenderTargetViews[slot])
	{
		m_State.RenderTargetViews[slot] = renderTarget.Get();

		m_State.RenderTargetsInUse |= (1U << slot);

		m_State.SetDirty(D3D11ContextState::eDirtyRenderTargetView, true);
	}
}

void D3D11Context::SetDepthStencilView(const D3D11DepthStencilView &depthStencilView)
{
	if (depthStencilView != m_State.DepthStencilView)
	{
		m_State.DepthStencilView = depthStencilView.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyDepthStencilView, true);
	}
}

void D3D11Context::SetVertexBuffer(const D3D11Buffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
{
	assert(slot < D3D11ContextState::eMaxVertexStream);

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

		m_State.SetDirty(D3D11ContextState::eDirtyVertexBuffer, true);
	}
}

void D3D11Context::SetIndexBuffer(const D3D11Buffer &indexBuffer, uint32_t fmt, uint32_t offset)
{
	if (m_State.IndexFormat != fmt ||
		m_State.IndexOffset != offset ||
		indexBuffer != m_State.IndexBuffer)
	{
		m_State.IndexFormat = fmt;
		m_State.IndexOffset = offset;
		m_State.IndexBuffer = indexBuffer.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyIndexBuffer, true);
	}
}

void D3D11Context::SetShaderResourceView(const D3D11ShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
{
	assert(slot < D3D11ContextState::eMaxShaderResourceViews && targetShader < eRShaderTypeCount);

	if (shaderResourceView != m_State.ShaderResourceViews[targetShader][slot])
	{
		m_State.ShaderResourceViews[targetShader][slot] = shaderResourceView.Get();

		m_State.ShaderResourceViewsInUse[targetShader] |= (1 << slot);

		m_State.SetDirty(D3D11ContextState::eDirtyShaderResourceView, true, targetShader);
	}
}

void D3D11Context::SetUnorderedAccessView(const D3D11UnorderedAccessView &unorderedAccessView, uint32_t slot, eRShaderType targetShader)
{
	assert(slot < D3D11ContextState::eMaxUnorderedAccessViews && targetShader == eComputeShader);

	if (unorderedAccessView != m_State.UnorderedAccessViews[slot])
	{
		m_State.UnorderedAccessViews[slot] = unorderedAccessView.Get();

		m_State.UnorderedAccessViewsInUse |= (1U << slot);

		m_State.SetDirty(D3D11ContextState::eDirtyUnorderedAccessView, true, targetShader);
	}
}

void D3D11Context::SetSamplerState(const D3D11SamplerState &samplerState, uint32_t slot, eRShaderType targetShader)
{
	assert(slot < D3D11ContextState::eMaxSamplers && targetShader < eRShaderTypeCount);

	if (samplerState != m_State.SamplerStates[targetShader][slot])
	{
		m_State.SamplerStates[targetShader][slot] = samplerState.Get();

		m_State.SamplersInUse[targetShader] |= (1U << slot);

		m_State.SetDirty(D3D11ContextState::eDirtySamplerState, true, targetShader);
	}
}

void D3D11Context::SetConstantBuffer(const D3D11Buffer &constantBuffer, uint32_t slot, eRShaderType targetShader)
{
	assert(slot < D3D11ContextState::eMaxConstantBuffers && targetShader < eRShaderTypeCount);

	if (constantBuffer != m_State.ConstantBuffers[targetShader][slot])
	{
		m_State.ConstantBuffers[targetShader][slot] = constantBuffer.Get();

		m_State.ConstantBuffersInUse[targetShader] |= (1U << slot);

		m_State.SetDirty(D3D11ContextState::eDirtyConstantBuffer, true, targetShader);
	}
}

void D3D11Context::SetRasterizerState(const D3D11RasterizerState &rasterizerState)
{
	if (rasterizerState != m_State.RasterizerState)
	{
		m_State.RasterizerState = rasterizerState.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyRasterizerState, true);
	}
}

void D3D11Context::SetDepthStencilState(const D3D11DepthStencilState &depthStencilState, uint32_t stencilRef)
{
	if (m_State.StencilRef != stencilRef ||
		depthStencilState != m_State.DepthStencilState)
	{
		m_State.StencilRef = stencilRef;
		m_State.DepthStencilState = depthStencilState.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyDepthStencilState, true);
	}
}

void D3D11Context::SetBlendState(const D3D11BlendState &blendState, Vec4 blendfactor, uint32_t mask)
{
	if (m_State.BlendMask != mask ||
		m_State.BlendFactor != blendfactor ||
		blendState != m_State.BlendState)
	{
		m_State.BlendMask = mask;
		m_State.BlendFactor = blendfactor;
		m_State.BlendState = blendState.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyBlendState, true);
	}
}

void D3D11Context::SetInputLayout(const D3D11InputLayout &inputLayout)
{
	if (inputLayout != m_State.InputLayout)
	{
		m_State.InputLayout = inputLayout.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyInputLayout, true);
	}
}

void D3D11Context::SetVertexShader(const D3D11VertexShader &vertexShader)
{
	if (vertexShader != m_State.VertexShader)
	{
		m_State.VertexShader = vertexShader.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyVertexShader, true);
	}
}

void D3D11Context::SetHullShader(const D3D11HullShader &hullShader)
{
	if (hullShader != m_State.HullShader)
	{
		m_State.HullShader = hullShader.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyHullShader, true);
	}
}

void D3D11Context::SetDomainShader(const D3D11DomainShader &domainShader)
{
	if (domainShader != m_State.DomainShader)
	{
		m_State.DomainShader = domainShader.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyDomainShader, true);
	}
}

void D3D11Context::SetPixelShader(const D3D11PixelShader &pixelShader)
{
	if (pixelShader != m_State.PixelShader)
	{
		m_State.PixelShader = pixelShader.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyPixelShader, true);
	}
}

void D3D11Context::SetGeometryShader(const D3D11GeometryShader &geometryShader)
{
	if (geometryShader != m_State.GeometryShader)
	{
		m_State.GeometryShader = geometryShader.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyGeometryShader, true);
	}
}

void D3D11Context::SetComputeShader(const D3D11ComputeShader &computeShader)
{
	if (computeShader != m_State.ComputeShader)
	{
		m_State.ComputeShader = computeShader.Get();

		m_State.SetDirty(D3D11ContextState::eDirtyComputeShader, true);
	}
}

void D3D11Context::SetViewport(const D3D11Viewport &viewport, uint32_t slot)
{
	assert(slot < D3D11ContextState::eMaxViewports);

	if (m_State.Viewports[slot] != viewport)
	{
		m_State.Viewports[slot] = viewport;

		m_State.ViewportsInUse |= (1U << slot);

		m_State.SetDirty(D3D11ContextState::eDirtyViewport, true);
	}
}

void D3D11Context::SetScissorRect(const D3D11Rect &rect, uint32_t slot)
{
	assert(slot < D3D11ContextState::eMaxScissorRects);

	if (m_State.ScissorRects[slot] != rect)
	{
		m_State.ScissorRects[slot] = rect;

		m_State.ScissorRectsInUse |= (1U << slot);

		m_State.SetDirty(D3D11ContextState::eDirtyScissorRect, true);
	}
}

void D3D11Context::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
{
	if (m_State.PrimitiveTopology != primitive)
	{
		m_State.PrimitiveTopology = primitive;

		m_State.SetDirty(D3D11ContextState::eDirtyPrimitiveTopology, true);
	}

	m_State.CommitState(*this);

	m_Object->Draw(vertexCount, startVertex);
}

void D3D11Context::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	if (m_State.PrimitiveTopology != primitive)
	{
		m_State.PrimitiveTopology = primitive;

		m_State.SetDirty(D3D11ContextState::eDirtyPrimitiveTopology, true);
	}

	m_State.CommitState(*this);

	m_Object->DrawIndexed(indexCount, startIndex, offset);
}