#include "D3D11Context.h"

void D3D11ContextState::CommitState(D3D11Context &ctx)
{
	if (!ctx.IsValid())
	{
		return;
	}

	/// Input Assembler
	if (IsDirty(eInputLayout))
	{
		ctx->IASetInputLayout(InputLayout);

		SetDirty(eInputLayout, false);
	}

	if (IsDirty(eVertexBuffer))
	{
		uint32_t vertexStreams = GetCount<uint32_t>(VertexBuffersInUse);

		assert(vertexStreams > 0U);

		ctx->IASetVertexBuffers(0U, vertexStreams, VertexBuffers.data(), VertexStrides.data(), VertexOffsets.data());

		SetDirty(eVertexBuffer, false);
	}

	if (IsDirty(eIndexBuffer))
	{
		ctx->IASetIndexBuffer(IndexBuffer, (DXGI_FORMAT)IndexFormat, IndexOffset);

		SetDirty(eIndexBuffer, false);
	}

	if (IsDirty(ePrimitiveTopology))
	{
		ctx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)PrimitiveTopology);

		SetDirty(ePrimitiveTopology, false);
	}

	BindConstantBuffers(ctx);

	BindSamplerStates(ctx);

	BindShaderResourceViews(ctx);

	BindUnorderedAccessViews(ctx);

	/// VS->HS->DS->GS
	if (IsDirty(eVertexShader))
	{
		assert(VertexShader);

		ctx->VSSetShader(VertexShader, nullptr, 0U);

		SetDirty(eVertexShader, false);
	}

	if (IsDirty(eHullShader))
	{
		ctx->HSSetShader(HullShader, nullptr, 0U);

		SetDirty(eHullShader, false);
	}

	if (IsDirty(eDomainShader))
	{
		ctx->DSSetShader(DomainShader, nullptr, 0U);

		SetDirty(eDomainShader, false);
	}

	if (IsDirty(eGeometryShader))
	{
		ctx->GSSetShader(GeometryShader, nullptr, 0U);

		SetDirty(eGeometryShader, false);
	}

	/// Rasterizer
	if (IsDirty(eRasterizerState))
	{
		ctx->RSSetState(RasterizerState);

		SetDirty(eRasterizerState, false);
	}

	if (IsDirty(eViewport))
	{
		assert(ViewportsInUse > 0U);

		ctx->RSSetViewports(ViewportsInUse, Viewports.data());

		SetDirty(eViewport, false);
	}

	if (IsDirty(eScissorRect))
	{
		ctx->RSSetScissorRects(ScissorRectsInUse, ScissorRects.data());

		SetDirty(eScissorRect, false);
	}

	/// Pixel Shader
	if (IsDirty(ePixelShader))
	{
		ctx->PSSetShader(PixelShader, nullptr, 0U);

		SetDirty(ePixelShader, false);
	}

	if (IsDirty(eComputeShader))
	{
		ctx->CSSetShader(ComputeShader, nullptr, 0U);

		SetDirty(eComputeShader, false);
	}

	/// Output Merge
	if (IsDirty(eBlendState))
	{
		ctx->OMSetBlendState(BlendState, (float *)&BlendFactor, BlendMask);

		SetDirty(eBlendState, false);
	}

	if (IsDirty(eDepthStencilState))
	{
		ctx->OMSetDepthStencilState(DepthStencilState, StencilRef);
		
		SetDirty(eDepthStencilState, false);
	}

	if (IsDirty(eRenderTargetView) || IsDirty(eDepthStencilView))
	{
		uint32_t renderTargets = GetCount<uint8_t>(RenderTargetsInUse);

		ctx->OMSetRenderTargets(renderTargets, RenderTargetViews.data(), DepthStencilView);

		///SetDirty(eRenderTargetView, false);

		SetDirty(eDepthStencilView, false);
	}
}

void D3D11ContextState::BindConstantBuffers(D3D11Context &ctx)
{
	for (uint32_t i = 0U; i < eRShaderTypeCount; ++i)
	{
		if (IsDirty(eConstantBuffer, i))
		{
			uint32_t constantBuffers = GetCount<uint16_t>(ConstantBuffersInUse[i]);

			if (0U == constantBuffers)
			{
				continue;
			}

			switch (i)
			{
			case eVertexShader:   ctx->VSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			case eHullShader:     ctx->HSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			case eDomainShader:   ctx->DSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			case eGeometryShader: ctx->GSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			case ePixelShader:    ctx->PSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			case eComputeShader:  ctx->CSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			}

			SetDirty(eConstantBuffer, false);
		}
	}
}

void D3D11ContextState::BindSamplerStates(D3D11Context &ctx)
{
	for (uint32_t i = 0U; i < eRShaderTypeCount; ++i)
	{
		if (IsDirty(eSamplerState, i))
		{
			uint32_t samplers = GetCount<uint16_t>(SamplersInUse[i]);

			if (0U == samplers)
			{
				continue;
			}

			switch (i)
			{
			case eVertexShader:   ctx->VSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			case eHullShader:     ctx->HSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			case eDomainShader:   ctx->DSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			case eGeometryShader: ctx->GSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			case ePixelShader:    ctx->PSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			case eComputeShader:  ctx->CSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			}
		}

		SetDirty(eSamplerState, false, i);
	}
}

void D3D11ContextState::BindShaderResourceViews(D3D11Context &ctx)
{
	for (uint32_t i = 0U; i < eRShaderTypeCount; ++i)
	{
		if (IsDirty(eShaderResourceView, i))
		{
			uint32_t shaderResourceViews = GetCount<uint8_t>(ShaderResourceViewsInUse[i]);

			if (0U == shaderResourceViews)
			{
				continue;
			}

			switch (i)
			{
			case eVertexShader:   ctx->VSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eHullShader:     ctx->HSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eDomainShader:   ctx->DSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eGeometryShader: ctx->GSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case ePixelShader:    ctx->PSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eComputeShader:  ctx->CSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			}

			SetDirty(eShaderResourceView, false, i);
		}
	}
}

void D3D11ContextState::BindUnorderedAccessViews(D3D11Context &ctx)
{
	if (!IsDirty(eUnorderedAccessView))
	{
		return;
	}

	uint32_t unorderedAccessViews = GetCount<uint8_t>(UnorderedAccessViewsInUse);

	if (unorderedAccessViews > 0U)
	{
		ctx->CSSetUnorderedAccessViews(0U, unorderedAccessViews, UnorderedAccessViews.data(), nullptr);
	}

	SetDirty(eUnorderedAccessView, false);
}
