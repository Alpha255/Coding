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

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	///g_Renderer->SetViewports(&viewport);
}

void D3DEngine::SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot)
{

}

void D3DEngine::SetDepthStencilView(const D3DDepthStencilView &depthStencilView)
{

}

void D3DEngine::SetShaderResource(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader)
{

}

void D3DEngine::SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t index)
{

}

void D3DEngine::SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
{

}

void D3DEngine::SetSamplerStates(const D3DSamplerState &samplerStates, uint32_t startSlot, D3DShader::eShaderType targetShader)
{

}

void D3DEngine::SetRasterizerState(const D3DRasterizerState &rasterizerState)
{

}

void D3DEngine::SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef)
{

}

void D3DEngine::SetBlendState(const D3DBlendState &blendState, const float *pFactor, uint32_t mask)
{

}

void D3DEngine::SetInputLayout(const D3DInputLayout &inputLayout)
{

}

void D3DEngine::SetVertexShader(const D3DVertexShader &vertexShader)
{

}

void D3DEngine::SetHullShader(const D3DHullShader &hullShader)
{

}

void D3DEngine::SetDomainShader(const D3DDomainShader &domainShader)
{

}

void D3DEngine::SetPixelShader(const D3DPixelShader &pixelShader)
{

}

void D3DEngine::SetGeometryShader(const D3DGeometryShader &geometryShader)
{

}

void D3DEngine::SetComputeShader(const D3DComputeShader &computeShader)
{

}

void D3DEngine::SetConstantBuffer(const D3DBuffer &constantsBuf, uint32_t slot, D3DShader::eShaderType targetShader)
{

}

void D3DEngine::SetViewport(const D3DViewport &viewport, uint32_t index)
{

}

void D3DEngine::SetScissorRect(const D3DRect &rect, uint32_t index)
{

}