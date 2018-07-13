#include "D3DContext.h"
#include "D3DEngine.h"


void D3DContext::CommitState()
{
	if (!IsValid())
	{
		return;
	}

	/// Input Assembler
	if (State.IsDirty(eDInputLayout))
	{
		m_Object->IASetInputLayout(State.InputLayout);

		State.SetDirty(eDInputLayout, false);
	}

	if (State.IsDirty(eDVertexBuffer))
	{
		assert(!State.VertexBuffers.empty());

		uint32_t vertexBuffers = GetCount<uint32_t>(State.VertexBuffersInUse);

		m_Object->IASetVertexBuffers(0U, vertexBuffers, State.VertexBuffers.data(), State.VertexStrides.data(), State.VertexOffsets.data());

		///State.VertexBuffers.fill(nullptr);
		///State.VertexStrides.fill(0U);
		///State.VertexOffsets.fill(0U);

		State.SetDirty(eDVertexBuffer, false);
	}

	if (State.IsDirty(eDIndexBuffer))
	{
		m_Object->IASetIndexBuffer(State.IndexBuffer, (DXGI_FORMAT)State.IndexFormat, State.IndexOffset);

		State.SetDirty(eDIndexBuffer, false);
	}

	if (State.IsDirty(eDPrimitiveTopology))
	{
		m_Object->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)State.PrimitiveTopology);

		State.SetDirty(eDPrimitiveTopology, false);
	}

	BindConstantBuffers();

	BindSamplerStates();

	BindShaderResourceViews();

	BindUnorderedAccessViews();

	/// VS->HS->DS->GS
	if (State.IsDirty(eDVertexShader))
	{
		assert(State.VertexShader);

		m_Object->VSSetShader(State.VertexShader, nullptr, 0U);

		State.SetDirty(eDVertexShader, false);
	}

	if (State.IsDirty(eDHullShader))
	{
		m_Object->HSSetShader(State.HullShader, nullptr, 0U);

		State.SetDirty(eDHullShader, false);
	}

	if (State.IsDirty(eDDomainShader))
	{
		m_Object->DSSetShader(State.DomainShader, nullptr, 0U);

		State.SetDirty(eDDomainShader, false);
	}

	if (State.IsDirty(eDGeometryShader))
	{
		m_Object->GSSetShader(State.GeometryShader, nullptr, 0U);

		State.SetDirty(eDGeometryShader, false);
	}

	/// Rasterizer
	if (State.IsDirty(eDRasterizerState))
	{
		m_Object->RSSetState(State.RasterizerState);

		State.SetDirty(eDRasterizerState, false);
	}

	if (State.IsDirty(eDViewport))
	{
		assert(!State.Viewports.empty());

		m_Object->RSSetViewports((uint32_t)State.Viewports.size(), State.Viewports.data());

		State.SetDirty(eDViewport, false);
	}

	if (State.IsDirty(eDScissorRect))
	{
		m_Object->RSSetScissorRects((uint32_t)State.ScissorRects.size(), State.ScissorRects.data());

		State.SetDirty(eDScissorRect, false);
	}

	/// Pixel Shader
	if (State.IsDirty(eDPixelShader))
	{
		m_Object->PSSetShader(State.PixelShader, nullptr, 0U);

		State.SetDirty(eDPixelShader, false);
	}

	if (State.IsDirty(eDComputeShader))
	{
		m_Object->CSSetShader(State.ComputeShader, nullptr, 0U);

		State.SetDirty(eDComputeShader, false);
	}

	/// Output Merge
	if (State.IsDirty(eDBlendState))
	{
		m_Object->OMSetBlendState(State.BlendState, (float *)&State.BlendFactor, State.BlendMask);

		State.SetDirty(eDBlendState, false);
	}

	if (State.IsDirty(eDDepthStencilState))
	{
		m_Object->OMSetDepthStencilState(State.DepthStencilState, State.StencilRef);
		
		State.SetDirty(eDDepthStencilState, false);
	}

	if (State.IsDirty(eDRenderTargetView) || State.IsDirty(eDDepthStencilView))
	{
		assert(!State.RenderTargetViews.empty());

		m_Object->OMSetRenderTargets((uint32_t)State.RenderTargetViews.size(), State.RenderTargetViews.data(), State.DepthStencilView);

		///RenderTargetViewCache.clear();

		///DirtyFlags[eDRenderTargetView] = false;
		State.SetDirty(eDDepthStencilView, false);
	}
}

void D3DContext::BindConstantBuffers()
{
	if (!State.IsDirty(eDConstantBuffer))
	{
		return;
	}

	for (uint32_t i = 0U; i < D3DShader::eShaderTypeCount; ++i)
	{
		uint32_t constantBuffers = GetCount<uint16_t>(State.ConstantBuffersInUse[i]);

		if (constantBuffers > 0U)
		{
			switch ((D3DShader::eShaderType)i)
			{
			case D3DShader::eVertexShader:   m_Object->VSSetConstantBuffers(0U, constantBuffers, State.ConstantBuffers[i].data()); break;
			case D3DShader::eHullShader:     m_Object->HSSetConstantBuffers(0U, constantBuffers, State.ConstantBuffers[i].data()); break;
			case D3DShader::eDomainShader:   m_Object->DSSetConstantBuffers(0U, constantBuffers, State.ConstantBuffers[i].data()); break;
			case D3DShader::eGeometryShader: m_Object->GSSetConstantBuffers(0U, constantBuffers, State.ConstantBuffers[i].data()); break;
			case D3DShader::ePixelShader:    m_Object->PSSetConstantBuffers(0U, constantBuffers, State.ConstantBuffers[i].data()); break;
			case D3DShader::eComputeShader:  m_Object->CSSetConstantBuffers(0U, constantBuffers, State.ConstantBuffers[i].data()); break;
			}
		}
	}

	State.SetDirty(eDConstantBuffer, false);
}

void D3DContext::BindSamplerStates()
{
	if (!State.IsDirty(eDSamplerState))
	{
		return;
	}

	for (uint32_t i = 0U; i < D3DShader::eShaderTypeCount; ++i)
	{
		if (!State.SamplerStates[i].empty())
		{
			switch ((D3DShader::eShaderType)i)
			{
			case D3DShader::eVertexShader:   m_Object->VSSetSamplers(0U, (uint32_t)State.SamplerStates[i].size(), State.SamplerStates[i].data()); break;
			case D3DShader::eHullShader:     m_Object->HSSetSamplers(0U, (uint32_t)State.SamplerStates[i].size(), State.SamplerStates[i].data()); break;
			case D3DShader::eDomainShader:   m_Object->DSSetSamplers(0U, (uint32_t)State.SamplerStates[i].size(), State.SamplerStates[i].data()); break;
			case D3DShader::eGeometryShader: m_Object->GSSetSamplers(0U, (uint32_t)State.SamplerStates[i].size(), State.SamplerStates[i].data()); break;
			case D3DShader::ePixelShader:    m_Object->PSSetSamplers(0U, (uint32_t)State.SamplerStates[i].size(), State.SamplerStates[i].data()); break;
			case D3DShader::eComputeShader:  m_Object->CSSetSamplers(0U, (uint32_t)State.SamplerStates[i].size(), State.SamplerStates[i].data()); break;
			}
		}
	}
	
	State.SetDirty(eDSamplerState, false);
}

void D3DContext::BindShaderResourceViews()
{
	if (!State.IsDirty(eDShaderResourceView))
	{
		return;
	}

	for (uint32_t i = 0U; i < D3DShader::eShaderTypeCount; ++i)
	{
		if (!State.ShaderResourceViews[i].empty())
		{
			switch ((D3DShader::eShaderType)i)
			{
			case D3DShader::eVertexShader:   m_Object->VSSetShaderResources(0U, (uint32_t)State.ShaderResourceViews[i].size(), State.ShaderResourceViews[i].data()); break;
			case D3DShader::eHullShader:     m_Object->HSSetShaderResources(0U, (uint32_t)State.ShaderResourceViews[i].size(), State.ShaderResourceViews[i].data()); break;
			case D3DShader::eDomainShader:   m_Object->DSSetShaderResources(0U, (uint32_t)State.ShaderResourceViews[i].size(), State.ShaderResourceViews[i].data()); break;
			case D3DShader::eGeometryShader: m_Object->GSSetShaderResources(0U, (uint32_t)State.ShaderResourceViews[i].size(), State.ShaderResourceViews[i].data()); break;
			case D3DShader::ePixelShader:    m_Object->PSSetShaderResources(0U, (uint32_t)State.ShaderResourceViews[i].size(), State.ShaderResourceViews[i].data()); break;
			case D3DShader::eComputeShader:  m_Object->CSSetShaderResources(0U, (uint32_t)State.ShaderResourceViews[i].size(), State.ShaderResourceViews[i].data()); break;
			}
		}
	}

	State.SetDirty(eDShaderResourceView, false);
}

void D3DContext::BindUnorderedAccessViews()
{
	if (!State.IsDirty(eDUnorderedAccessView))
	{
		return;
	}

	if (!State.UnorderedAccessViews.empty())
	{
		m_Object->CSSetUnorderedAccessViews(0U, (uint32_t)State.UnorderedAccessViews.size(), State.UnorderedAccessViews.data(), nullptr);

		///UnorderedAccessViewCache.clear();
	}

	State.SetDirty(eDUnorderedAccessView, false);
}
