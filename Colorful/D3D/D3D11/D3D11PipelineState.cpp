#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

void D3D11ContextState::CommitState(D3D11Context &ctx)
{
	if (!ctx.isValid())
	{
		return;
	}

	/// Input Assembler
	if (IsDirty(eDirtyInputLayout))
	{
		ctx->IASetInputLayout(InputLayout);

		SetDirty(eDirtyInputLayout, false);
	}

	if (IsDirty(eDirtyVertexBuffer))
	{
		uint32_t vertexStreams = GetCount<uint32_t>(VertexBuffersInUse);

		assert(vertexStreams > 0U);

		ctx->IASetVertexBuffers(0U, vertexStreams, VertexBuffers.data(), VertexStrides.data(), VertexOffsets.data());

		SetDirty(eDirtyVertexBuffer, false);
	}

	if (IsDirty(eDirtyIndexBuffer))
	{
		ctx->IASetIndexBuffer(IndexBuffer, (DXGI_FORMAT)IndexFormat, IndexOffset);

		SetDirty(eDirtyIndexBuffer, false);
	}

	if (IsDirty(eDirtyPrimitiveTopology))
	{
		ctx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)PrimitiveTopology);

		SetDirty(eDirtyPrimitiveTopology, false);
	}

	BindConstantBuffers(ctx);

	BindSamplerStates(ctx);

	BindShaderResourceViews(ctx);

	BindUnorderedAccessViews(ctx);

	/// VS->HS->DS->GS
	if (IsDirty(eDirtyVertexShader))
	{
		assert(VertexShader);

		ctx->VSSetShader(VertexShader, nullptr, 0U);

		SetDirty(eDirtyVertexShader, false);
	}

	if (IsDirty(eDirtyHullShader))
	{
		ctx->HSSetShader(HullShader, nullptr, 0U);

		SetDirty(eDirtyHullShader, false);
	}

	if (IsDirty(eDirtyDomainShader))
	{
		ctx->DSSetShader(DomainShader, nullptr, 0U);

		SetDirty(eDirtyDomainShader, false);
	}

	if (IsDirty(eDirtyGeometryShader))
	{
		ctx->GSSetShader(GeometryShader, nullptr, 0U);

		SetDirty(eDirtyGeometryShader, false);
	}

	/// Rasterizer
	if (IsDirty(eDirtyRasterizerState))
	{
		ctx->RSSetState(RasterizerState);

		SetDirty(eDirtyRasterizerState, false);
	}

	if (IsDirty(eDirtyViewport))
	{
		assert(ViewportsInUse > 0U);

		ctx->RSSetViewports(ViewportsInUse, Viewports.data());

		SetDirty(eDirtyViewport, false);
	}

	if (IsDirty(eDirtyScissorRect))
	{
		ctx->RSSetScissorRects(ScissorRectsInUse, ScissorRects.data());

		SetDirty(eDirtyScissorRect, false);
	}

	/// Pixel Shader
	if (IsDirty(eDirtyPixelShader))
	{
		ctx->PSSetShader(PixelShader, nullptr, 0U);

		SetDirty(eDirtyPixelShader, false);
	}

	if (IsDirty(eDirtyComputeShader))
	{
		ctx->CSSetShader(ComputeShader, nullptr, 0U);

		SetDirty(eDirtyComputeShader, false);
	}

	/// Output Merge
	if (IsDirty(eDirtyBlendState))
	{
		ctx->OMSetBlendState(BlendState, (float *)&BlendFactor, BlendMask);

		SetDirty(eDirtyBlendState, false);
	}

	if (IsDirty(eDirtyDepthStencilState))
	{
		ctx->OMSetDepthStencilState(DepthStencilState, StencilRef);
		
		SetDirty(eDirtyDepthStencilState, false);
	}

	if (IsDirty(eDirtyRenderTargetView) || IsDirty(eDirtyDepthStencilView))
	{
		uint32_t renderTargets = GetCount<uint8_t>(RenderTargetsInUse);

		ctx->OMSetRenderTargets(renderTargets, RenderTargetViews.data(), DepthStencilView);

		///SetDirty(eRenderTargetView, false);

		SetDirty(eDirtyDepthStencilView, false);
	}
}

void D3D11ContextState::BindConstantBuffers(D3D11Context &ctx)
{
	for (uint32_t i = 0U; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (IsDirty(eDirtyConstantBuffer, i))
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
			case eFragmentShader: ctx->PSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			case eComputeShader:  ctx->CSSetConstantBuffers(0U, constantBuffers, ConstantBuffers[i].data()); break;
			}

			SetDirty(eDirtyConstantBuffer, false);
		}
	}
}

void D3D11ContextState::BindSamplerStates(D3D11Context &ctx)
{
	for (uint32_t i = 0U; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (IsDirty(eDirtySamplerState, i))
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
			case eFragmentShader: ctx->PSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			case eComputeShader:  ctx->CSSetSamplers(0U, samplers, SamplerStates[i].data()); break;
			}
		}

		SetDirty(eDirtySamplerState, false, i);
	}
}

void D3D11ContextState::BindShaderResourceViews(D3D11Context &ctx)
{
	for (uint32_t i = 0U; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (IsDirty(eDirtyShaderResourceView, i))
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
			case eFragmentShader: ctx->PSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eComputeShader:  ctx->CSSetShaderResources(0U, shaderResourceViews, ShaderResourceViews[i].data()); break;
			}

			SetDirty(eDirtyShaderResourceView, false, i);
		}
	}
}

void D3D11ContextState::BindUnorderedAccessViews(D3D11Context &ctx)
{
	if (!IsDirty(eDirtyUnorderedAccessView))
	{
		return;
	}

	uint32_t unorderedAccessViews = GetCount<uint8_t>(UnorderedAccessViewsInUse);

	if (unorderedAccessViews > 0U)
	{
		ctx->CSSetUnorderedAccessViews(0U, unorderedAccessViews, UnorderedAccessViews.data(), nullptr);
	}

	SetDirty(eDirtyUnorderedAccessView, false);
}
