#include "D3DEngine.h"

std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>> D3DEngine::s_Instance;

void D3DEngine::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	assert(hWnd && !m_Inited);

	uint32_t flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::array<D3D_DRIVER_TYPE, 3> driverTypes =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	std::array<D3D_FEATURE_LEVEL, 3> featureLevels = 
	{ 
		D3D_FEATURE_LEVEL_11_0, 
		D3D_FEATURE_LEVEL_10_1, 
		D3D_FEATURE_LEVEL_10_0 
	};

	ID3D11Device *pDevice = nullptr;
	ID3D11DeviceContext *pContext = nullptr;
	for (size_t index = 0; index < driverTypes.size(); ++index)
	{
		if (SUCCEEDED(D3D11CreateDevice(
			nullptr, 
			driverTypes[index], 
			nullptr, 
			flags, 
			&featureLevels[0], 
			(uint32_t)featureLevels.size(), 
			D3D11_SDK_VERSION, 
			&pDevice,
			nullptr, 
			&pContext)))
		{
			m_Device.MakeObject(pDevice);
			m_IMContext.MakeObject(pContext);

			m_SwapChain.Create(hWnd, width, height, bWindowed);

			RecreateRenderTargetDepthStencil(width, height);

			D3DStaticState::Initialize();

			m_Inited = true;

			return;
		}
	}

	assert(!"Failed to initialize D3D device!!!");
}

void D3DEngine::RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height)
{
	assert(m_SwapChain.IsValid());

	ID3D11Texture2D *pTexture2D = nullptr;
	HRCheck(m_SwapChain->GetBuffer(0U, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pTexture2D)));

	D3DResource surface;
	surface.MakeObject(pTexture2D);
	m_RenderTargetView.Create(surface);

	m_DepthStencilView.Create(eD24_UNorm_S8_UInt, width, height);

	SetRenderTargetView(m_RenderTargetView);

	SetDepthStencilView(m_DepthStencilView);

	D3DViewport viewport(0.0f, 0.0f, (float)width, (float)height);
	SetViewport(viewport);
}

void D3DEngine::Resize(uint32_t width, uint32_t height)
{
	assert((width > 0U) && (height > 0U));

	if (width == m_SwapChain.Width() && height == m_SwapChain.Height())
	{
		return;
	}

	for (uint32_t i = 0U; i < eMaxRenderTargetViews; ++i)
	{
		m_Pipeline.RenderTargetViews[i].Reset();
	}

	m_RenderTargetView.Reset();
	m_DepthStencilView.Reset();

	m_SwapChain.Resize(width, height);

	RecreateRenderTargetDepthStencil(width, height);

	D3DViewport viewport(0.0f, 0.0f, (float)width, (float)height);
	SetViewport(viewport);
}

void D3DEngine::SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot)
{
	assert(slot < eMaxRenderTargetViews);

	if (m_Pipeline.RenderTargetViews[slot] != renderTarget)
	{
		m_Pipeline.RenderTargetViews[slot] = renderTarget;

		///m_Pipeline.RenderTargetViewCache.push_back(renderTarget.Get());
		m_Pipeline.RenderTargetViewCache.assign(1U, renderTarget.Get());

		m_Pipeline.DirtyFlags[eDRenderTargetView] = true;
	}
}

void D3DEngine::SetDepthStencilView(const D3DDepthStencilView &depthStencilView)
{
	if (m_Pipeline.DepthStencilView != depthStencilView)
	{
		m_Pipeline.DepthStencilView = depthStencilView;

		m_Pipeline.DirtyFlags[eDDepthStencilView] = true;
	}
}

void D3DEngine::SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < eMaxShaderResourceView && targetShader < D3DShader::eShaderTypeCount);

	if (m_Pipeline.ShaderResourceViews[targetShader][slot] != shaderResourceView)
	{
		m_Pipeline.ShaderResourceViews[targetShader][slot] = shaderResourceView;

		m_Pipeline.ShaderResourceViewCache[targetShader].push_back(shaderResourceView.Get());

		m_Pipeline.DirtyFlags[eDShaderResourceView] = true;
	}
}

void D3DEngine::SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < eMaxUnorderedAccessViews && targetShader == D3DShader::eComputeShader);
	
	if (m_Pipeline.UnorderedAccessViews[slot] != unorderedAccessView)
	{
		m_Pipeline.UnorderedAccessViews[slot] = unorderedAccessView;

		m_Pipeline.UnorderedAccessViewCache.push_back(unorderedAccessView.Get());

		m_Pipeline.DirtyFlags[eDUnorderedAccessView] = true;
	}
}

void D3DEngine::SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
{
	assert(slot < eMaxVertexStream);

	if (m_Pipeline.VertexBuffers[slot].Stride != stride ||
		m_Pipeline.VertexBuffers[slot].Offset != offset ||
		m_Pipeline.VertexBuffers[slot].Buffer != vertexBuffer)
	{
		m_Pipeline.VertexBuffers[slot].Stride = stride;
		m_Pipeline.VertexBuffers[slot].Offset = offset;
		m_Pipeline.VertexBuffers[slot].Buffer = vertexBuffer;

		m_Pipeline.VertexBufferCache.push_back(vertexBuffer.Get());
		m_Pipeline.VertexStrideCache.push_back(stride);
		m_Pipeline.VertexOffsetCache.push_back(offset);

		m_Pipeline.DirtyFlags[eDVertexBuffer] = true;
	}
}

void D3DEngine::SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
{
	if (m_Pipeline.IndexBuffer.Format != fmt ||
		m_Pipeline.IndexBuffer.Offset != offset ||
		m_Pipeline.IndexBuffer.Buffer != indexBuffer)
	{
		m_Pipeline.IndexBuffer.Format = fmt;
		m_Pipeline.IndexBuffer.Offset = offset;
		m_Pipeline.IndexBuffer.Buffer = indexBuffer;

		m_Pipeline.DirtyFlags[eDIndexBuffer] = true;
	}
}

void D3DEngine::SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < eMaxSamplers && targetShader < D3DShader::eShaderTypeCount);

	if (m_Pipeline.SamplerStates[targetShader][slot] != samplerState)
	{
		m_Pipeline.SamplerStates[targetShader][slot] = samplerState;

		m_Pipeline.SamplerStateCache[targetShader].push_back(samplerState.Get());

		m_Pipeline.DirtyFlags[eDSamplerState] = true;
	}
}

void D3DEngine::SetRasterizerState(const D3DRasterizerState &rasterizerState)
{
	if (m_Pipeline.RasterizerState != rasterizerState)
	{
		m_Pipeline.RasterizerState = rasterizerState;

		m_Pipeline.DirtyFlags[eDRasterizerState] = true;
	}
}

void D3DEngine::SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef)
{
	if (m_Pipeline.StencilRef != stencilRef ||
		m_Pipeline.DepthStencilState != depthStencilState)
	{
		m_Pipeline.StencilRef = stencilRef;
		m_Pipeline.DepthStencilState = depthStencilState;

		m_Pipeline.DirtyFlags[eDDepthStencilState] = true;
	}
}

void D3DEngine::SetBlendState(const D3DBlendState &blendState, const float *pFactor, uint32_t mask)
{
	if (m_Pipeline.BlendState != blendState)
	{
		m_Pipeline.BlendMask = mask;
		
		if (pFactor)
		{
			memcpy(&m_Pipeline.BlendFactor, pFactor, sizeof(float) * 4);
		}

		m_Pipeline.BlendState = blendState;

		m_Pipeline.DirtyFlags[eDBlendState] = true;
	}
}

void D3DEngine::SetInputLayout(const D3DInputLayout &inputLayout)
{
	if (m_Pipeline.InputLayout != inputLayout)
	{
		m_Pipeline.InputLayout = inputLayout;

		m_Pipeline.DirtyFlags[eDInputLayout] = true;
	}
}

void D3DEngine::SetVertexShader(const D3DVertexShader &vertexShader)
{
	if (m_Pipeline.VertexShader != vertexShader)
	{
		m_Pipeline.VertexShader = vertexShader;

		m_Pipeline.DirtyFlags[eDVertexShader] = true;
	}
}

void D3DEngine::SetHullShader(const D3DHullShader &hullShader)
{
	if (m_Pipeline.HullShader != hullShader)
	{
		m_Pipeline.HullShader = hullShader;

		m_Pipeline.DirtyFlags[eDHullShader] = true;
	}
}

void D3DEngine::SetDomainShader(const D3DDomainShader &domainShader)
{
	if (m_Pipeline.DomainShader != domainShader)
	{
		m_Pipeline.DomainShader = domainShader;

		m_Pipeline.DirtyFlags[eDDomainShader] = true;
	}
}

void D3DEngine::SetPixelShader(const D3DPixelShader &pixelShader)
{
	if (m_Pipeline.PixelShader != pixelShader)
	{
		m_Pipeline.PixelShader = pixelShader;

		m_Pipeline.DirtyFlags[eDPixelShader] = true;
	}
}

void D3DEngine::SetGeometryShader(const D3DGeometryShader &geometryShader)
{
	if (m_Pipeline.GeometryShader != geometryShader)
	{
		m_Pipeline.GeometryShader = geometryShader;

		m_Pipeline.DirtyFlags[eDGeometryShader] = true;
	}
}

void D3DEngine::SetComputeShader(const D3DComputeShader &computeShader)
{
	if (m_Pipeline.ComputeShader != computeShader)
	{
		m_Pipeline.ComputeShader = computeShader;

		m_Pipeline.DirtyFlags[eDComputeShader] = true;
	}
}

void D3DEngine::SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < eMaxConstantBuffers && targetShader < D3DShader::eShaderTypeCount);

	if (m_Pipeline.ConstantBuffers[targetShader][slot] != constantBuffer)
	{
		m_Pipeline.ConstantBuffers[targetShader][slot] = constantBuffer;

		m_Pipeline.ConstantBufferCache[targetShader].push_back(constantBuffer.Get());

		m_Pipeline.DirtyFlags[eDConstantBuffer] = true;
	}
}

void D3DEngine::SetViewport(const D3DViewport &viewport, uint32_t slot)
{
	assert(slot < eMaxViewports);

	if (m_Pipeline.Viewports[slot] != viewport)
	{
		m_Pipeline.Viewports[slot] = viewport;

		m_Pipeline.ViewportCache.push_back(viewport);

		m_Pipeline.DirtyFlags[eDViewport] = true;
	}
}

void D3DEngine::SetScissorRect(const D3DRect &rect, uint32_t slot)
{
	assert(slot < eMaxScissorRects);

	if (m_Pipeline.ScissorRects[slot] != rect)
	{
		m_Pipeline.ScissorRects[slot] = rect;

		m_Pipeline.ScissorRectCache.push_back(rect);

		m_Pipeline.DirtyFlags[eDScissorRect] = true;
	}
}

void D3DEngine::D3DPipeline::CommitState(const D3DContext &IMContext)
{
	if (!IMContext.IsValid())
	{
		return;
	}

	/// Input Assembler
	if (DirtyFlags[eDInputLayout])
	{
		IMContext->IASetInputLayout(InputLayout.Get());
		DirtyFlags[eDInputLayout] = false;
	}

	if (DirtyFlags[eDVertexBuffer])
	{
		assert(!VertexBufferCache.empty());

		IMContext->IASetVertexBuffers(0U, (uint32_t)VertexBufferCache.size(), VertexBufferCache.data(), VertexStrideCache.data(), VertexOffsetCache.data());

		VertexBufferCache.clear();
		VertexStrideCache.clear();
		VertexOffsetCache.clear();

		DirtyFlags[eDVertexBuffer] = false;
	}

	if (DirtyFlags[eDIndexBuffer])
	{
		IMContext->IASetIndexBuffer(IndexBuffer.Buffer.Get(), (DXGI_FORMAT)IndexBuffer.Format, IndexBuffer.Offset);
		DirtyFlags[eDIndexBuffer] = false;
	}

	if (DirtyFlags[eDPrimitiveTopology])
	{
		IMContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)PrimitiveTopology);
		DirtyFlags[eDPrimitiveTopology] = false;
	}

	BindConstantBuffers(IMContext);

	BindSamplerStates(IMContext);

	BindShaderResourceViews(IMContext);

	BindUnorderedAccessViews(IMContext);

	/// VS->HS->DS->GS
	if (DirtyFlags[eDVertexShader])
	{
		assert(VertexShader.IsValid());
		IMContext->VSSetShader(VertexShader.Get(), nullptr, 0U);
		DirtyFlags[eDVertexShader] = false;
	}

	if (DirtyFlags[eDHullShader])
	{
		IMContext->HSSetShader(HullShader.Get(), nullptr, 0U);
		DirtyFlags[eDHullShader] = false;
	}

	if (DirtyFlags[eDDomainShader])
	{
		IMContext->DSSetShader(DomainShader.Get(), nullptr, 0U);
		DirtyFlags[eDDomainShader] = false;
	}

	if (DirtyFlags[eDGeometryShader])
	{
		IMContext->GSSetShader(GeometryShader.Get(), nullptr, 0U);
		DirtyFlags[eDGeometryShader] = false;
	}

	/// Rasterizer
	if (DirtyFlags[eDRasterizerState])
	{
		IMContext->RSSetState(RasterizerState.Get());
		DirtyFlags[eDRasterizerState] = false;
	}

	if (DirtyFlags[eDViewport])
	{
		assert(!ViewportCache.empty());

		IMContext->RSSetViewports((uint32_t)ViewportCache.size(), ViewportCache.data());

		ViewportCache.clear();

		DirtyFlags[eDViewport] = false;
	}

	if (DirtyFlags[eDScissorRect])
	{
		if (!ScissorRectCache.empty())
		{
			IMContext->RSSetScissorRects((uint32_t)ScissorRectCache.size(), ScissorRectCache.data());

			ScissorRectCache.clear();
		}

		DirtyFlags[eDScissorRect] = false;
	}

	/// Pixel Shader
	if (DirtyFlags[eDPixelShader])
	{
		IMContext->PSSetShader(PixelShader.Get(), nullptr, 0U);
		DirtyFlags[eDPixelShader] = false;
	}

	if (DirtyFlags[eDComputeShader])
	{
		IMContext->CSSetShader(ComputeShader.Get(), nullptr, 0U);
		DirtyFlags[eDComputeShader] = false;
	}

	/// Output Merge
	if (DirtyFlags[eDBlendState])
	{
		IMContext->OMSetBlendState(BlendState.Get(), BlendFactor, BlendMask);
		DirtyFlags[eDBlendState] = false;
	}

	if (DirtyFlags[eDDepthStencilState])
	{
		IMContext->OMSetDepthStencilState(DepthStencilState.Get(), StencilRef);
		DirtyFlags[eDDepthStencilState] = false;
	}

	if (DirtyFlags[eDRenderTargetView] || DirtyFlags[eDDepthStencilView])
	{
		assert(!RenderTargetViewCache.empty());

		IMContext->OMSetRenderTargets((uint32_t)RenderTargetViewCache.size(), RenderTargetViewCache.data(), DepthStencilView.Get());

		///RenderTargetViewCache.clear();

		///DirtyFlags[eDRenderTargetView] = false;
		DirtyFlags[eDDepthStencilView] = false;
	}
}

void D3DEngine::D3DPipeline::BindConstantBuffers(const D3DContext &IMContext)
{
	if (!DirtyFlags[eDConstantBuffer])
	{
		return;
	}

	for (uint32_t i = 0U; i < D3DShader::eShaderTypeCount; ++i)
	{
		std::vector<ID3D11Buffer *> &constantBuffers = ConstantBufferCache[i];
		if (!constantBuffers.empty())
		{
			switch ((D3DShader::eShaderType)i)
			{
			case D3DShader::eVertexShader:   IMContext->VSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
			case D3DShader::eHullShader:     IMContext->HSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
			case D3DShader::eDomainShader:   IMContext->DSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
			case D3DShader::eGeometryShader: IMContext->GSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
			case D3DShader::ePixelShader:    IMContext->PSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
			case D3DShader::eComputeShader:  IMContext->CSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
			}

			constantBuffers.clear();
		}
	}

	DirtyFlags[eDConstantBuffer] = false;
}

void D3DEngine::D3DPipeline::BindSamplerStates(const D3DContext &IMContext)
{
	if (!DirtyFlags[eDSamplerState])
	{
		return;
	}

	for (uint32_t i = 0U; i < D3DShader::eShaderTypeCount; ++i)
	{
		std::vector<ID3D11SamplerState *> &samplerStates = SamplerStateCache[i];
		if (!samplerStates.empty())
		{
			switch ((D3DShader::eShaderType)i)
			{
			case D3DShader::eVertexShader:   IMContext->VSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
			case D3DShader::eHullShader:     IMContext->HSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
			case D3DShader::eDomainShader:   IMContext->DSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
			case D3DShader::eGeometryShader: IMContext->GSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
			case D3DShader::ePixelShader:    IMContext->PSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
			case D3DShader::eComputeShader:  IMContext->CSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
			}

			samplerStates.clear();
		}
	}

	DirtyFlags[eDSamplerState] = false;
}

void D3DEngine::D3DPipeline::BindShaderResourceViews(const D3DContext &IMContext)
{
	if (!DirtyFlags[eDShaderResourceView])
	{
		return;
	}

	for (uint32_t i = 0U; i < D3DShader::eShaderTypeCount; ++i)
	{
		std::vector<ID3D11ShaderResourceView *> &shaderResourceViews = ShaderResourceViewCache[i];
		if (!shaderResourceViews.empty())
		{
			switch ((D3DShader::eShaderType)i)
			{
			case D3DShader::eVertexShader:   IMContext->VSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
			case D3DShader::eHullShader:     IMContext->HSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
			case D3DShader::eDomainShader:   IMContext->DSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
			case D3DShader::eGeometryShader: IMContext->GSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
			case D3DShader::ePixelShader:    IMContext->PSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
			case D3DShader::eComputeShader:  IMContext->CSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
			}

			shaderResourceViews.clear();
		}
	}

	DirtyFlags[eDShaderResourceView] = false;
}

void D3DEngine::D3DPipeline::BindUnorderedAccessViews(const D3DContext &IMContext)
{
	if (!DirtyFlags[eDUnorderedAccessView])
	{
		return;
	}

	if (!UnorderedAccessViewCache.empty())
	{
		IMContext->CSSetUnorderedAccessViews(0U, (uint32_t)UnorderedAccessViewCache.size(), UnorderedAccessViewCache.data(), nullptr);

		UnorderedAccessViewCache.clear();
	}

	DirtyFlags[eDUnorderedAccessView] = false;
}

void D3DEngine::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
{
	if (m_Pipeline.PrimitiveTopology != primitive)
	{
		m_Pipeline.PrimitiveTopology = primitive;
		m_Pipeline.DirtyFlags[eDPrimitiveTopology] = true;
	}

	m_Pipeline.CommitState(m_IMContext);

	m_IMContext->Draw(vertexCount, startVertex);
}

void D3DEngine::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	if (m_Pipeline.PrimitiveTopology != primitive)
	{
		m_Pipeline.PrimitiveTopology = primitive;
		m_Pipeline.DirtyFlags[eDPrimitiveTopology] = true;
	}

	m_Pipeline.CommitState(m_IMContext);

	m_IMContext->DrawIndexed(indexCount, startIndex, offset);
}