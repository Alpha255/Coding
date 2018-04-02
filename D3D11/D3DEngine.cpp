#include "D3DEngine.h"

std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>> D3DEngine::s_Instance;

void D3DEngine::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	assert(hWnd);

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
	m_RenderTarget.Create(surface);

	D3DResource empty;
	m_DepthStencil.Create(empty, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, D3D11_DSV_DIMENSION_TEXTURE2D);

	SetRenderTargetView(m_RenderTarget);

	SetDepthStencilView(m_DepthStencil);
}

void D3DEngine::Resize(uint32_t width, uint32_t height)
{
	assert((width > 0U) && (height > 0U));

	m_RenderTarget.Reset();
	m_DepthStencil.Reset();

	if (!m_SwapChain.Resize(width, height))
	{
		return;
	}

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

		size_t index = m_Pipeline.RenderTargetViewCache.size();
		m_Pipeline.RenderTargetViewCache[index] = renderTarget.Get();

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

		size_t index = m_Pipeline.ShaderResourceViewCache[targetShader].size();
		m_Pipeline.ShaderResourceViewCache[targetShader][index] = shaderResourceView.Get();

		m_Pipeline.DirtyFlags[eDShaderResourceView] = true;
	}
}

void D3DEngine::SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < eMaxUnorderedAccessViews && targetShader == D3DShader::eComputeShader);
	
	if (m_Pipeline.UnorderedAccessViews[slot] != unorderedAccessView)
	{
		m_Pipeline.UnorderedAccessViews[slot] = unorderedAccessView;

		size_t index = m_Pipeline.UnorderedAccessViewCache.size();
		m_Pipeline.UnorderedAccessViewCache[index] = unorderedAccessView.Get();

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

		size_t index = m_Pipeline.VertexBufferCache.size();

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

void D3DEngine::SetSamplerStates(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < eMaxSamplers && targetShader < D3DShader::eShaderTypeCount);

	if (m_Pipeline.SamplerStates[targetShader][slot] != samplerState)
	{
		m_Pipeline.SamplerStates[targetShader][slot] = samplerState;

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
		memcpy(&m_Pipeline.BlendFactor, pFactor, sizeof(float) * 4);
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

		m_Pipeline.DirtyFlags[eDVertexShader] = true;
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

		m_Pipeline.DirtyFlags[eDConstantBuffer] = true;
	}
}

void D3DEngine::SetViewport(const D3DViewport &viewport, uint32_t slot)
{
	assert(slot < eMaxViewports);

	if (m_Pipeline.Viewports[slot] != viewport)
	{
		m_Pipeline.Viewports[slot] = viewport;

		m_Pipeline.DirtyFlags[eDViewport] = true;
	}
}

void D3DEngine::SetScissorRect(const D3DRect &rect, uint32_t slot)
{
	assert(slot < eMaxScissorRects);

	if (m_Pipeline.ScissorRects[slot] != rect)
	{
		m_Pipeline.ScissorRects[slot] = rect;

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
		std::array<ID3D11Buffer *, eMaxVertexStream> vertexBuffers;
		std::array<uint32_t, eMaxVertexStream> strides;
		std::array<uint32_t, eMaxVertexStream> offsets;

		uint32_t index = 0U;
		for (uint32_t i = 0U; i < eMaxVertexStream; ++i)
		{
			if (VertexBuffers[i].Buffer.IsValid())
			{
				vertexBuffers[index] = VertexBuffers[i].Buffer.Get();
				strides[index] = VertexBuffers[i].Stride;
				offsets[index] = VertexBuffers[i].Offset;
				++index;
			}
		}

		assert(vertexBuffers.empty());

		IMContext->IASetVertexBuffers(0, (uint32_t)vertexBuffers.size(), vertexBuffers.data(), strides.data(), offsets.data());
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

	BindViewports(IMContext);

	BindScirrorRects(IMContext);

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
		uint32_t index = 0U;
		std::array<ID3D11RenderTargetView *, eMaxRenderTargetViews> renderTargetViews;
		for (uint32_t i = 0U; i < eMaxRenderTargetViews; ++i)
		{
			if (RenderTargetViews[i].IsValid())
			{
				renderTargetViews[index++] = RenderTargetViews[i].Get();
			}
		}

		assert(!renderTargetViews.empty());

		IMContext->OMSetRenderTargets((uint32_t)renderTargetViews.size(), renderTargetViews.data(), DepthStencilView.Get());
		
		DirtyFlags[eDRenderTargetView] = false;
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
		uint32_t index = 0U;
		std::array<ID3D11Buffer *, eMaxConstantBuffers> constantBuffers;
		for (uint32_t j = 0U; j < eMaxConstantBuffers; ++j)
		{
			if (ConstantBuffers[i][j].IsValid())
			{
				constantBuffers[index++] = ConstantBuffers[i][j].Get();
			}
		}

		switch ((D3DShader::eShaderType)i)
		{
		case D3DShader::eVertexShader:   IMContext->VSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
		case D3DShader::eHullShader:     IMContext->HSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
		case D3DShader::eDomainShader:   IMContext->DSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
		case D3DShader::eGeometryShader: IMContext->GSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
		case D3DShader::ePixelShader:    IMContext->PSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
		case D3DShader::eComputeShader:  IMContext->CSSetConstantBuffers(0U, (uint32_t)constantBuffers.size(), constantBuffers.data()); break;
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
		uint32_t index = 0U;
		std::array<ID3D11SamplerState *, eMaxSamplers> samplerStates;
		for (uint32_t j = 0U; j < eMaxSamplers; ++j)
		{
			if (SamplerStates[i][j].IsValid())
			{
				samplerStates[index++] = SamplerStates[i][j].Get();
			}
		}

		switch ((D3DShader::eShaderType)i)
		{
		case D3DShader::eVertexShader:   IMContext->VSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
		case D3DShader::eHullShader:     IMContext->HSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
		case D3DShader::eDomainShader:   IMContext->DSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
		case D3DShader::eGeometryShader: IMContext->GSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
		case D3DShader::ePixelShader:    IMContext->PSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
		case D3DShader::eComputeShader:  IMContext->CSSetSamplers(0U, (uint32_t)samplerStates.size(), samplerStates.data()); break;
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
		uint32_t index = 0U;
		std::array<ID3D11ShaderResourceView *, eMaxShaderResourceView> shaderResourceViews;
		for (uint32_t j = 0U; j < eMaxShaderResourceView; ++j)
		{
			if (ShaderResourceViews[i][j].IsValid())
			{
				shaderResourceViews[index++] = ShaderResourceViews[i][j].Get();
			}
		}

		switch ((D3DShader::eShaderType)i)
		{
		case D3DShader::eVertexShader:   IMContext->VSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
		case D3DShader::eHullShader:     IMContext->HSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
		case D3DShader::eDomainShader:   IMContext->DSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
		case D3DShader::eGeometryShader: IMContext->GSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
		case D3DShader::ePixelShader:    IMContext->PSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
		case D3DShader::eComputeShader:  IMContext->CSSetShaderResources(0U, (uint32_t)shaderResourceViews.size(), shaderResourceViews.data()); break;
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

	uint32_t index = 0U;
	std::array<ID3D11UnorderedAccessView *, eMaxUnorderedAccessViews> unorderedAccessViews;
	for (uint32_t i = 0U; i < eMaxUnorderedAccessViews; ++i)
	{
		if (UnorderedAccessViews[i].IsValid())
		{
			unorderedAccessViews[index++] = UnorderedAccessViews[i].Get();
		}
	}

	IMContext->CSSetUnorderedAccessViews(0U, (uint32_t)unorderedAccessViews.size(), unorderedAccessViews.data(), nullptr);

	DirtyFlags[eDUnorderedAccessView] = false;
}

void D3DEngine::D3DPipeline::BindViewports(const D3DContext &IMContext)
{
	if (!DirtyFlags[eDViewport])
	{
		return;
	}

	DirtyFlags[eDViewport] = false;
}

void D3DEngine::D3DPipeline::BindScirrorRects(const D3DContext &IMContext)
{
	if (!DirtyFlags[eDScissorRect])
	{
		return;
	}

	DirtyFlags[eDScissorRect] = false;
}