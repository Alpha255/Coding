#include "D3DGraphic.h"
#include <fstream>
#include <string>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

const char* ResourceFilePath(const char* pFileName, D3DGraphic::eD3DResourceType resType)
{
	static const char* s_ResourcePath[D3DGraphic::eRTCount] = { "\\Resource\\Shaders\\", "Resource\\Textures\\" };
	static char filePath[MAX_PATH] = { 0 };
	::GetModuleFileNameA(::GetModuleHandle(nullptr), filePath, MAX_PATH);
	std::string strFileName(filePath);
	std::string strFilePath = strFileName.substr(0, strFileName.rfind("\\"));
	strFilePath += s_ResourcePath[resType];
	strFilePath += pFileName;
	memset(filePath, 0, MAX_PATH);
	memcpy(filePath, strFilePath.c_str(), strFilePath.size());

	return filePath;
}

D3DGraphic::D3DGraphic()
{
	memset(&m_D3DPipelineState, 0, sizeof(D3DPipelineState));
	memset(&m_FlushState, 0, sizeof(bool) * eFSCount);
	memset(&m_SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	memset(&m_DefaultViewport, 0, sizeof(D3D11_VIEWPORT));
}

void D3DGraphic::CreateTexture2D(ID3D11Texture2D** ppTexture, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
	uint32_t bindFlags, uint32_t mipLevels, uint32_t arraySize, uint32_t cpuFlags, uint32_t sampleCount, uint32_t sampleQuality,
	uint32_t miscFlags, D3D11_USAGE usage, const void* pData, uint32_t pitch, uint32_t slice)
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

	D3D11_SUBRESOURCE_DATA subResData;
	memset(&subResData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = pData;
	subResData.SysMemPitch = pitch;
	subResData.SysMemSlicePitch = slice;

	HRCheck(m_D3DDevice->CreateTexture2D(&texDesc, ((nullptr == pData) ?  nullptr : &subResData), ppTexture));
}

void D3DGraphic::CreateDepthStencilView(ID3D11DepthStencilView** ppDSV, ID3D11Resource* pResource, DXGI_FORMAT format, uint32_t width, uint32_t height, D3D11_DSV_DIMENSION dimension)
{
	assert(ppDSV);

	if (pResource)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		memset(&dsvDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvDesc.Format = format;
		dsvDesc.ViewDimension = dimension;
		dsvDesc.Flags = 0U;
		switch (dimension)
		{
		case D3D11_DSV_DIMENSION_TEXTURE1D:
			dsvDesc.Texture1D.MipSlice = 0U;
			break;
		case D3D11_DSV_DIMENSION_TEXTURE2D:
			dsvDesc.Texture2D.MipSlice = 0U;
			break;
		default:
			assert("Invalid type!!!");
			break;
		}
		HRCheck(m_D3DDevice->CreateDepthStencilView(pResource, &dsvDesc, ppDSV));
	}
	else
	{
		ID3D11Texture2D* pTexture = nullptr;
		CreateTexture2D(&pTexture, format, width, height, D3D11_BIND_DEPTH_STENCIL);
		HRCheck(m_D3DDevice->CreateDepthStencilView(pTexture, nullptr, ppDSV));
		SafeRelease(pTexture);
	}
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
			m_SwapChain.GetReference(),
			m_D3DDevice.GetReference(),
			nullptr, 
			m_D3DContext.GetReference())))
		{
			/// Create Back Buffer 
			RecreateBackBuffer();

			/// Create Depth Stencil View
			CreateDepthStencilView(m_DefaultDepthStencil.GetReference(), nullptr, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);

			return;
		}
	}

	assert(!"Failed to create D3D environment!!!");
}

void D3DGraphic::CreateShaderResourceView(ID3D11ShaderResourceView** ppSRV, char* pFileName)
{
	assert(ppSRV);
	assert(pFileName);
	
	assert(!"Unsupport yet!!!");
}

void D3DGraphic::CreateRenderTargetView(ID3D11RenderTargetView** ppRTV, ID3D11Resource* pResource)
{
	assert(ppRTV && pResource);
	///D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	///memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC)); 
	
	HRCheck(m_D3DDevice->CreateRenderTargetView(pResource, nullptr, ppRTV));
}

void D3DGraphic::CreateShaderResourceView(ID3D11ShaderResourceView** ppSRV, ID3D11Resource* pResource, DXGI_FORMAT format, D3D11_SRV_DIMENSION dimension)
{
	assert(ppSRV && pResource);
	if (DXGI_FORMAT_UNKNOWN == format)
	{
		HRCheck(m_D3DDevice->CreateShaderResourceView(pResource, nullptr, ppSRV));
	}
	else
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = format;
		srvDesc.ViewDimension = dimension;
		switch (dimension)
		{
		case D3D11_SRV_DIMENSION_TEXTURE1D:
			srvDesc.Texture1D.MipLevels = 1U;
			srvDesc.Texture1D.MostDetailedMip = 0U;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2D:
			srvDesc.Texture2D.MipLevels = 1U;
			srvDesc.Texture2D.MostDetailedMip = 0U;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE3D:
			srvDesc.Texture3D.MipLevels = 1U;
			srvDesc.Texture3D.MostDetailedMip = 0U;
			break;
		case D3D11_SRV_DIMENSION_TEXTURECUBE:
			srvDesc.TextureCube.MipLevels = 1U;
			srvDesc.TextureCube.MostDetailedMip = 0U;
			break;
		default:
			assert("Invalid type!!");
			break;
		}
		HRCheck(m_D3DDevice->CreateShaderResourceView(pResource, &srvDesc, ppSRV));
	}
}

void D3DGraphic::CreateBuffer(ID3D11Buffer** ppBuffer, D3D11_BIND_FLAG bindFlag, uint32_t byteWidth,
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

void D3DGraphic::CompileShaderFile(ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget, const D3D_SHADER_MACRO* pDefines)
{
	assert(ppRes && pFileName && pEntryPoint && pTarget);

	std::ifstream file(ResourceFilePath(pFileName, eRTShader), std::ios::in);
	if (file.good())
	{
		file.seekg(0U, std::ios::end);
		size_t fileSize = (size_t)file.tellg();

		char* pData = new char[fileSize]();
		file.seekg(0U, std::ios::beg);
		file.read(pData, fileSize);

#if defined(DEBUG) || defined(_DEBUG)
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

		ID3DBlob* pErrMsg = nullptr;
		if (FAILED(D3DCompile(pData, fileSize, pFileName, pDefines, nullptr, pEntryPoint, pTarget, flags, 0U, ppRes, &pErrMsg)))
		{
			OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
			assert(!"Shader compile failed!!!");
		}

		SafeRelease(pErrMsg);
		SafeDeleteArray(pData);
		file.close();
	}
	else
	{
		assert(!"Failed to open specified shader file!!!");
	}
}

void D3DGraphic::RecreateBackBuffer()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer));
	HRCheck(m_D3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, m_DefaultRenderTarget.GetReference()));
	SafeRelease(pBackBuffer);
}

void D3DGraphic::CreateVertexShader(ID3D11VertexShader** ppVS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines)
{
	assert(ppVS);

	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "vs_5_0", pDefines);
	HRCheck(m_D3DDevice->CreateVertexShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, ppVS));
	SafeRelease(pRes);
}

void D3DGraphic::CreatePixelShader(ID3D11PixelShader** ppPS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines)
{
	assert(ppPS);

	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "ps_5_0", pDefines);
	HRCheck(m_D3DDevice->CreatePixelShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, ppPS));
	SafeRelease(pRes);
}

void D3DGraphic::CreateVertexShaderAndInputLayout(ID3D11VertexShader** ppVS, ID3D11InputLayout** ppLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, char* pFileName, char* pEntryPoint)
{
	assert(ppVS);

	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "vs_5_0", nullptr);
	HRCheck(m_D3DDevice->CreateVertexShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, ppVS));
	CreateInputLayout(ppLayout, pInputElement, size, pRes);
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
	rsDesc.MultisampleEnable = true;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;

	HRCheck(m_D3DDevice->CreateRasterizerState(&rsDesc, ppRasterizerState));
}

void D3DGraphic::CreateSamplerState(ID3D11SamplerState** ppSamplerState, const D3D11_SAMPLER_DESC* pSamplerDesc)
{
	assert(ppSamplerState && pSamplerDesc);

	HRCheck(m_D3DDevice->CreateSamplerState(pSamplerDesc, ppSamplerState));
}

void D3DGraphic::CreateInputLayout(ID3D11InputLayout** ppInputLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, ID3DBlob* pRes)
{
	assert(ppInputLayout && pInputElement && (size > 0U) && pRes);

	HRCheck(m_D3DDevice->CreateInputLayout(pInputElement, size, pRes->GetBufferPointer(), pRes->GetBufferSize(), ppInputLayout));
}

void D3DGraphic::ClearRenderTarget(const Ref<ID3D11RenderTargetView>& refRenderTarget, float* pClearColor)
{
	assert(refRenderTarget.IsValid());

	float darkBlue[] = { 0.0f, 0.125f, 0.3f, 1.0f };
	pClearColor = (nullptr == pClearColor ? darkBlue : pClearColor);

	m_D3DContext->ClearRenderTargetView(refRenderTarget.GetPtr(), pClearColor);
}

void D3DGraphic::ClearDepthStencil(const Ref<ID3D11DepthStencilView>& refDepthStencil, float depth, uint8_t stencil)
{
	assert(refDepthStencil.IsValid());

	m_D3DContext->ClearDepthStencilView(refDepthStencil.GetPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

void D3DGraphic::ResizeBackBuffer(uint32_t width, uint32_t height)
{
	if (!m_SwapChain.IsValid())
	{
		return;
	}

	assert((width > 0U) && (height > 0U));

	if (width != m_SwapChainDesc.BufferDesc.Width || height != m_SwapChainDesc.BufferDesc.Height)
	{
		m_SwapChainDesc.BufferDesc.Width = width;
		m_SwapChainDesc.BufferDesc.Height = height;

		if (m_DefaultRenderTarget.IsValid())
		{
			m_DefaultRenderTarget->Release();
		}
		m_SwapChain->ResizeBuffers(m_SwapChainDesc.BufferCount, width, height, m_SwapChainDesc.BufferDesc.Format, m_SwapChainDesc.Flags);
		RecreateBackBuffer();
		SetRenderTarget(m_DefaultRenderTarget);

		if (m_DefaultDepthStencil.IsValid())
		{
			m_DefaultDepthStencil->Release();
		}
		CreateDepthStencilView(m_DefaultDepthStencil.GetReference(), nullptr, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);
		SetDepthStencil(m_DefaultDepthStencil);
	}
}

void D3DGraphic::SetVertexBuffer(const Ref<ID3D11Buffer>& refBuffer, uint32_t stride, uint32_t offset, uint32_t index)
{
	assert(index <= eCTVertexStream);

	ID3D11Buffer* const pBuffer = refBuffer.GetPtr();
	if (m_D3DPipelineState.VertexBuffer.Buffers[index] != pBuffer ||
		m_D3DPipelineState.VertexBuffer.Stride[index] != stride ||
		m_D3DPipelineState.VertexBuffer.Offset[index] != offset)
	{
		m_D3DPipelineState.VertexBuffer.Buffers[index] = pBuffer;
		m_D3DPipelineState.VertexBuffer.Stride[index] = stride;
		m_D3DPipelineState.VertexBuffer.Offset[index] = offset;

		m_FlushState[eFSVertexBuffer] = true;
	}
}

void D3DGraphic::SetIndexBuffer(const Ref<ID3D11Buffer>& refBuffer, DXGI_FORMAT format, uint32_t offset)
{
	ID3D11Buffer* const pBuffer = refBuffer.GetPtr();
	if (m_D3DPipelineState.IndexBuffer.Buffers != pBuffer ||
		m_D3DPipelineState.IndexBuffer.Format != format ||
		m_D3DPipelineState.IndexBuffer.Offset != offset)
	{
		m_D3DPipelineState.IndexBuffer.Buffers = pBuffer;
		m_D3DPipelineState.IndexBuffer.Format = format;
		m_D3DPipelineState.IndexBuffer.Offset = offset;

		m_FlushState[eFSIndexBuffer] = true;
	}
}

void D3DGraphic::SetInputLayout(const Ref<ID3D11InputLayout>& refInputLayout)
{
	ID3D11InputLayout* const pInputLayout = refInputLayout.GetPtr();
	if (m_D3DPipelineState.InputLayout != pInputLayout)
	{
		m_D3DPipelineState.InputLayout = pInputLayout;

		m_FlushState[eFSInputLayout] = true;
	}
}

void D3DGraphic::SetRenderTarget(const Ref<ID3D11RenderTargetView>& refRenderTarget, uint32_t slot)
{
	assert(slot <= eCTRenderTarget);

	ID3D11RenderTargetView* const pRenderTarget = refRenderTarget.GetPtr();
	if (m_D3DPipelineState.RenderTarget[slot] != pRenderTarget)
	{
		m_D3DPipelineState.RenderTarget[slot] = pRenderTarget;

		m_FlushState[eFSRenderTarget] = true;
	}
}

void D3DGraphic::SetDepthStencil(const Ref<ID3D11DepthStencilView>& refDepthStencil)
{
	ID3D11DepthStencilView* const pDepthStencil = refDepthStencil.GetPtr();
	if (m_D3DPipelineState.DepthStencil != pDepthStencil)
	{
		m_D3DPipelineState.DepthStencil = pDepthStencil;

		m_FlushState[eFSDepthStencil] = true;
	}
}

void D3DGraphic::SetViewports(D3D11_VIEWPORT* pViewports, uint32_t count)
{
	assert(count <= 1U && "Unsupport yet!!!");

	if (m_D3DPipelineState.ViewportCount != count ||
		m_D3DPipelineState.Viewports != pViewports)
	{
		m_D3DPipelineState.ViewportCount = count;
		m_D3DPipelineState.Viewports = pViewports;

		memcpy(&m_DefaultViewport, pViewports, sizeof(D3D11_VIEWPORT));

		m_FlushState[eFSViewports] = true;
	}
}

void D3DGraphic::SetRasterizerState(const Ref<ID3D11RasterizerState>& refRasterizerState)
{
	ID3D11RasterizerState* const pRasterizerState = refRasterizerState.GetPtr();
	if (m_D3DPipelineState.RasterizerState != pRasterizerState)
	{
		m_D3DPipelineState.RasterizerState = pRasterizerState;

		m_FlushState[eFSRasterizerState] = true;
	}
}

void D3DGraphic::SetDepthStencilState(const Ref<ID3D11DepthStencilState>& refDepthStencilState)
{
	ID3D11DepthStencilState* const pDepthStencilState = refDepthStencilState.GetPtr();
	if (m_D3DPipelineState.DepthStencilState != pDepthStencilState)
	{
		m_D3DPipelineState.DepthStencilState = pDepthStencilState;

		m_FlushState[eFSDepthStencilState] = true;
	}
}

void D3DGraphic::SetBlendState(const Ref<ID3D11BlendState>& refBlendState, DirectX::XMFLOAT4 blendFactor, uint32_t mask)
{
	ID3D11BlendState* const pBlendState = refBlendState.GetPtr();
	if (m_D3DPipelineState.BlendState != pBlendState ||
		m_D3DPipelineState.SampleMask != mask)
	{
		m_D3DPipelineState.BlendState = pBlendState;
		memcpy(m_D3DPipelineState.BlendFactor, &blendFactor, sizeof(DirectX::XMFLOAT4));
		m_D3DPipelineState.SampleMask = mask;

		m_FlushState[eFSBlendState] = true;
	}
}

void D3DGraphic::SetVertexShader(const Ref<ID3D11VertexShader>& refVertexShader)
{
	assert(refVertexShader.IsValid());

	ID3D11VertexShader* const pVertexShader = refVertexShader.GetPtr();
	if (m_D3DPipelineState.VertexShader != pVertexShader)
	{
		m_D3DPipelineState.VertexShader = pVertexShader;

		m_FlushState[eFSVertexShader] = true;
	}
}

void D3DGraphic::SetPixelShader(const Ref<ID3D11PixelShader>& refPixelShader)
{
	ID3D11PixelShader* const pPixelShader = refPixelShader.GetPtr();
	if (m_D3DPipelineState.PixelShader != pPixelShader)
	{
		m_D3DPipelineState.PixelShader = pPixelShader;

		m_FlushState[eFSPixelShader] = true;
	}
}

void D3DGraphic::SetConstantBuffer(Ref<ID3D11Buffer>& pConstantBuf, uint32_t slot, eD3DShaderType shaderType)
{
	assert(pConstantBuf.IsValid());

	switch (shaderType)
	{
	case eSTVertexShader:
		m_D3DContext->VSSetConstantBuffers(slot, 1U, pConstantBuf.GetReference());
		break;
	case eSTPixelShader:
		m_D3DContext->PSSetConstantBuffers(slot, 1U, pConstantBuf.GetReference());
		break;
	default:
		assert(!"Unsupport yet!!");
		break;
	}
}

void D3DGraphic::UpdateConstantBuffer(ID3D11Resource* pTarget, const void* pSource, uint32_t size)
{
	assert(pTarget && pSource && (size > 0U));

	D3D11_MAPPED_SUBRESOURCE mapData;
	memset(&mapData, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HRCheck(m_D3DContext->Map(pTarget, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapData));
	memcpy(mapData.pData, pSource, size);
	m_D3DContext->Unmap(pTarget, 0U);
}

void D3DGraphic::Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_D3DPipelineState.PrimitiveTopology != prim)
	{
		m_D3DPipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushPipelineState();

	m_D3DContext->Draw(vertexCount, startIndex);

	m_SwapChain->Present(0U, 0U);
}

void D3DGraphic::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_D3DPipelineState.PrimitiveTopology != prim)
	{
		m_D3DPipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushPipelineState();

	m_D3DContext->DrawIndexed(indexCount, startIndex, offset);

	m_SwapChain->Present(0U, 0U);
}

void D3DGraphic::FlushPipelineState()
{
	if (!m_D3DContext.IsValid())
	{
		return;
	}

	/// Input Assembler
	if (m_FlushState[eFSInputLayout])
	{
		m_D3DContext->IASetInputLayout(m_D3DPipelineState.InputLayout);
		m_FlushState[eFSInputLayout] = false;
	}

	if (m_FlushState[eFSVertexBuffer])
	{
		m_D3DContext->IASetVertexBuffers(0, eCTVertexStream, m_D3DPipelineState.VertexBuffer.Buffers,
			m_D3DPipelineState.VertexBuffer.Stride, m_D3DPipelineState.VertexBuffer.Offset);
		m_FlushState[eFSVertexBuffer] = false;
	}

	if (m_FlushState[eFSIndexBuffer])
	{
		m_D3DContext->IASetIndexBuffer(m_D3DPipelineState.IndexBuffer.Buffers, m_D3DPipelineState.IndexBuffer.Format,
			m_D3DPipelineState.IndexBuffer.Offset);
		m_FlushState[eFSIndexBuffer] = false;
	}

	if (m_FlushState[eFSPrimitiveTopology])
	{
		m_D3DContext->IASetPrimitiveTopology(m_D3DPipelineState.PrimitiveTopology);
		m_FlushState[eFSPrimitiveTopology] = false;
	}

	/// VS->HS->DS->GS
	if (m_FlushState[eFSVertexShader])
	{
		m_D3DContext->VSSetShader(m_D3DPipelineState.VertexShader, nullptr, 0U);
		m_FlushState[eFSVertexShader] = false;
	}

	/// Rasterizer
	if (m_FlushState[eFSRasterizerState])
	{
		m_D3DContext->RSSetState(m_D3DPipelineState.RasterizerState);
		m_FlushState[eFSRasterizerState] = false;
	}

	if (m_FlushState[eFSViewports])
	{
		m_D3DContext->RSSetViewports(m_D3DPipelineState.ViewportCount, &m_DefaultViewport);
		m_FlushState[eFSViewports] = false;
	}

	/// Pixel Shader
	if (m_FlushState[eFSPixelShader])
	{
		m_D3DContext->PSSetShader(m_D3DPipelineState.PixelShader, nullptr, 0U);
		m_FlushState[eFSPixelShader] = false;
	}

	/// Output Merge
	if (m_FlushState[eFSBlendState])
	{
		m_D3DContext->OMSetBlendState(m_D3DPipelineState.BlendState, m_D3DPipelineState.BlendFactor, m_D3DPipelineState.SampleMask);
		m_FlushState[eFSBlendState] = false;
	}

	if (m_FlushState[eFSDepthStencilState])
	{
		m_D3DContext->OMSetDepthStencilState(m_D3DPipelineState.DepthStencilState, 0U);
		m_FlushState[eFSDepthStencilState] = false;
	}

	if (m_FlushState[eFSRenderTarget] || m_FlushState[eFSDepthStencil] || eCTBackBuffer > 1U)
	{
		m_D3DContext->OMSetRenderTargets(eCTRenderTarget, m_D3DPipelineState.RenderTarget, m_D3DPipelineState.DepthStencil);
		m_FlushState[eFSRenderTarget] = m_FlushState[eFSDepthStencil] = false;
	}
}