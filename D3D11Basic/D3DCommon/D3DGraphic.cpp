#include "D3DGraphic.h"
#include "Utility.h"

#include <fstream>
#include <string>
#include <DirectXTK/Inc/DDSTextureLoader.h>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

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
			m_SwapChain.Reference(),
			m_D3DDevice.Reference(),
			nullptr,
			m_IMContent.Reference())))
		{
			/// Create Back Buffer 
			RecreateBackBuffer();

			/// Create Depth Stencil View
			CreateDepthStencilView(m_DefaultDepthStencil, nullptr, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);

			return;
		}
	}

	assert(!"Failed to create D3D environment!!!");
}

void D3DGraphic::CreateTexture1D(__out Ref<ID3D11Texture1D> &rTex,
	DXGI_FORMAT fmt,
	uint32_t width,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t arraySize,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	D3D11_USAGE usage,
	const void *pData,
	uint32_t memPitch)
{
	assert(!rTex.Valid());

	D3D11_TEXTURE1D_DESC texDesc = {};
	texDesc.Format = fmt;
	texDesc.Width = width;
	texDesc.BindFlags = bindFlags;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = arraySize;
	texDesc.CPUAccessFlags = cpuFlags;
	texDesc.MiscFlags = miscFlags;
	texDesc.Usage = usage;

	D3D11_SUBRESOURCE_DATA subResData;
	memset(&subResData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = pData;
	subResData.SysMemPitch = memPitch;
	subResData.SysMemSlicePitch = 0U; 	/// System-memory-slice pitch is only used for 3D texture data as it has no meaning for the other resource types

	HRCheck(m_D3DDevice->CreateTexture1D(&texDesc, ((nullptr == pData) ? nullptr : &subResData), rTex.Reference()));
}

void D3DGraphic::CreateTexture2D(Ref<ID3D11Texture2D> &rTex,
	DXGI_FORMAT fmt, 
	uint32_t width, 
	uint32_t height,
	uint32_t bindFlags, 
	uint32_t mipLevels, 
	uint32_t arraySize, 
	uint32_t cpuFlags, 
	uint32_t miscFlags, 
	D3D11_USAGE usage, 
	const void *pData,
	uint32_t memPitch)
{
	assert(!rTex.Valid());

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Format = fmt;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.BindFlags = bindFlags;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = arraySize;
	texDesc.CPUAccessFlags = cpuFlags;
	texDesc.SampleDesc.Count = 1U;
	texDesc.SampleDesc.Quality = 0U;
	texDesc.MiscFlags = miscFlags;
	texDesc.Usage = usage;

	D3D11_SUBRESOURCE_DATA subResData;
	memset(&subResData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = pData;
	subResData.SysMemPitch = memPitch;
	subResData.SysMemSlicePitch = 0U; 	/// System-memory-slice pitch is only used for 3D texture data as it has no meaning for the other resource types

	HRCheck(m_D3DDevice->CreateTexture2D(&texDesc, ((nullptr == pData) ?  nullptr : &subResData), rTex.Reference()));
}

void D3DGraphic::CreateShaderResourceView(Ref<ID3D11ShaderResourceView> &rSRV, const char *pFileName)
{
	assert(pFileName && !rSRV.Valid());

	std::string texFilePath = Utility::ResourceFilePath(pFileName, Utility::eTexture);
	std::wstring wtexFilePath(texFilePath.begin(), texFilePath.end());

	HRCheck(DirectX::CreateDDSTextureFromFile(m_D3DDevice.Ptr(), wtexFilePath.c_str(), nullptr, rSRV.Reference()));
}

void D3DGraphic::CreateShaderResourceView(Ref<ID3D11ShaderResourceView> &rSRV, 
	ID3D11Resource *pSrc, 
	DXGI_FORMAT format, 
	D3D11_SRV_DIMENSION dimension, 
	uint32_t firstElem, 
	uint32_t numElems)
{
	assert(pSrc && !rSRV.Valid());

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
	case D3D11_SRV_DIMENSION_BUFFER:
		srvDesc.Buffer.FirstElement = firstElem;
		srvDesc.Buffer.NumElements = numElems;
	default:
		assert("Unsupport yet!!");
		break;
	}
	HRCheck(m_D3DDevice->CreateShaderResourceView(pSrc, &srvDesc, rSRV.Reference()));
}

void D3DGraphic::CreateUnorderedAccessView(Ref<ID3D11UnorderedAccessView> &rUAV, 
	ID3D11Resource *pSrc, 
	DXGI_FORMAT format, 
	D3D11_UAV_DIMENSION dimension, 
	uint32_t firstElem, 
	uint32_t numElems, 
	uint32_t flags)
{
	assert(pSrc && !rUAV.Valid());

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = dimension;
	desc.Buffer.FirstElement = firstElem;
	desc.Buffer.Flags = flags;
	desc.Buffer.NumElements = numElems;

	HRCheck(m_D3DDevice->CreateUnorderedAccessView(pSrc, &desc, rUAV.Reference()));
}

void D3DGraphic::CreateRenderTargetView(Ref<ID3D11RenderTargetView> &rRTV, ID3D11Resource *pSrc, const D3D11_RENDER_TARGET_VIEW_DESC *pRTVDesc)
{
	assert(pSrc && !rRTV.Valid());

	/// Set pRTVDesc to NULL to create a view that accesses all of the subresources in mipmap level 0
	HRCheck(m_D3DDevice->CreateRenderTargetView(pSrc, pRTVDesc, rRTV.Reference()));
}

void D3DGraphic::CreateDepthStencilView(Ref<ID3D11DepthStencilView> &rDSV,
	ID3D11Resource* pSrc,
	DXGI_FORMAT format,
	uint32_t width,
	uint32_t height,
	D3D11_DSV_DIMENSION dimension)
{
	assert(!rDSV.Valid());

	if (pSrc)
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
		HRCheck(m_D3DDevice->CreateDepthStencilView(pSrc, &dsvDesc, rDSV.Reference()));
	}
	else
	{
		Ref<ID3D11Texture2D> texture;
		CreateTexture2D(texture, format, width, height, D3D11_BIND_DEPTH_STENCIL);
		HRCheck(m_D3DDevice->CreateDepthStencilView(texture.Ptr(), nullptr, rDSV.Reference()));
	}
}

void D3DGraphic::CreateBuffer(Ref<ID3D11Buffer> &rBuf,
	uint32_t bindFlags,
	uint32_t byteWidth,
	D3D11_USAGE usage,
	const void *pData,
	uint32_t cpuAccessFlags,
	uint32_t miscFlags,
	uint32_t byteStride,
	uint32_t memPitch)
{
	assert(!rBuf.Valid());

	D3D11_BUFFER_DESC bufDesc;
	D3D11_SUBRESOURCE_DATA srcData;
	memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));
	memset(&srcData, 0, sizeof(D3D11_SUBRESOURCE_DATA));

	bufDesc.ByteWidth = byteWidth;
	bufDesc.Usage = usage;
	bufDesc.BindFlags = bindFlags;
	bufDesc.CPUAccessFlags = cpuAccessFlags;
	bufDesc.MiscFlags = miscFlags;
	bufDesc.StructureByteStride = byteStride;

	srcData.pSysMem = pData;
	srcData.SysMemPitch = memPitch;
	srcData.SysMemSlicePitch = 0U;

	HRCheck(m_D3DDevice->CreateBuffer(&bufDesc, ((nullptr == pData) ? nullptr : &srcData), rBuf.Reference()));
}

D3DGraphic::D3DShaders D3DGraphic::CreateShader(eShaderType shaderType, const char *pFileName, const char *pEntryPoint, const D3D_SHADER_MACRO *pMacros, ID3DInclude *pInclude)
{
	static const char *const s_ShaderModel[eShaderTypeCount] = 
	{
		"vs_5_0",
		"hs_5_0",
		"ds_5_0",
		"gs_5_0",
		"ps_5_0",
		"cs_5_0"
	};

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob, pFileName, pEntryPoint, s_ShaderModel[shaderType], pMacros, pInclude);

	D3DShaders shaders;

	switch (shaderType)
	{
	case eVertexShader:
		HRCheck(m_D3DDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shaders.VertexShader.Reference()));
		break;
	case eHullShader:
		HRCheck(m_D3DDevice->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shaders.HullShader.Reference()));
		break;
	case eDomainShader:
		HRCheck(m_D3DDevice->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shaders.DomainShader.Reference()));
		break;
	case eGeometryShader:
		HRCheck(m_D3DDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shaders.GeometryShader.Reference()));
		break;
	case ePixelShader:
		HRCheck(m_D3DDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shaders.PixelShader.Reference()));
		break;
	case eComputeShader:
		HRCheck(m_D3DDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shaders.ComputeShader.Reference()));
		break;
	}

	return shaders;
}

void D3DGraphic::CompileShaderFile(Ref<ID3DBlob> &rBlob,
	const char *pFileName, 
	const char *pEntryPoint, 
	const char *pTarget, 
	const D3D_SHADER_MACRO* pDefines, 
	ID3DInclude* pInclude)
{
	assert(!rBlob.Valid() && pFileName && pEntryPoint && pTarget);

	std::string shaderFileDir = Utility::ResourceFileDirectory(Utility::eShader);
	std::string shaderFilePath = shaderFileDir + pFileName;

	std::ifstream file(shaderFilePath, std::ios::in);
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
		char workingDir[MAX_PATH] = { 0 };
		::GetCurrentDirectoryA(MAX_PATH, workingDir);

		::SetCurrentDirectoryA(shaderFileDir.c_str());
		ID3DBlob* pErrMsg = nullptr;
		ID3DInclude* pIncludeInfo = (nullptr == pInclude ? D3D_COMPILE_STANDARD_FILE_INCLUDE : pInclude);
		if (FAILED(D3DCompile(pData, fileSize, pFileName, pDefines, pIncludeInfo, pEntryPoint, pTarget, flags, 0U, rBlob.Reference(), &pErrMsg)))
		{
			OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
			assert(!"Shader compile failed!!!");
		}
		::SetCurrentDirectoryA(workingDir);

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
	Ref<ID3D11Texture2D> backBuffer;
	HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.Reference()));
	HRCheck(m_D3DDevice->CreateRenderTargetView(backBuffer.Ptr(), nullptr, m_DefaultRenderTarget.Reference()));
}

void D3DGraphic::CreateRasterizerState(Ref<ID3D11RasterizerState> &rRasterizerState, 
	D3D11_FILL_MODE fillMode, 
	D3D11_CULL_MODE cullMode, 
	bool cw, 
	bool depthClip, 
	bool bScissor)
{
	/// FrontCounterClockwise
	/// Determines if a triangle is front- or back-facing. If this parameter is true, 
	/// then a triangle will be considered front-facing if its vertices are counter-clockwise on the render target 
	/// and considered back-facing if they are clockwise. If this parameter is false then the opposite is true.

	assert(!rRasterizerState.Valid());

	D3D11_RASTERIZER_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = fillMode;
	rsDesc.CullMode = cullMode;
	rsDesc.FrontCounterClockwise = cw;
	rsDesc.DepthClipEnable = depthClip;
	rsDesc.MultisampleEnable = false;
	rsDesc.ScissorEnable = bScissor;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;

	HRCheck(m_D3DDevice->CreateRasterizerState(&rsDesc, rRasterizerState.Reference()));
}

void D3DGraphic::CreateRandomTexture1D(Ref<ID3D11ShaderResourceView> &rSRV)
{
	assert(!rSRV.Valid());

	static const uint32_t size = 1024U;
	Vec4 random[size] = {};

	for (uint32_t i = 0U; i < size; ++i)
	{
		random[i].x = Math::RandF(-1.0f, 1.0f);
		random[i].y = Math::RandF(-1.0f, 1.0f);
		random[i].z = Math::RandF(-1.0f, 1.0f);
		random[i].w = Math::RandF(-1.0f, 1.0f);
	}

	Ref<ID3D11Texture1D> tex;
	CreateTexture1D(tex, DXGI_FORMAT_R32G32B32A32_FLOAT, size, D3D11_BIND_SHADER_RESOURCE, 1U, 1U, 0U, 0U, 
		D3D11_USAGE_IMMUTABLE, random, size * sizeof(Vec4));

	CreateShaderResourceView(rSRV, tex.Ptr(), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE1D);
}

void D3DGraphic::CreateSamplerState(__out Ref<ID3D11SamplerState> &rSamplerState,
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressMode,
	float LODBias,
	D3D11_COMPARISON_FUNC,
	const Vec4 &borderClr,
	float minLOD,
	float maxLOD)
{
	assert(!rSamplerState.Valid());

	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_SAMPLER_DESC));
	desc.Filter = filter;
	desc.AddressU = desc.AddressV = desc.AddressW = addressMode;
	desc.MipLODBias = LODBias;
	desc.MaxAnisotropy = 0U;
	desc.MinLOD = minLOD;
	desc.MaxLOD = maxLOD;
	memcpy(desc.BorderColor, &borderClr, sizeof(Vec4));

	HRCheck(m_D3DDevice->CreateSamplerState(&desc, rSamplerState.Reference()));
}

void D3DGraphic::CreateBlendState(__out Ref<ID3D11BlendState> &rBlendState,
	bool bBlend,
	bool bAlphaToCoverage,
	D3D11_BLEND srcColor,
	D3D11_BLEND dstColor,
	D3D11_BLEND_OP colorOp,
	D3D11_BLEND srcAlpha,
	D3D11_BLEND dstAlpha,
	D3D11_BLEND_OP alphaOp,
	D3D11_COLOR_WRITE_ENABLE writeMask)
{
	assert(!rBlendState.Valid());

	D3D11_BLEND_DESC desc{ 0 };
	desc.AlphaToCoverageEnable = bAlphaToCoverage;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = bBlend;
	desc.RenderTarget[0].SrcBlend = srcColor;
	desc.RenderTarget[0].DestBlend = dstColor;
	desc.RenderTarget[0].BlendOp = colorOp;
	
	desc.RenderTarget[0].SrcBlendAlpha = srcAlpha;
	desc.RenderTarget[0].DestBlendAlpha = dstAlpha;
	desc.RenderTarget[0].BlendOpAlpha = alphaOp;

	desc.RenderTarget[0].RenderTargetWriteMask = (uint8_t)writeMask;

	HRCheck(m_D3DDevice->CreateBlendState(&desc, rBlendState.Reference()));
}

void D3DGraphic::ResizeBackBuffer(uint32_t width, uint32_t height)
{
	if (!m_SwapChain.Valid())
	{
		return;
	}

	assert((width > 0U) && (height > 0U));

	if (width != m_SwapChainDesc.BufferDesc.Width || height != m_SwapChainDesc.BufferDesc.Height)
	{
		m_SwapChainDesc.BufferDesc.Width = width;
		m_SwapChainDesc.BufferDesc.Height = height;

		if (m_DefaultRenderTarget.Valid())
		{
			m_DefaultRenderTarget->Release();
		}
		m_SwapChain->ResizeBuffers(m_SwapChainDesc.BufferCount, width, height, m_SwapChainDesc.BufferDesc.Format, m_SwapChainDesc.Flags);
		RecreateBackBuffer();
		SetRenderTarget(m_DefaultRenderTarget);

		if (m_DefaultDepthStencil.Valid())
		{
			static Ref<ID3D11DepthStencilView> s_NullDepthStencilView;
			m_DefaultDepthStencil = s_NullDepthStencilView;
		}

		CreateDepthStencilView(m_DefaultDepthStencil, nullptr, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);
		SetDepthStencil(m_DefaultDepthStencil);
	}
}

void D3DGraphic::SetVertexBuffer(const Ref<ID3D11Buffer> &rVertexBuffer, uint32_t stride, uint32_t offset, uint32_t index)
{
	assert(index <= eVertexStreamCount);

	if (m_PipelineState.VertexBuffer.Buffers[index] != rVertexBuffer.Ptr() ||
		m_PipelineState.VertexBuffer.Stride[index] != stride ||
		m_PipelineState.VertexBuffer.Offset[index] != offset)
	{
		m_PipelineState.VertexBuffer.Buffers[index] = rVertexBuffer.Ptr();
		m_PipelineState.VertexBuffer.Stride[index] = stride;
		m_PipelineState.VertexBuffer.Offset[index] = offset;

		m_FlushState[eFSVertexBuffer] = true;
	}
}

void D3DGraphic::SetIndexBuffer(const Ref<ID3D11Buffer> &rIndexBuffer, DXGI_FORMAT format, uint32_t offset)
{
	if (m_PipelineState.IndexBuffer.Buffers != rIndexBuffer.Ptr() ||
		m_PipelineState.IndexBuffer.Format != format ||
		m_PipelineState.IndexBuffer.Offset != offset)
	{
		m_PipelineState.IndexBuffer.Buffers = rIndexBuffer.Ptr();
		m_PipelineState.IndexBuffer.Format = format;
		m_PipelineState.IndexBuffer.Offset = offset;

		m_FlushState[eFSIndexBuffer] = true;
	}
}

void D3DGraphic::SetInputLayout(const Ref<ID3D11InputLayout> &rInputLayout)
{
	ID3D11InputLayout *const pInputLayout = rInputLayout.Ptr();
	if (m_PipelineState.InputLayout != pInputLayout)
	{
		m_PipelineState.InputLayout = pInputLayout;

		m_FlushState[eFSInputLayout] = true;
	}
}

void D3DGraphic::SetRenderTarget(const Ref<ID3D11RenderTargetView> &rRenderTarget, uint32_t slot)
{
	assert(slot <= eRenderTargetCount);

	ID3D11RenderTargetView *const pRenderTarget = rRenderTarget.Ptr();
	if (m_PipelineState.RenderTarget[slot] != pRenderTarget)
	{
		m_PipelineState.RenderTarget[slot] = pRenderTarget;

		m_FlushState[eFSRenderTarget] = true;
	}
}

void D3DGraphic::SetDepthStencil(const Ref<ID3D11DepthStencilView> &rDepthStencilView)
{
	ID3D11DepthStencilView *const pDepthStencil = rDepthStencilView.Ptr();
	if (m_PipelineState.DepthStencil != pDepthStencil)
	{
		m_PipelineState.DepthStencil = pDepthStencil;

		m_FlushState[eFSDepthStencil] = true;
	}
}

void D3DGraphic::SetViewports(const D3D11_VIEWPORT *pViewports, uint32_t count)
{
	assert(count <= 1U && "Unsupport yet!!!");

	if (m_PipelineState.ViewportCount != count ||
		m_PipelineState.Viewports != pViewports)
	{
		m_PipelineState.ViewportCount = count;
		m_PipelineState.Viewports = pViewports;

		memcpy(&m_DefaultViewport, pViewports, sizeof(D3D11_VIEWPORT));

		m_FlushState[eFSViewports] = true;
	}
}

void D3DGraphic::SetScissorRects(const D3D11_RECT *pRects, uint32_t count)
{
	assert(count <= 1U && "Unsupport yet!!!");

	if (nullptr == m_PipelineState.ScissorRects)
	{
		///m_PipelineState.ScissorRects = new D3D11_RECT();
		///assert(0);
	}

	if (m_PipelineState.ScissorRectCount != count ||
		m_PipelineState.ScissorRects != pRects)
	{
		m_PipelineState.ScissorRectCount = count;

		memcpy(&m_DefaultScissorRect, pRects, sizeof(D3D11_RECT));

		m_FlushState[eFSScissorRect] = true;
	}
}

void D3DGraphic::SetSamplerStates(const Ref<ID3D11SamplerState> &rSamplerStates, uint32_t startSlot, eShaderType shaderType)
{
	ID3D11SamplerState *samplers[1]{ rSamplerStates.Ptr() };
	switch (shaderType)
	{
	case eVertexShader:
		m_IMContent->VSSetSamplers(startSlot, 1U, samplers);
		break;
	case eHullShader:
		m_IMContent->HSSetSamplers(startSlot, 1U, samplers);
		break;
	case eDomainShader:
		m_IMContent->DSSetSamplers(startSlot, 1U, samplers);
		break;
	case eGeometryShader:
		m_IMContent->GSSetSamplers(startSlot, 1U, samplers);
		break;
	case ePixelShader:
		m_IMContent->PSSetSamplers(startSlot, 1U, samplers);
		break;
	case eComputeShader:
		m_IMContent->CSSetSamplers(startSlot, 1U, samplers);
		break;
	}
}

void D3DGraphic::SetShaderResource(const Ref<ID3D11ShaderResourceView> &rSRV, uint32_t startSlot, eShaderType type)
{
	ID3D11ShaderResourceView *const ppSRV[1]{ rSRV.Ptr() };
	switch (type)
	{
	case eVertexShader:
		m_IMContent->VSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eHullShader:
		m_IMContent->HSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eDomainShader:
		m_IMContent->DSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eGeometryShader:
		m_IMContent->GSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case ePixelShader:
		m_IMContent->PSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	case eComputeShader:
		m_IMContent->CSSetShaderResources(startSlot, 1U, ppSRV);
		break;
	}
}

void D3DGraphic::SetStreamOut(const Ref<ID3D11Buffer> &rBuf, uint32_t offset)
{
	if (rBuf.Ptr() != m_PipelineState.StreamOut[0] || offset != m_PipelineState.StreamOutOffset[0])
	{
		m_PipelineState.StreamOut[0] = rBuf.Ptr();
		m_PipelineState.StreamOutOffset[0] = offset;

		m_FlushState[eFSStreamOut] = true;
	}
}

void D3DGraphic::SetRasterizerState(const Ref<ID3D11RasterizerState> &rRasterizerState)
{
	ID3D11RasterizerState *const pRasterizerState = rRasterizerState.Ptr();
	if (m_PipelineState.RasterizerState != pRasterizerState)
	{
		m_PipelineState.RasterizerState = pRasterizerState;

		m_FlushState[eFSRasterizerState] = true;
	}
}

void D3DGraphic::SetDepthStencilState(const Ref<ID3D11DepthStencilState> &rDepthStencilState, uint32_t stencilRef)
{
	ID3D11DepthStencilState *const pDepthStencilState = rDepthStencilState.Ptr();
	if (m_PipelineState.DepthStencilState != pDepthStencilState || m_PipelineState.StencilRef != stencilRef)
	{
		m_PipelineState.DepthStencilState = pDepthStencilState;
		m_PipelineState.StencilRef = stencilRef;

		m_FlushState[eFSDepthStencilState] = true;
	}
}

void D3DGraphic::SetBlendState(const Ref<ID3D11BlendState> &rBlendState, Vec4 blendFactor, uint32_t mask)
{
	ID3D11BlendState *const pBlendState = rBlendState.Ptr();
	if (m_PipelineState.BlendState != pBlendState ||
		m_PipelineState.SampleMask != mask)
	{
		m_PipelineState.BlendState = pBlendState;
		memcpy(m_PipelineState.BlendFactor, &blendFactor, sizeof(Vec4));
		m_PipelineState.SampleMask = mask;

		m_FlushState[eFSBlendState] = true;
	}
}

void D3DGraphic::SetVertexShader(const Ref<ID3D11VertexShader> &rVS)
{
	assert(rVS.Valid());

	ID3D11VertexShader *const pVertexShader = rVS.Ptr();
	if (m_PipelineState.VertexShader != pVertexShader)
	{
		m_PipelineState.VertexShader = pVertexShader;

		m_FlushState[eFSVertexShader] = true;
	}
}

void D3DGraphic::SetHullShader(const Ref<ID3D11HullShader> &rHS)
{
	ID3D11HullShader *const pHullShader = rHS.Ptr();
	if (m_PipelineState.HullShader != pHullShader)
	{
		m_PipelineState.HullShader = pHullShader;

		m_FlushState[eFSHullShader] = true;
	}
}

void D3DGraphic::SetDomainShader(const Ref<ID3D11DomainShader> &rDS)
{
	ID3D11DomainShader *const pDomainShader = rDS.Ptr();
	if (m_PipelineState.DomainShader != pDomainShader)
	{
		m_PipelineState.DomainShader = pDomainShader;

		m_FlushState[eFSDomainShader] = true;
	}
}

void D3DGraphic::SetPixelShader(const Ref<ID3D11PixelShader> &rPS)
{
	ID3D11PixelShader *const pPixelShader = rPS.Ptr();
	if (m_PipelineState.PixelShader != pPixelShader)
	{
		m_PipelineState.PixelShader = pPixelShader;

		m_FlushState[eFSPixelShader] = true;
	}
}

void D3DGraphic::SetGeometryShader(const Ref<ID3D11GeometryShader> &rGS)
{
	ID3D11GeometryShader *const pGeometryShader = rGS.Ptr();
	if (m_PipelineState.GeometryShader != pGeometryShader)
	{
		m_PipelineState.GeometryShader = pGeometryShader;

		m_FlushState[eFSGeometryShader] = true;
	}
}

void D3DGraphic::SetComputeShader(const Ref<ID3D11ComputeShader> &rCS)
{
	ID3D11ComputeShader *const pComputeShader = rCS.Ptr();
	if (m_PipelineState.ComputeShader != pComputeShader)
	{
		m_PipelineState.ComputeShader = pComputeShader;

		m_FlushState[eFSComputeShader] = true;
	}
}

void D3DGraphic::SetConstantBuffer(const Ref<ID3D11Buffer> &rConstantBuf, uint32_t slot, eShaderType shaderType)
{
	ID3D11Buffer *const ppBuf[1] = { rConstantBuf.Ptr() };
	switch (shaderType)
	{
	case eVertexShader:
		m_IMContent->VSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eHullShader:
		m_IMContent->HSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eDomainShader:
		m_IMContent->DSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eGeometryShader:
		m_IMContent->GSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case ePixelShader:
		m_IMContent->PSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	case eComputeShader:
		m_IMContent->CSSetConstantBuffers(slot, 1U, ppBuf);
		break;
	}
}

void D3DGraphic::UpdateBuffer(Ref<ID3D11Buffer> &rBuffer, const void *pSource, size_t size)
{
	assert(pSource && (size > 0U) && rBuffer.Valid());

	D3D11_MAPPED_SUBRESOURCE mapData;
	memset(&mapData, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	/// Map cannot be called with MAP_WRITE_DISCARD access
	HRCheck(m_IMContent->Map(rBuffer.Ptr(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapData));
	memcpy(mapData.pData, pSource, size);
	m_IMContent->Unmap(rBuffer.Ptr(), 0U);
}

///void D3DGraphic::GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc)
///{
///	assert(m_SwapChain.Valid());
///
///	Ref<ID3D11Texture2D> backBuffer;
///	HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.Reference()));
///
///	backBuffer->GetDesc(&tex2DDesc);
///}

///void D3DGraphic::ResolveSubResource(ID3D11Texture2D* pDstResource, ID3D11Texture2D* pSrcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt)
///{
///	assert(pDstResource && pSrcResource);
///
///	m_IMContent->ResolveSubresource(pDstResource, dstCount, pSrcResource, srcCount, fmt);
///}

void D3DGraphic::CopyBuffer(ID3D11Resource *pSrc, ID3D11Resource *pDst, void *pDstMem, size_t memSize, ::RECT &rect)
{
	assert(pSrc && pDst && memSize);

	D3D11_BOX box = { 0U };
	box.left = (uint32_t)rect.left;
	box.right = (uint32_t)rect.right;
	box.top = rect.top;
	box.bottom = rect.bottom;
	box.front = 0U;
	box.back = 1U;

	D3D11_MAPPED_SUBRESOURCE mappedSrc = { 0U };

	m_IMContent->CopySubresourceRegion(pSrc, 0U, 0U, 0U, 0U, pDst, 0U, &box);
	HRCheck(m_IMContent->Map(pSrc, 0U, D3D11_MAP_READ, 0U, &mappedSrc));
	memcpy(pDstMem, mappedSrc.pData, memSize);
	m_IMContent->Unmap(pSrc, 0U);
}

void D3DGraphic::Draw(uint32_t vertexCount, uint32_t startIndex, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

	m_IMContent->Draw(vertexCount, startIndex);
}

void D3DGraphic::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

	m_IMContent->DrawIndexed(indexCount, startIndex, offset);
}

void D3DGraphic::DrawQuad(float /*top*/, float /*left*/, float /*width*/, float /*height*/)
{
	assert(0);
}

void D3DGraphic::DrawAuto(D3D_PRIMITIVE_TOPOLOGY prim)
{
	if (m_PipelineState.PrimitiveTopology != prim)
	{
		m_PipelineState.PrimitiveTopology = prim;
		m_FlushState[eFSPrimitiveTopology] = true;
	}

	FlushState();

	m_IMContent->DrawAuto();
}

void D3DGraphic::FlushState()
{
	if (!m_IMContent.Valid())
	{
		return;
	}

	/// Input Assembler
	if (m_FlushState[eFSInputLayout])
	{
		m_IMContent->IASetInputLayout(m_PipelineState.InputLayout);
		m_FlushState[eFSInputLayout] = false;
	}

	if (m_FlushState[eFSStreamOut])
	{
		m_IMContent->SOSetTargets(1U, m_PipelineState.StreamOut, m_PipelineState.StreamOutOffset);
	}

	if (m_FlushState[eFSVertexBuffer])
	{
		m_IMContent->IASetVertexBuffers(0, eVertexStreamCount, m_PipelineState.VertexBuffer.Buffers,
			m_PipelineState.VertexBuffer.Stride, m_PipelineState.VertexBuffer.Offset);
		m_FlushState[eFSVertexBuffer] = false;
	}

	if (m_FlushState[eFSIndexBuffer])
	{
		m_IMContent->IASetIndexBuffer(m_PipelineState.IndexBuffer.Buffers, m_PipelineState.IndexBuffer.Format,
			m_PipelineState.IndexBuffer.Offset);
		m_FlushState[eFSIndexBuffer] = false;
	}

	if (m_FlushState[eFSPrimitiveTopology])
	{
		m_IMContent->IASetPrimitiveTopology(m_PipelineState.PrimitiveTopology);
		m_FlushState[eFSPrimitiveTopology] = false;
	}

	/// VS->HS->DS->GS
	if (m_FlushState[eFSVertexShader])
	{
		m_IMContent->VSSetShader(m_PipelineState.VertexShader, nullptr, 0U);
		m_FlushState[eFSVertexShader] = false;
	}

	if (m_FlushState[eFSHullShader])
	{
		m_IMContent->HSSetShader(m_PipelineState.HullShader, nullptr, 0U);
		m_FlushState[eFSHullShader] = false;
	}

	if (m_FlushState[eFSDomainShader])
	{
		m_IMContent->DSSetShader(m_PipelineState.DomainShader, nullptr, 0U);
		m_FlushState[eFSDomainShader] = false;
	}

	if (m_FlushState[eFSGeometryShader])
	{
		m_IMContent->GSSetShader(m_PipelineState.GeometryShader, nullptr, 0U);
		m_FlushState[eFSGeometryShader] = false;
	}

	/// Rasterizer
	if (m_FlushState[eFSRasterizerState])
	{
		m_IMContent->RSSetState(m_PipelineState.RasterizerState);
		m_FlushState[eFSRasterizerState] = false;
	}

	if (m_FlushState[eFSViewports])
	{
		m_IMContent->RSSetViewports(m_PipelineState.ViewportCount, &m_DefaultViewport);
		m_FlushState[eFSViewports] = false;
	}

	if (m_FlushState[eFSScissorRect])
	{
		m_IMContent->RSSetScissorRects(m_PipelineState.ScissorRectCount, /*m_PipelineState.ScissorRects*/&m_DefaultScissorRect);
		m_FlushState[eFSScissorRect] = false;
	}

	/// Pixel Shader
	if (m_FlushState[eFSPixelShader])
	{
		m_IMContent->PSSetShader(m_PipelineState.PixelShader, nullptr, 0U);
		m_FlushState[eFSPixelShader] = false;
	}

	if (m_FlushState[eFSComputeShader])
	{
		m_IMContent->CSSetShader(m_PipelineState.ComputeShader, nullptr, 0U);
		m_FlushState[eFSComputeShader] = false;
	}

	/// Output Merge
	if (m_FlushState[eFSBlendState])
	{
		m_IMContent->OMSetBlendState(m_PipelineState.BlendState, m_PipelineState.BlendFactor, m_PipelineState.SampleMask);
		m_FlushState[eFSBlendState] = false;
	}

	if (m_FlushState[eFSDepthStencilState])
	{
		m_IMContent->OMSetDepthStencilState(m_PipelineState.DepthStencilState, m_PipelineState.StencilRef);
		m_FlushState[eFSDepthStencilState] = false;
	}

	if (m_FlushState[eFSRenderTarget] || m_FlushState[eFSDepthStencil] || eBackBufferCount > 1U)
	{
		m_IMContent->OMSetRenderTargets(eRenderTargetCount, m_PipelineState.RenderTarget, m_PipelineState.DepthStencil);
		m_FlushState[eFSRenderTarget] = m_FlushState[eFSDepthStencil] = false;
	}
}