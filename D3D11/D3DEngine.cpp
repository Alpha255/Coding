#include "D3DEngine.h"
#include "D3DFontFreeType.h"

#include <thread>

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

	const uint32_t maxContexts = std::thread::hardware_concurrency() + 1U;
	m_ContextsInUse.resize(maxContexts);

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
			m_ContextsInUse[0].MakeObject(pContext);

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

	///for (uint32_t i = 0U; i < D3DContext::eMaxRenderTargetViews; ++i)
	///{
		///m_ContextsInUse[ctxIndex].State.RenderTargetViews[i].Reset();
	///}

	m_RenderTargetView.Reset();
	m_DepthStencilView.Reset();

	m_SwapChain.Resize(width, height);

	RecreateRenderTargetDepthStencil(width, height);

	D3DViewport viewport(0.0f, 0.0f, (float)width, (float)height);
	SetViewport(viewport);

	///D3DFontFreeType::Instance().Resize(width, height);
}

void D3DEngine::CreateDeferredContexts(uint32_t count)
{
	assert(count <= m_ContextsInUse.size() - 1U);

	for (uint32_t i = 1U; i <= count; ++i)
	{
		assert(!m_ContextsInUse[i].IsValid());

		ID3D11DeviceContext *pContext = nullptr;
		HRCheck(D3DEngine::Instance().GetDevice().Get()->CreateDeferredContext(0U, &pContext));
		m_ContextsInUse[i].MakeObject(pContext);
	}
}

void D3DEngine::SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxRenderTargetViews && m_ContextsInUse[ctxIndex].IsValid());

	if (renderTarget != m_ContextsInUse[ctxIndex].State.RenderTargetViews[slot])
	{
		m_ContextsInUse[ctxIndex].State.RenderTargetViews[slot] = renderTarget.Get();

		m_ContextsInUse[ctxIndex].State.RenderTargetsInUse |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDRenderTargetView, true);
	}
}

void D3DEngine::SetDepthStencilView(const D3DDepthStencilView &depthStencilView, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (depthStencilView != m_ContextsInUse[ctxIndex].State.DepthStencilView)
	{
		m_ContextsInUse[ctxIndex].State.DepthStencilView = depthStencilView.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDDepthStencilView, true);
	}
}

void D3DEngine::SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxShaderResourceView && targetShader < D3DShader::eShaderTypeCount && m_ContextsInUse[ctxIndex].IsValid());

	if (shaderResourceView != m_ContextsInUse[ctxIndex].State.ShaderResourceViews[targetShader][slot])
	{
		m_ContextsInUse[ctxIndex].State.ShaderResourceViews[targetShader][slot] = shaderResourceView.Get();

		m_ContextsInUse[ctxIndex].State.ShaderResourceViewsInUse[targetShader] |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDShaderResourceView, true);
	}
}

void D3DEngine::SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxUnorderedAccessViews && m_ContextsInUse[ctxIndex].IsValid());
	
	if (unorderedAccessView != m_ContextsInUse[ctxIndex].State.UnorderedAccessViews[slot])
	{
		m_ContextsInUse[ctxIndex].State.UnorderedAccessViews[slot] = unorderedAccessView.Get();

		m_ContextsInUse[ctxIndex].State.UnorderedAccessViewsInUse |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDUnorderedAccessView, true);
	}
}

void D3DEngine::SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxVertexStream && m_ContextsInUse[ctxIndex].IsValid());

	if (stride != m_ContextsInUse[ctxIndex].State.VertexStrides[slot] ||
		offset != m_ContextsInUse[ctxIndex].State.VertexOffsets[slot] ||
		vertexBuffer != m_ContextsInUse[ctxIndex].State.VertexBuffers[slot])
	{
		m_ContextsInUse[ctxIndex].State.VertexStrides[slot] = stride;
		m_ContextsInUse[ctxIndex].State.VertexOffsets[slot] = offset;
		m_ContextsInUse[ctxIndex].State.VertexBuffers[slot] = vertexBuffer.Get();

		m_ContextsInUse[ctxIndex].State.VertexBuffersInUse |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDVertexBuffer, true);
	}
}

void D3DEngine::SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset, uint32_t ctxIndex)
{
	if (fmt != m_ContextsInUse[ctxIndex].State.IndexFormat ||
		offset != m_ContextsInUse[ctxIndex].State.IndexOffset ||
		indexBuffer != m_ContextsInUse[ctxIndex].State.IndexBuffer)
	{
		m_ContextsInUse[ctxIndex].State.IndexFormat = fmt;
		m_ContextsInUse[ctxIndex].State.IndexOffset = offset;
		m_ContextsInUse[ctxIndex].State.IndexBuffer = indexBuffer.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDIndexBuffer, true);
	}
}

void D3DEngine::SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxSamplers && targetShader < D3DShader::eShaderTypeCount && m_ContextsInUse[ctxIndex].IsValid());

	if (samplerState != m_ContextsInUse[ctxIndex].State.SamplerStates[targetShader][slot])
	{
		m_ContextsInUse[ctxIndex].State.SamplerStates[targetShader][slot] = samplerState.Get();

		m_ContextsInUse[ctxIndex].State.SamplersInUse[targetShader] |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDSamplerState, true);
	}
}

void D3DEngine::SetRasterizerState(const D3DRasterizerState &rasterizerState, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (rasterizerState != m_ContextsInUse[ctxIndex].State.RasterizerState)
	{
		m_ContextsInUse[ctxIndex].State.RasterizerState = rasterizerState.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDRasterizerState, true);
	}
}

void D3DEngine::SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (stencilRef != m_ContextsInUse[ctxIndex].State.StencilRef || 
		depthStencilState != m_ContextsInUse[ctxIndex].State.DepthStencilState)
	{
		m_ContextsInUse[ctxIndex].State.StencilRef = stencilRef;
		m_ContextsInUse[ctxIndex].State.DepthStencilState = depthStencilState.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDDepthStencilState, true);
	}
}

void D3DEngine::SetBlendState(const D3DBlendState &blendState, Vec4 blendfactor, uint32_t mask, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (mask != m_ContextsInUse[ctxIndex].State.BlendMask ||
		blendfactor != m_ContextsInUse[ctxIndex].State.BlendFactor ||
		blendState != m_ContextsInUse[ctxIndex].State.BlendState)
	{
		m_ContextsInUse[ctxIndex].State.BlendMask = mask;
		m_ContextsInUse[ctxIndex].State.BlendFactor = blendfactor;
		m_ContextsInUse[ctxIndex].State.BlendState = blendState.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDBlendState, true);
	}
}

void D3DEngine::SetInputLayout(const D3DInputLayout &inputLayout, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (inputLayout != m_ContextsInUse[ctxIndex].State.InputLayout)
	{
		m_ContextsInUse[ctxIndex].State.InputLayout = inputLayout.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDInputLayout, true);
	}
}

void D3DEngine::SetVertexShader(const D3DVertexShader &vertexShader, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (vertexShader != m_ContextsInUse[ctxIndex].State.VertexShader)
	{
		m_ContextsInUse[ctxIndex].State.VertexShader = vertexShader.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDVertexShader, true);
	}
}

void D3DEngine::SetHullShader(const D3DHullShader &hullShader, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (hullShader != m_ContextsInUse[ctxIndex].State.HullShader)
	{
		m_ContextsInUse[ctxIndex].State.HullShader = hullShader.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDHullShader, true);
	}
}

void D3DEngine::SetDomainShader(const D3DDomainShader &domainShader, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (domainShader != m_ContextsInUse[ctxIndex].State.DomainShader)
	{
		m_ContextsInUse[ctxIndex].State.DomainShader = domainShader.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDDomainShader, true);
	}
}

void D3DEngine::SetPixelShader(const D3DPixelShader &pixelShader, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (pixelShader != m_ContextsInUse[ctxIndex].State.PixelShader)
	{
		m_ContextsInUse[ctxIndex].State.PixelShader = pixelShader.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDPixelShader, true);
	}
}

void D3DEngine::SetGeometryShader(const D3DGeometryShader &geometryShader, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (geometryShader != m_ContextsInUse[ctxIndex].State.GeometryShader)
	{
		m_ContextsInUse[ctxIndex].State.GeometryShader = geometryShader.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDGeometryShader, true);
	}
}

void D3DEngine::SetComputeShader(const D3DComputeShader &computeShader, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (computeShader != m_ContextsInUse[ctxIndex].State.ComputeShader)
	{
		m_ContextsInUse[ctxIndex].State.ComputeShader = computeShader.Get();

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDComputeShader, true);
	}
}

void D3DEngine::SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxConstantBuffers && targetShader < D3DShader::eShaderTypeCount && m_ContextsInUse[ctxIndex].IsValid());

	if (constantBuffer != m_ContextsInUse[ctxIndex].State.ConstantBuffers[targetShader][slot])
	{
		m_ContextsInUse[ctxIndex].State.ConstantBuffers[targetShader][slot] = constantBuffer.Get();

		m_ContextsInUse[ctxIndex].State.ConstantBuffersInUse[targetShader] |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDConstantBuffer, true);
	}
}

void D3DEngine::SetViewport(const D3DViewport &viewport, uint32_t slot, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxViewports && m_ContextsInUse[ctxIndex].IsValid());

	if (m_ContextsInUse[ctxIndex].State.Viewports[slot] != viewport)
	{
		m_ContextsInUse[ctxIndex].State.Viewports[slot] = viewport;

		m_ContextsInUse[ctxIndex].State.ViewportsInUse |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDViewport, true);
	}
}

void D3DEngine::SetScissorRect(const D3DRect &rect, uint32_t slot, uint32_t ctxIndex)
{
	assert(slot < D3DContext::eMaxScissorRects && m_ContextsInUse[ctxIndex].IsValid());

	if (m_ContextsInUse[ctxIndex].State.ScissorRects[slot] != rect)
	{
		m_ContextsInUse[ctxIndex].State.ScissorRects[slot] = rect;

		m_ContextsInUse[ctxIndex].State.ScissorRectsInUse |= (1U << slot);

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDScissorRect, true);
	}
}

void D3DEngine::Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive, bool bClearState, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (m_ContextsInUse[ctxIndex].State.PrimitiveTopology != primitive)
	{
		m_ContextsInUse[ctxIndex].State.PrimitiveTopology = primitive;

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDPrimitiveTopology, true);
	}

	m_ContextsInUse[ctxIndex].CommitState();

	m_ContextsInUse[ctxIndex]->Draw(vertexCount, startVertex);

	if (bClearState)
	{
		m_ContextsInUse[ctxIndex].State.ResetState();
	}
}

void D3DEngine::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive, bool bClearState, uint32_t ctxIndex)
{
	assert(m_ContextsInUse[ctxIndex].IsValid());

	if (m_ContextsInUse[ctxIndex].State.PrimitiveTopology != primitive)
	{
		m_ContextsInUse[ctxIndex].State.PrimitiveTopology = primitive;

		m_ContextsInUse[ctxIndex].State.SetDirty(D3DContext::eDPrimitiveTopology, true);
	}

	m_ContextsInUse[ctxIndex].CommitState();

	m_ContextsInUse[ctxIndex]->DrawIndexed(indexCount, startIndex, offset);

	if (bClearState)
	{
		m_ContextsInUse[ctxIndex].State.ResetState();
	}
}

#if 0
void D3DEngine::DrawTextInPos(const char *pTextContent, uint32_t left, uint32_t top, uint32_t fontSize)
{
	assert(pTextContent);

	D3DFontFreeType::Instance().RenderText(pTextContent, left, top, fontSize);
}
#endif