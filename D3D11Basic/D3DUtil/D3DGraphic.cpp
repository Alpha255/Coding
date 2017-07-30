#include "D3DGraphic.h"
#include <fstream>
#include <string>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

const char* D3DGraphic::ResourceFileDirectory(eResourceType resType)
{
	static const char* s_ResourcePath[D3DGraphic::eResourceType::eCount] =
	{
		"\\Resource\\Shaders\\",
		"\\Resource\\Textures\\",
		"\\Resource\\SDKMeshs\\",
	};
	static char directory[MAX_PATH] = { 0 };
	::GetModuleFileNameA(::GetModuleHandle(nullptr), directory, MAX_PATH);
	std::string strFileName(directory);
	std::string strFilePath = strFileName.substr(0, strFileName.rfind("\\"));
	strFilePath += s_ResourcePath[resType];
	memset(directory, 0, MAX_PATH);
	strcpy_s(directory, strFilePath.c_str());

	return directory;
}

std::string D3DGraphic::ResourceFilePath(const char* pFileName, eResourceType resType)
{
	std::string filePath = ResourceFileDirectory(resType);
	filePath += pFileName;

	return filePath;
}

D3DGraphic::D3DGraphic()
{
	memset(&m_D3DPipelineState, 0, sizeof(D3DPipelineState));
	memset(&m_FlushState, 0, sizeof(bool) * eFSCount);
	memset(&m_SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	memset(&m_DefaultViewport, 0, sizeof(D3D11_VIEWPORT));
	memset(&m_DefaultScissorRect, 0, sizeof(D3D11_RECT));
}

void D3DGraphic::CreateTexture2D(Ref<ID3D11Texture2D>& resource, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
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

	HRCheck(m_D3DDevice->CreateTexture2D(&texDesc, ((nullptr == pData) ?  nullptr : &subResData), resource.GetReference()));
}

void D3DGraphic::CreateDepthStencilView(Ref<ID3D11DepthStencilView>& dsv, Ref<ID3D11Texture2D>& resource, DXGI_FORMAT format, uint32_t width, uint32_t height, D3D11_DSV_DIMENSION dimension)
{
	if (resource.GetPtr())
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
		HRCheck(m_D3DDevice->CreateDepthStencilView(resource.GetPtr(), &dsvDesc, dsv.GetReference()));
	}
	else
	{
		Ref<ID3D11Texture2D> texture;
		CreateTexture2D(texture, format, width, height, D3D11_BIND_DEPTH_STENCIL);
		HRCheck(m_D3DDevice->CreateDepthStencilView(texture.GetPtr(), nullptr, dsv.GetReference()));
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

	m_SwapChainDesc.BufferCount = eBackBufferCount;
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
	m_SwapChainDesc.SwapEffect = ((eBackBufferCount > 1) ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD);
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
			Ref<ID3D11Texture2D> empty;
			CreateDepthStencilView(m_DefaultDepthStencil, empty, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);

			return;
		}
	}

	assert(!"Failed to create D3D environment!!!");
}

void D3DGraphic::CreateShaderResourceView(Ref<ID3D11ShaderResourceView>& srv, char* pFileName)
{
	assert(pFileName && srv.IsValid());  /// Warnings...
	
	assert(!"Unsupport yet!!!");
}

void D3DGraphic::CreateRenderTargetView(Ref<ID3D11RenderTargetView>& rtv, Ref<ID3D11Texture2D>& resource)
{
	assert(resource.IsValid());
	///D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	///memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC)); 
	
	HRCheck(m_D3DDevice->CreateRenderTargetView(resource.GetPtr(), nullptr, rtv.GetReference()));
}

void D3DGraphic::CreateShaderResourceView(Ref<ID3D11ShaderResourceView>& srv, Ref<ID3D11Texture2D>& resource, DXGI_FORMAT format, D3D11_SRV_DIMENSION dimension)
{
	assert(resource.IsValid());

	if (DXGI_FORMAT_UNKNOWN == format)
	{
		HRCheck(m_D3DDevice->CreateShaderResourceView(resource.GetPtr(), nullptr, srv.GetReference()));
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
		HRCheck(m_D3DDevice->CreateShaderResourceView(resource.GetPtr(), &srvDesc, srv.GetReference()));
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

void D3DGraphic::CreateBlendState(Ref<ID3D11BlendState>& blendState, D3D11_BLEND_DESC* pBlendDesc)
{
	HRCheck(m_D3DDevice->CreateBlendState(pBlendDesc, blendState.GetReference()));
}

void D3DGraphic::CompileShaderFile(ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget, const D3D_SHADER_MACRO* pDefines)
{
	assert(ppRes && pFileName && pEntryPoint && pTarget);

	std::ifstream file(ResourceFilePath(pFileName, eShader), std::ios::in);
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

void D3DGraphic::CreateVertexShader(Ref<ID3D11VertexShader>& vs, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines)
{
	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "vs_5_0", pDefines);
	HRCheck(m_D3DDevice->CreateVertexShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, vs.GetReference()));
	SafeRelease(pRes);
}

void D3DGraphic::CreatePixelShader(Ref<ID3D11PixelShader>& ps, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines)
{
	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "ps_5_0", pDefines);
	HRCheck(m_D3DDevice->CreatePixelShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, ps.GetReference()));
	SafeRelease(pRes);
}

void D3DGraphic::CreateVertexShaderAndInputLayout(Ref<ID3D11VertexShader>& vs, Ref<ID3D11InputLayout>& layout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, char* pFileName, char* pEntryPoint)
{
	ID3DBlob* pRes = nullptr;
	CompileShaderFile(&pRes, pFileName, pEntryPoint, "vs_5_0", nullptr);
	HRCheck(m_D3DDevice->CreateVertexShader(pRes->GetBufferPointer(), pRes->GetBufferSize(), nullptr, vs.GetReference()));
	CreateInputLayout(layout, pInputElement, size, pRes);
	SafeRelease(pRes);
}

void D3DGraphic::CreateRasterizerState(Ref<ID3D11RasterizerState>& rasterizerState, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool cw, bool depthClip)
{
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

	HRCheck(m_D3DDevice->CreateRasterizerState(&rsDesc, rasterizerState.GetReference()));
}

void D3DGraphic::CreateSamplerState(Ref<ID3D11SamplerState>& samplerState, const D3D11_SAMPLER_DESC* pSamplerDesc)
{
	assert(pSamplerDesc);

	HRCheck(m_D3DDevice->CreateSamplerState(pSamplerDesc, samplerState.GetReference()));
}

void D3DGraphic::CreateInputLayout(Ref<ID3D11InputLayout>& inputLayout, D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, ID3DBlob* pRes)
{
	assert(pInputElement && (size > 0U) && pRes);

	HRCheck(m_D3DDevice->CreateInputLayout(pInputElement, size, pRes->GetBufferPointer(), pRes->GetBufferSize(), inputLayout.GetReference()));
}

void D3DGraphic::ClearRenderTarget(const Ref<ID3D11RenderTargetView>& renderTarget, float* pClearColor)
{
	assert(renderTarget.IsValid());

	float darkBlue[] = { 0.0f, 0.125f, 0.3f, 1.0f };
	pClearColor = (nullptr == pClearColor ? darkBlue : pClearColor);

	m_D3DContext->ClearRenderTargetView(renderTarget.GetPtr(), pClearColor);
}

void D3DGraphic::ClearDepthStencil(const Ref<ID3D11DepthStencilView>& depthStencil, uint32_t clearFlags, float depth, uint8_t stencil)
{
	assert(depthStencil.IsValid());

	m_D3DContext->ClearDepthStencilView(depthStencil.GetPtr(), clearFlags, depth, stencil);
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

		Ref<ID3D11Texture2D> empty;
		CreateDepthStencilView(m_DefaultDepthStencil, empty, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);
		SetDepthStencil(m_DefaultDepthStencil);
	}
}

void D3DGraphic::SetVertexBuffer(const ID3D11Buffer* pVertexBuffer, uint32_t stride, uint32_t offset, uint32_t index)
{
	assert(index <= eVertexStreamCount);

	if (m_D3DPipelineState.VertexBuffer.Buffers[index] != pVertexBuffer ||
		m_D3DPipelineState.VertexBuffer.Stride[index] != stride ||
		m_D3DPipelineState.VertexBuffer.Offset[index] != offset)
	{
		m_D3DPipelineState.VertexBuffer.Buffers[index] = const_cast<ID3D11Buffer*>(pVertexBuffer);
		m_D3DPipelineState.VertexBuffer.Stride[index] = stride;
		m_D3DPipelineState.VertexBuffer.Offset[index] = offset;

		m_FlushState[eFSVertexBuffer] = true;
	}
}

void D3DGraphic::SetIndexBuffer(const ID3D11Buffer* pIndexBuffer, DXGI_FORMAT format, uint32_t offset)
{
	if (m_D3DPipelineState.IndexBuffer.Buffers != pIndexBuffer ||
		m_D3DPipelineState.IndexBuffer.Format != format ||
		m_D3DPipelineState.IndexBuffer.Offset != offset)
	{
		m_D3DPipelineState.IndexBuffer.Buffers = const_cast<ID3D11Buffer*>(pIndexBuffer);
		m_D3DPipelineState.IndexBuffer.Format = format;
		m_D3DPipelineState.IndexBuffer.Offset = offset;

		m_FlushState[eFSIndexBuffer] = true;
	}
}

void D3DGraphic::SetInputLayout(const Ref<ID3D11InputLayout>& inputLayout)
{
	ID3D11InputLayout* const pInputLayout = inputLayout.GetPtr();
	if (m_D3DPipelineState.InputLayout != pInputLayout)
	{
		m_D3DPipelineState.InputLayout = pInputLayout;

		m_FlushState[eFSInputLayout] = true;
	}
}

void D3DGraphic::SetRenderTarget(const Ref<ID3D11RenderTargetView>& renderTarget, uint32_t slot)
{
	assert(slot <= eRenderTargetCount);

	ID3D11RenderTargetView* const pRenderTarget = renderTarget.GetPtr();
	if (m_D3DPipelineState.RenderTarget[slot] != pRenderTarget)
	{
		m_D3DPipelineState.RenderTarget[slot] = pRenderTarget;

		m_FlushState[eFSRenderTarget] = true;
	}
}

void D3DGraphic::SetDepthStencil(const Ref<ID3D11DepthStencilView>& depthStencil)
{
	ID3D11DepthStencilView* const pDepthStencil = depthStencil.GetPtr();
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

void D3DGraphic::SetScissorRects(D3D11_RECT* pRects, uint32_t count)
{
	assert(count <= 1U && "Unsupport yet!!!");

	if (m_D3DPipelineState.ScissorRectCount != count ||
		m_D3DPipelineState.ScissorRects != pRects)
	{
		m_D3DPipelineState.ScissorRectCount = count;
		m_D3DPipelineState.ScissorRects = pRects;

		memcpy(&m_DefaultScissorRect, pRects, sizeof(D3D11_RECT));

		m_FlushState[eFSScissorRect] = true;
	}
}

void D3DGraphic::SetSamplerStates(ID3D11SamplerState* const * ppStates, uint32_t startSlot, uint32_t count)
{
	assert(ppStates);
	m_D3DContext->PSSetSamplers(startSlot, count, ppStates);
}

void D3DGraphic::SetShaderResource(ID3D11ShaderResourceView* const* ppSRV, uint32_t startSlot, uint32_t count)
{
	assert(ppSRV);
	m_D3DContext->PSSetShaderResources(startSlot, count, ppSRV);
}

void D3DGraphic::SetRasterizerState(const Ref<ID3D11RasterizerState>& rasterizerState)
{
	ID3D11RasterizerState* const pRasterizerState = rasterizerState.GetPtr();
	if (m_D3DPipelineState.RasterizerState != pRasterizerState)
	{
		m_D3DPipelineState.RasterizerState = pRasterizerState;

		m_FlushState[eFSRasterizerState] = true;
	}
}

void D3DGraphic::SetDepthStencilState(const Ref<ID3D11DepthStencilState>& depthStencilState)
{
	ID3D11DepthStencilState* const pDepthStencilState = depthStencilState.GetPtr();
	if (m_D3DPipelineState.DepthStencilState != pDepthStencilState)
	{
		m_D3DPipelineState.DepthStencilState = pDepthStencilState;

		m_FlushState[eFSDepthStencilState] = true;
	}
}

void D3DGraphic::SetBlendState(const Ref<ID3D11BlendState>& blendState, Vec4 blendFactor, uint32_t mask)
{
	ID3D11BlendState* const pBlendState = blendState.GetPtr();
	if (m_D3DPipelineState.BlendState != pBlendState ||
		m_D3DPipelineState.SampleMask != mask)
	{
		m_D3DPipelineState.BlendState = pBlendState;
		memcpy(m_D3DPipelineState.BlendFactor, &blendFactor, sizeof(Vec4));
		m_D3DPipelineState.SampleMask = mask;

		m_FlushState[eFSBlendState] = true;
	}
}

void D3DGraphic::SetVertexShader(const Ref<ID3D11VertexShader>& vertexShader)
{
	assert(vertexShader.IsValid());

	ID3D11VertexShader* const pVertexShader = vertexShader.GetPtr();
	if (m_D3DPipelineState.VertexShader != pVertexShader)
	{
		m_D3DPipelineState.VertexShader = pVertexShader;

		m_FlushState[eFSVertexShader] = true;
	}
}

void D3DGraphic::SetPixelShader(const Ref<ID3D11PixelShader>& pixelShader)
{
	ID3D11PixelShader* const pPixelShader = pixelShader.GetPtr();
	if (m_D3DPipelineState.PixelShader != pPixelShader)
	{
		m_D3DPipelineState.PixelShader = pPixelShader;

		m_FlushState[eFSPixelShader] = true;
	}
}

void D3DGraphic::SetConstantBuffer(Ref<ID3D11Buffer>& constantBuf, uint32_t slot, eShaderType shaderType)
{
	assert(constantBuf.IsValid());

	switch (shaderType)
	{
	case eVertexShader:
		m_D3DContext->VSSetConstantBuffers(slot, 1U, constantBuf.GetReference());
		break;
	case ePixelShader:
		m_D3DContext->PSSetConstantBuffers(slot, 1U, constantBuf.GetReference());
		break;
	default:
		assert(!"Unsupport yet!!");
		break;
	}
}

void D3DGraphic::UpdateConstantBuffer(Ref<ID3D11Buffer>& constantBuf, const void* pSource, uint32_t size)
{
	assert(pSource && (size > 0U));

	D3D11_MAPPED_SUBRESOURCE mapData;
	memset(&mapData, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HRCheck(m_D3DContext->Map(constantBuf.GetPtr(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapData));
	memcpy(mapData.pData, pSource, size);
	m_D3DContext->Unmap(constantBuf.GetPtr(), 0U);
}

void D3DGraphic::GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc)
{
	assert(m_SwapChain.IsValid());

	Ref<ID3D11Texture2D> backBuffer;
	HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetReference()));

	backBuffer->GetDesc(&tex2DDesc);
}

void D3DGraphic::ResolveSubResource(const Ref<ID3D11Texture2D>& dstResource, const Ref<ID3D11Texture2D>& srcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt)
{
	assert(dstResource.IsValid() && srcResource.IsValid());

	m_D3DContext->ResolveSubresource(dstResource.GetPtr(), dstCount, srcResource.GetPtr(), srcCount, fmt);
}

void D3DGraphic::Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_D3DPipelineState.PrimitiveTopology != prim)
	{
		m_D3DPipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

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

	FlushState();

	m_D3DContext->DrawIndexed(indexCount, startIndex, offset);

	m_SwapChain->Present(0U, 0U);
}

void D3DGraphic::DrawQuad(float top, float left, float width, float height)
{
	static uint32_t indices[6] = { 0U, 1U, 2U, 0U, 2U, 3U };
	static Ref<ID3D11Buffer> ib;
	if (!ib.IsValid())
	{
		CreateIndexBuffer(ib, sizeof(uint32_t) * 6U, D3D11_USAGE_IMMUTABLE, indices);
	}

	struct Vertex
	{
		Vec3 Pos;
		Vec2 UV;

		Vertex() {}
		Vertex(float x, float y, float z, float u, float v)
			: Pos(x, y, z)
			, UV(u, v)
		{
		}
	};

	static Vertex vertices[4];
	vertices[0] = Vertex(top, left, 1.0f, 0.0f, 0.0f);
	vertices[1] = Vertex(top + width, left, 1.0f, 1.0f, 0.0f);
	vertices[2] = Vertex(top + width, left + height, 1.0f, 1.0f, 1.0f);
	vertices[3] = Vertex(top, left + height, 1.0f, 0.0f, 1.0f);
	static Ref<ID3D11Buffer> vb;
	if (!vb.IsValid())
	{
		CreateVertexBuffer(vb, sizeof(Vertex) * 4, D3D11_USAGE_DYNAMIC, vertices);
	}

	SetVertexBuffer(vb, sizeof(Vertex), 0U);

	SetIndexBuffer(ib, DXGI_FORMAT_R32_UINT);

	DrawIndexed(6U, 0U, 0U);
}

void D3DGraphic::FlushState()
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
		m_D3DContext->IASetVertexBuffers(0, eVertexStreamCount, m_D3DPipelineState.VertexBuffer.Buffers,
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

	if (m_FlushState[eFSScissorRect])
	{
		m_D3DContext->RSSetScissorRects(m_D3DPipelineState.ScissorRectCount, m_D3DPipelineState.ScissorRects);
		m_FlushState[eFSScissorRect] = false;
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

	if (m_FlushState[eFSRenderTarget] || m_FlushState[eFSDepthStencil] || eBackBufferCount > 1U)
	{
		m_D3DContext->OMSetRenderTargets(eRenderTargetCount, m_D3DPipelineState.RenderTarget, m_D3DPipelineState.DepthStencil);
		m_FlushState[eFSRenderTarget] = m_FlushState[eFSDepthStencil] = false;
	}
}