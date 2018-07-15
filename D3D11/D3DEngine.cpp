#include "D3DEngine.h"
#include "D3DFontFreeType.h"

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

			///D3DFontFreeType::Instance().Initialize(width, height);

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
}

void D3DEngine::Resize(uint32_t width, uint32_t height)
{
	assert((width > 0U) && (height > 0U));

	if (width == m_SwapChain.Width() && height == m_SwapChain.Height())
	{
		return;
	}

	m_RenderTargetView.Reset();

	m_DepthStencilView.Reset();

	//for each(auto rtv in m_IMContextState.RenderTargetViews)
	//{
	//	if (rtv)
	//	{
	//		rtv->Release();
	//	}
	//}

	m_SwapChain.Resize(width, height);

	RecreateRenderTargetDepthStencil(width, height);

	///D3DFontFreeType::Instance().Resize(width, height);
}

void D3DEngine::SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxRenderTargetViews);

	if (renderTarget != m_IMContextState.RenderTargetViews[slot])
	{
		m_IMContextState.RenderTargetViews[slot] = renderTarget.Get();

		m_IMContextState.RenderTargetsInUse |= (1U << slot);

		m_IMContextState.SetDirty(D3DContextState::eRenderTargetView, true);
	}
}

void D3DEngine::SetDepthStencilView(const D3DDepthStencilView &depthStencilView)
{
	if (depthStencilView != m_IMContextState.DepthStencilView)
	{
		m_IMContextState.DepthStencilView = depthStencilView.Get();

		m_IMContextState.SetDirty(D3DContextState::eDepthStencilView, true);
	}
}

void D3DEngine::SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxVertexStream);

	if (m_IMContextState.VertexStrides[slot] != stride ||
		m_IMContextState.VertexOffsets[slot] != offset ||
		vertexBuffer != m_IMContextState.VertexBuffers[slot])
	{
		if (vertexBuffer != m_IMContextState.VertexBuffers[slot])
		{
			m_IMContextState.VertexBuffersInUse |= (1U << slot);
		}

		m_IMContextState.VertexStrides[slot] = stride;
		m_IMContextState.VertexOffsets[slot] = offset;
		m_IMContextState.VertexBuffers[slot] = vertexBuffer.Get();

		m_IMContextState.SetDirty(D3DContextState::eVertexBuffer, true);
	}
}

void D3DEngine::SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
{
	if (m_IMContextState.IndexFormat != fmt ||
		m_IMContextState.IndexOffset != offset ||
		indexBuffer != m_IMContextState.IndexBuffer)
	{
		m_IMContextState.IndexFormat = fmt;
		m_IMContextState.IndexOffset = offset;
		m_IMContextState.IndexBuffer = indexBuffer.Get();

		m_IMContextState.SetDirty(D3DContextState::eIndexBuffer, true);
	}
}

void D3DEngine::SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxShaderResourceViews && targetShader < D3DShader::eShaderTypeCount);

	if (shaderResourceView != m_IMContextState.ShaderResourceViews[targetShader][slot])
	{
		m_IMContextState.ShaderResourceViews[targetShader][slot] = shaderResourceView.Get();

		m_IMContextState.ShaderResourceViewsInUse[targetShader] |= (1 << slot);

		m_IMContextState.SetDirty(D3DContextState::eShaderResourceView, true, targetShader);
	}
}

void D3DEngine::SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxUnorderedAccessViews && targetShader == D3DShader::eComputeShader);

	if (unorderedAccessView != m_IMContextState.UnorderedAccessViews[slot])
	{
		m_IMContextState.UnorderedAccessViews[slot] = unorderedAccessView.Get();

		m_IMContextState.UnorderedAccessViewsInUse |= (1U << slot);

		m_IMContextState.SetDirty(D3DContextState::eUnorderedAccessView, true, targetShader);
	}
}

void D3DEngine::SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxSamplers && targetShader < D3DShader::eShaderTypeCount);

	if (samplerState != m_IMContextState.SamplerStates[targetShader][slot])
	{
		m_IMContextState.SamplerStates[targetShader][slot] = samplerState.Get();

		m_IMContextState.SamplersInUse[targetShader] |= (1U << slot);

		m_IMContextState.SetDirty(D3DContextState::eSamplerState, true, targetShader);
	}
}

void D3DEngine::SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader)
{
	assert(slot < D3DContextState::eMaxConstantBuffers && targetShader < D3DShader::eShaderTypeCount);

	if (constantBuffer != m_IMContextState.ConstantBuffers[targetShader][slot])
	{
		m_IMContextState.ConstantBuffers[targetShader][slot] = constantBuffer.Get();

		m_IMContextState.ConstantBuffersInUse[targetShader] |= (1U << slot);

		m_IMContextState.SetDirty(D3DContextState::eConstantBuffer, true, targetShader);
	}
}

void D3DEngine::SetRasterizerState(const D3DRasterizerState &rasterizerState)
{
	if (rasterizerState != m_IMContextState.RasterizerState)
	{
		m_IMContextState.RasterizerState = rasterizerState.Get();

		m_IMContextState.SetDirty(D3DContextState::eRasterizerState, true);
	}
}

void D3DEngine::SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef)
{
	if (m_IMContextState.StencilRef != stencilRef ||
		depthStencilState != m_IMContextState.DepthStencilState)
	{
		m_IMContextState.StencilRef = stencilRef;
		m_IMContextState.DepthStencilState = depthStencilState.Get();

		m_IMContextState.SetDirty(D3DContextState::eDepthStencilState, true);
	}
}

void D3DEngine::SetBlendState(const D3DBlendState &blendState, Vec4 blendfactor, uint32_t mask)
{
	if (m_IMContextState.BlendMask != mask ||
		m_IMContextState.BlendFactor != blendfactor ||
		blendState != m_IMContextState.BlendState)
	{
		m_IMContextState.BlendMask = mask;
		m_IMContextState.BlendFactor = blendfactor;
		m_IMContextState.BlendState = blendState.Get();

		m_IMContextState.SetDirty(D3DContextState::eBlendState, true);
	}
}

void D3DEngine::SetInputLayout(const D3DInputLayout &inputLayout)
{
	if (inputLayout != m_IMContextState.InputLayout)
	{
		m_IMContextState.InputLayout = inputLayout.Get();

		m_IMContextState.SetDirty(D3DContextState::eInputLayout, true);
	}
}

void D3DEngine::SetVertexShader(const D3DVertexShader &vertexShader)
{
	if (vertexShader != m_IMContextState.VertexShader)
	{
		m_IMContextState.VertexShader = vertexShader.Get();

		m_IMContextState.SetDirty(D3DContextState::eVertexShader, true);
	}
}

void D3DEngine::SetHullShader(const D3DHullShader &hullShader)
{
	if (hullShader != m_IMContextState.HullShader)
	{
		m_IMContextState.HullShader = hullShader.Get();

		m_IMContextState.SetDirty(D3DContextState::eHullShader, true);
	}
}

void D3DEngine::SetDomainShader(const D3DDomainShader &domainShader)
{
	if (domainShader != m_IMContextState.DomainShader)
	{
		m_IMContextState.DomainShader = domainShader.Get();

		m_IMContextState.SetDirty(D3DContextState::eDomainShader, true);
	}
}

void D3DEngine::SetPixelShader(const D3DPixelShader &pixelShader)
{
	if (pixelShader != m_IMContextState.PixelShader)
	{
		m_IMContextState.PixelShader = pixelShader.Get();

		m_IMContextState.SetDirty(D3DContextState::ePixelShader, true);
	}
}

void D3DEngine::SetGeometryShader(const D3DGeometryShader &geometryShader)
{
	if (geometryShader != m_IMContextState.GeometryShader)
	{
		m_IMContextState.GeometryShader = geometryShader.Get();

		m_IMContextState.SetDirty(D3DContextState::eGeometryShader, true);
	}
}

void D3DEngine::SetComputeShader(const D3DComputeShader &computeShader)
{
	if (computeShader != m_IMContextState.ComputeShader)
	{
		m_IMContextState.ComputeShader = computeShader.Get();

		m_IMContextState.SetDirty(D3DContextState::eComputeShader, true);
	}
}

void D3DEngine::SetViewport(const D3DViewport &viewport, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxViewports);

	if (m_IMContextState.Viewports[slot] != viewport)
	{
		m_IMContextState.Viewports[slot] = viewport;

		m_IMContextState.ViewportsInUse |= (1U << slot);

		m_IMContextState.SetDirty(D3DContextState::eViewport, true);
	}
}

void D3DEngine::SetScissorRect(const D3DRect &rect, uint32_t slot)
{
	assert(slot < D3DContextState::eMaxScissorRects);

	if (m_IMContextState.ScissorRects[slot] != rect)
	{
		m_IMContextState.ScissorRects[slot] = rect;

		m_IMContextState.ScissorRectsInUse |= (1U << slot);

		m_IMContextState.SetDirty(D3DContextState::eScissorRect, true);
	}
}

void D3DEngine::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
{
	if (m_IMContextState.PrimitiveTopology != primitive)
	{
		m_IMContextState.PrimitiveTopology = primitive;

		m_IMContextState.SetDirty(D3DContextState::ePrimitiveTopology, true);
	}

	m_IMContextState.CommitState(m_IMContext);

	m_IMContext->Draw(vertexCount, startVertex);
}

void D3DEngine::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
{
	if (m_IMContextState.PrimitiveTopology != primitive)
	{
		m_IMContextState.PrimitiveTopology = primitive;

		m_IMContextState.SetDirty(D3DContextState::ePrimitiveTopology, true);
	}

	m_IMContextState.CommitState(m_IMContext);

	m_IMContext->DrawIndexed(indexCount, startIndex, offset);
}

#if 0
void D3DEngine::DrawTextInPos(const char *pTextContent, uint32_t left, uint32_t top, uint32_t fontSize)
{
	assert(pTextContent);

	D3DFontFreeType::Instance().RenderText(pTextContent, left, top, fontSize);
}
#endif