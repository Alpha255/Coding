#include "D3DGraphic.h"
#include <fstream>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

D3DGraphic::D3DGraphic()
{
	memset(&m_D3DPipeLineState, 0, sizeof(D3DPipelineState));
	memset(&m_FlushState, 0, sizeof(bool) * eFSCount);
	memset(&m_SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
}

void D3DGraphic::CreateTexture2D(ID3D11Texture2D** ppTexture, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
	uint32_t bindFlags, uint32_t mipLevels, uint32_t arraySize, uint32_t cpuFlags, uint32_t sampleCount, uint32_t sampleQuality,
	uint32_t miscFlags, D3D11_USAGE usage)
{
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Format = fmt;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.BindFlags = bindFlags;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = arraySize;
	texDesc.CPUAccessFlags = cpuFlags;
	texDesc.SampleDesc.Count = sampleCount;
	texDesc.SampleDesc.Quality = sampleQuality;
	texDesc.MiscFlags = miscFlags;
	texDesc.Usage = usage;

	HRCheck(m_D3DDevice->CreateTexture2D(&texDesc, nullptr, ppTexture));
}

void D3DGraphic::CreateDepthStencilView(ID3D11DepthStencilView** ppDSV, DXGI_FORMAT fmt, uint32_t width, uint32_t height)
{
	assert(ppDSV);
	assert(width > 0U);
	assert(height > 0U);

	ID3D11Texture2D* pTexture = nullptr;
	CreateTexture2D(&pTexture, fmt, width, height, D3D11_BIND_DEPTH_STENCIL, 1U, 1U, 0U, 0U, 0U, 0U, D3D11_USAGE_DEFAULT);
	HRCheck(m_D3DDevice->CreateDepthStencilView(pTexture, nullptr, ppDSV));
	SafeRelease(pTexture);
}

void D3DGraphic::InitD3DEnvironment(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed)
{
	assert(hWnd);

	/// Create D3D Device and SwapChain
	uint32_t flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	m_SwapChainDesc.BufferCount = eCTBackBuffer;
	m_SwapChainDesc.BufferDesc.Width = width;
	m_SwapChainDesc.BufferDesc.Height = height;
	m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	m_SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	m_SwapChainDesc.OutputWindow = hWnd;
	m_SwapChainDesc.Windowed = bWindowed;
	m_SwapChainDesc.SampleDesc.Count = 1;
	m_SwapChainDesc.SampleDesc.Quality = 0;
	m_SwapChainDesc.SwapEffect = ((eCTBackBuffer > 1) ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD);
	m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	for (UINT index = 0; index < ARRAYSIZE(driverTypes); ++index)
	{
		if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(nullptr, 
			driverTypes[index], 
			nullptr, 
			flags,
			featureLevels, 
			ARRAYSIZE(featureLevels), 
			D3D11_SDK_VERSION,
			&m_SwapChainDesc, 
			m_SwapChain.GetResource(), 
			m_D3DDevice.GetResource(),
			nullptr, 
			m_D3DContext.GetResource())))
		{
			/// Create Back Buffer 
			RecreateBackBuffer();

			/// Create Depth Stencil View
			CreateDepthStencilView(m_DefaultDepthStencilView.GetResource(), DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);

			break;
		}
	}

	assert(!"Failed to create D3D environment!!!");
}

void D3DGraphic::CreateShaderResourceView(ID3D11ShaderResourceView** ppSRV, char* pFileName)
{
	assert(ppSRV);
	assert(pFileName);
}

void D3DGraphic::CreateStreamBuffer(ID3D11Buffer** ppBuffer, D3D11_BIND_FLAG bindFlag, uint32_t byteWidth, 
	D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag)
{
	assert(ppBuffer);

	switch (bindFlag)
	{
	case D3D11_BIND_INDEX_BUFFER:
	case D3D11_BIND_VERTEX_BUFFER:
	{
		D3D11_BUFFER_DESC bufDesc;
		D3D11_SUBRESOURCE_DATA srcData;
		memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));
		memset(&srcData, 0, sizeof(D3D11_SUBRESOURCE_DATA));

		bufDesc.ByteWidth = byteWidth;
		bufDesc.Usage = usage;
		bufDesc.BindFlags = bindFlag;
		bufDesc.CPUAccessFlags = cpuAccessFlag;
		bufDesc.MiscFlags = 0U;
		bufDesc.StructureByteStride = 0U;

		srcData.pSysMem = pBuf;
		srcData.SysMemPitch = 0U;
		srcData.SysMemSlicePitch = 0U;

		HRCheck(m_D3DDevice->CreateBuffer(&bufDesc, ((nullptr == pBuf) ? nullptr : &srcData), ppBuffer));
	}
		break;
	case D3D11_BIND_CONSTANT_BUFFER:
	{
		D3D11_BUFFER_DESC bufDesc;
		memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));

		bufDesc.Usage = usage;
		bufDesc.ByteWidth = byteWidth;
		bufDesc.CPUAccessFlags = cpuAccessFlag;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.MiscFlags = 0U;
		bufDesc.StructureByteStride = 0U;

		HRCheck(m_D3DDevice->CreateBuffer(&bufDesc, nullptr, ppBuffer));
	}
		break;
	default:
		assert(!"Unsupport yet!!!");
		break;
	}
}

void D3DGraphic::CreateBlendState(ID3D11BlendState** ppBlendState, D3D11_BLEND_DESC* pBlendDesc)
{
	assert(ppBlendState);

	HRCheck(m_D3DDevice->CreateBlendState(pBlendDesc, ppBlendState));
}

void D3DGraphic::CompileShaderFile(ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget)
{
	assert(ppRes && pFileName && pEntryPoint && pTarget);

	std::ifstream file(pFileName, std::ios::in);
	if (file.good())
	{
		file.seekg(std::ios::end);
		size_t fileSize = (size_t)file.tellg();

		char* pData = new char[fileSize]();
		file.seekg(std::ios::beg);
		file.read(pData, fileSize);

#if defined(DEBUG) || defined(_DEBUG)
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

		ID3DBlob* pErrMsg = nullptr;
		if (FAILED(D3DCompile(pData, fileSize, pFileName, nullptr, nullptr, pEntryPoint, pTarget, flags, 0U, ppRes, &pErrMsg)))
		{
			OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
			assert(!"Shader compile failed!!!");
		}

		SafeRelease(pErrMsg);
		SafeDeleteArray(pData);
		file.close();
	}

	assert(!"Failed to open specified shader file!!!");
}

void D3DGraphic::CreateVertexShader(ID3D11VertexShader** ppVS, char* pFileName, char* pEntryPoint)
{
	assert(ppVS);

	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "vs_5_0");
	HRCheck(m_D3DDevice->CreateVertexShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, ppVS));
	SafeRelease(pRes);
}

void D3DGraphic::CreatePixelShader(ID3D11PixelShader** ppPS, char* pFileName, char* pEntryPoint)
{
	assert(ppPS);

	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "ps_5_0");
	HRCheck(m_D3DDevice->CreatePixelShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, ppPS));
	SafeRelease(pRes);
}

void D3DGraphic::CreateRasterizerState(ID3D11RasterizerState** ppRasterizerState, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool cw, bool depthClip)
{
	assert(ppRasterizerState);

	D3D11_RASTERIZER_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = fillMode;
	rsDesc.CullMode = cullMode;
	rsDesc.FrontCounterClockwise = cw;
	rsDesc.DepthClipEnable = depthClip;

	HRCheck(m_D3DDevice->CreateRasterizerState(&rsDesc, ppRasterizerState));
}

void D3DGraphic::CreateInputLayout(ID3D11InputLayout** ppInputLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, ID3DBlob* pRes)
{
	assert(ppInputLayout && pInputElement && (size > 0U) && pRes);

	HRCheck(m_D3DDevice->CreateInputLayout(pInputElement, size, pRes->GetBufferPointer(), pRes->GetBufferSize(), ppInputLayout));
}

void D3DGraphic::ClearRenderTargetView(ID3D11RenderTargetView* pRenderTarget, float* pClearColor)
{
	assert(pRenderTarget);

	float darkBlue[] = { 0.0f, 0.125f, 0.3f, 1.0f };
	pClearColor = (nullptr == pClearColor ? darkBlue : pClearColor);

	m_D3DContext->ClearRenderTargetView(pRenderTarget, pClearColor);
}

void D3DGraphic::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencil, float depth, uint8_t stencil)
{
	assert(pDepthStencil);

	m_D3DContext->ClearDepthStencilView(pDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

void D3DGraphic::ResizeBackBuffer(uint32_t width, uint32_t height)
{

}

inline void D3DGraphic::SetVertexBuffer(ID3D11Buffer* pBuffer, uint32_t stride, uint32_t offset, uint32_t index)
{

}

inline void D3DGraphic::SetIndexBuffer(ID3D11Buffer* pBuffer, DXGI_FORMAT format, uint32_t offset)
{

}

inline void D3DGraphic::SetInputLayout(ID3D11InputLayout* pInputLayout)
{

}

inline void D3DGraphic::SetRenderTargetView(ID3D11RenderTargetView* pRenderTarget)
{

}

inline void D3DGraphic::SetDepthStencilView(ID3D11DepthStencilView* pDepthStencil)
{

}

inline void D3DGraphic::SetViewports(D3D11_VIEWPORT* pViewports)
{

}

inline void D3DGraphic::SetRasterizerState(ID3D11RasterizerState* pRasterizerState)
{

}

inline void D3DGraphic::SetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState)
{

}

inline void D3DGraphic::SetBlendState(ID3D11BlendState* pBlendState, float* pBlendFactor, uint32_t mask)
{

}

inline void D3DGraphic::SetVertexShader(ID3D11VertexShader* pVertexShader)
{

}

inline void D3DGraphic::SetPixelShader(ID3D11PixelShader* pPixelShader)
{

}

inline void D3DGraphic::SetConstantBuffer(Ref<ID3D11Buffer>& pConstantBuf, uint32_t slot, eD3DShaderType shaderType)
{

}

void D3DGraphic::UpdateConstantBuffer(ID3D11Resource* pTarget, const void* pSource, uint32_t size)
{

}

void D3DGraphic::Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim)
{
	FlushPipeLineState();

	m_D3DContext->Draw(vertexCount, startIndex);
}

void D3DGraphic::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim)
{
	FlushPipeLineState();

	m_D3DContext->DrawIndexed(indexCount, startIndex, offset);
}

void D3DGraphic::FlushPipeLineState()
{

}