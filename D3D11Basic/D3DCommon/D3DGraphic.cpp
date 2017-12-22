#include "D3DGraphic.h"
#include "Utility.h"

#include <fstream>
#include <string>
#include <DirectXTK/Inc/DDSTextureLoader.h>

D3DGraphic* D3DGraphic::m_sInstance = nullptr;

D3DGraphic::D3DGraphic()
{
	memset(&m_D3DPipelineState, 0, sizeof(D3DPipelineState));
	memset(&m_FlushState, 0, sizeof(bool) * eFSCount);
	memset(&m_SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	memset(&m_DefaultViewport, 0, sizeof(D3D11_VIEWPORT));
	memset(&m_DefaultScissorRect, 0, sizeof(D3D11_RECT));
}

void D3DGraphic::CreateTexture2D(ID3D11Texture2D** ppTex, DXGI_FORMAT fmt, uint32_t width, uint32_t height,
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

	HRCheck(m_D3DDevice->CreateTexture2D(&texDesc, ((nullptr == pData) ?  nullptr : &subResData), ppTex));
}

void D3DGraphic::CreateDepthStencilView(ID3D11DepthStencilView** ppDSV, ID3D11Texture2D* ppTex, DXGI_FORMAT format, uint32_t width, uint32_t height, D3D11_DSV_DIMENSION dimension)
{
	assert(ppDSV);

	if (ppTex)
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
		HRCheck(m_D3DDevice->CreateDepthStencilView(ppTex, &dsvDesc, ppDSV));
	}
	else
	{
		Ref<ID3D11Texture2D> texture;
		CreateTexture2D(texture.Reference(), format, width, height, D3D11_BIND_DEPTH_STENCIL);
		HRCheck(m_D3DDevice->CreateDepthStencilView(texture.Ptr(), nullptr, ppDSV));
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
			m_SwapChain.Reference(),
			m_D3DDevice.Reference(),
			nullptr, 
			m_D3DContext.Reference())))
		{
			/// Create Back Buffer 
			RecreateBackBuffer();

			/// Create Depth Stencil View
			CreateDepthStencilView(m_DefaultDepthStencil.Reference(), nullptr, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);

			return;
		}
	}

	assert(!"Failed to create D3D environment!!!");
}

void D3DGraphic::CreateShaderResourceView(ID3D11ShaderResourceView** ppSRV, const char* pFileName)
{
	assert(pFileName && ppSRV); 

	std::string texFilePath = Utility::ResourceFilePath(pFileName, Utility::eTexture);
	std::wstring wtexFilePath(texFilePath.begin(), texFilePath.end());

	HRCheck(DirectX::CreateDDSTextureFromFile(m_D3DDevice.Ptr(), wtexFilePath.c_str(), nullptr, ppSRV));
}

void D3DGraphic::CreateRenderTargetView(ID3D11RenderTargetView** ppRTV, ID3D11Texture2D* pTex, const D3D11_RENDER_TARGET_VIEW_DESC *pRTVDesc)
{
	assert(ppRTV && pTex); 
	
	HRCheck(m_D3DDevice->CreateRenderTargetView(pTex, pRTVDesc, ppRTV));
}

void D3DGraphic::CreateShaderResourceView(ID3D11ShaderResourceView** ppSRV, ID3D11Resource *pSrc, DXGI_FORMAT format, D3D11_SRV_DIMENSION dimension, 
	uint32_t firstElem, uint32_t numElems)
{
	assert(ppSRV && pSrc);

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
	HRCheck(m_D3DDevice->CreateShaderResourceView(pSrc, &srvDesc, ppSRV));
}

void D3DGraphic::CreateUnorderedAccessView(ID3D11UnorderedAccessView **ppUAV, ID3D11Resource *pSrc, 
	DXGI_FORMAT format, D3D11_UAV_DIMENSION dimension, uint32_t firstElem, uint32_t numElems, uint32_t bufFlag)
{
	assert(ppUAV && pSrc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = dimension;
	desc.Buffer.FirstElement = firstElem;
	desc.Buffer.Flags = bufFlag;
	desc.Buffer.NumElements = numElems;

	HRCheck(m_D3DDevice->CreateUnorderedAccessView(pSrc, &desc, ppUAV));
}

void D3DGraphic::CreateBuffer(ID3D11Buffer** ppBuffer, uint32_t bindFlag, uint32_t byteWidth,
	D3D11_USAGE usage, const void* pBuf, uint32_t cpuAccessFlag, uint32_t miscFlag, uint32_t byteStride, uint32_t memPitch, uint32_t memSlicePitch)
{
	assert(ppBuffer);

	D3D11_BUFFER_DESC bufDesc;
	D3D11_SUBRESOURCE_DATA srcData;
	memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));
	memset(&srcData, 0, sizeof(D3D11_SUBRESOURCE_DATA));

	bufDesc.ByteWidth = byteWidth;
	bufDesc.Usage = usage;
	bufDesc.BindFlags = bindFlag;
	bufDesc.CPUAccessFlags = cpuAccessFlag;
	bufDesc.MiscFlags = miscFlag;
	bufDesc.StructureByteStride = byteStride;

	srcData.pSysMem = pBuf;
	srcData.SysMemPitch = memPitch;
	srcData.SysMemSlicePitch = memSlicePitch;

	HRCheck(m_D3DDevice->CreateBuffer(&bufDesc, ((nullptr == pBuf) ? nullptr : &srcData), ppBuffer));
}

void D3DGraphic::CreateBlendState(ID3D11BlendState** ppBlendState, D3D11_BLEND_DESC* pBlendDesc)
{
	assert(ppBlendState);

	HRCheck(m_D3DDevice->CreateBlendState(pBlendDesc, ppBlendState));
}

void D3DGraphic::CompileShaderFile(ID3DBlob** ppRes, char* pFileName, char* pEntryPoint, char* pTarget, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude)
{
	assert(ppRes && pFileName && pEntryPoint && pTarget);

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
		if (FAILED(D3DCompile(pData, fileSize, pFileName, pDefines, pIncludeInfo, pEntryPoint, pTarget, flags, 0U, ppRes, &pErrMsg)))
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

void D3DGraphic::CreateVertexShader(ID3D11VertexShader** ppVS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude)
{
	assert(ppVS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "vs_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppVS));
}

void D3DGraphic::CreatePixelShader(ID3D11PixelShader** ppPS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude)
{
	assert(ppPS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "ps_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppPS));
}

void D3DGraphic::CreateHullShader(ID3D11HullShader **ppHS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude)
{
	assert(ppHS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "hs_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppHS));
}

void D3DGraphic::CreateDomainShader(ID3D11DomainShader **ppDS, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude)
{
	assert(ppDS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "ds_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppDS));
}

void D3DGraphic::CreateGeometryShader(ID3D11GeometryShader **ppGS, char *pFileName, char *pEntryPoint, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude)
{
	assert(ppGS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "gs_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppGS));
}

void D3DGraphic::CreateComputeShader(ID3D11ComputeShader **ppCS, char *pFileName, char *pEntryPoint, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude)
{
	assert(ppCS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "cs_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppCS));
}

void D3DGraphic::CreateVertexShaderAndInputLayout(ID3D11VertexShader** ppVS, ID3D11InputLayout** ppLayout, 
	D3D11_INPUT_ELEMENT_DESC* pInputElement, uint32_t size, char* pFileName, char* pEntryPoint, const D3D_SHADER_MACRO *pDefines, ID3DInclude* pInclude)
{
	assert(ppVS);

	Ref<ID3DBlob> blob;
	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, "vs_5_0", pDefines, pInclude);
	HRCheck(m_D3DDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ppVS));
	CreateInputLayout(ppLayout, pInputElement, size, blob.Ptr());
}

///void D3DGraphic::CreateShader(eShaderType shaderType, D3DShaders &targetShaders, char *pFileName, char *pEntryPoint, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude)
///{
///	static char *const s_ShaderModel[eShaderTypeCount] = 
///	{
///		"vs_5_0",
///		"hs_5_0",
///		"ds_5_0",
///		"gs_5_0",
///		"ps_5_0",
///		"cs_5_0"
///	};
///
///	Ref<ID3DBlob> blob;
///	CompileShaderFile(blob.Reference(), pFileName, pEntryPoint, s_ShaderModel[shaderType], pDefines, pInclude);
///
///	switch (shaderType)
///	{
///	case eVertexShader:
///		assert(targetShaders.VertexShader);
///		HRCheck(m_D3DDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, targetShaders.VertexShader));
///		break;
///	case eHullShader:
///		assert(targetShaders.HullShader);
///		HRCheck(m_D3DDevice->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, targetShaders.HullShader));
///		break;
///	case eDomainShader:
///		assert(targetShaders.DomainShader);
///		HRCheck(m_D3DDevice->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, targetShaders.DomainShader));
///		break;
///	case eGeometryShader:
///		assert(targetShaders.GeometryShader);
///		HRCheck(m_D3DDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, targetShaders.GeometryShader));
///		break;
///	case ePixelShader:
///		assert(targetShaders.PixelShader);
///		HRCheck(m_D3DDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, targetShaders.PixelShader));
///		break;
///	case eComputeShader:
///		assert(targetShaders.ComputeShader);
///		HRCheck(m_D3DDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, targetShaders.ComputeShader));
///		break;
///	}
///}

void D3DGraphic::CreateRasterizerState(ID3D11RasterizerState** ppRasterizerState, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool cw, bool depthClip, bool bScissor)
{
	/// FrontCounterClockwise
	/// Determines if a triangle is front- or back-facing. If this parameter is true, 
	/// then a triangle will be considered front-facing if its vertices are counter-clockwise on the render target 
	/// and considered back-facing if they are clockwise. If this parameter is false then the opposite is true.

	assert(ppRasterizerState);

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

	HRCheck(m_D3DDevice->CreateRasterizerState(&rsDesc, ppRasterizerState));
}

void D3DGraphic::CreateDepthStencilState(ID3D11DepthStencilState** ppDepthStencilState, const D3D11_DEPTH_STENCIL_DESC* pDepthStencilStateDesc)
{
	assert(ppDepthStencilState && pDepthStencilStateDesc);

	HRCheck(m_D3DDevice->CreateDepthStencilState(pDepthStencilStateDesc, ppDepthStencilState));
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

void D3DGraphic::ClearRenderTarget(ID3D11RenderTargetView* pRenderTarget, const float* pClearColor)
{
	assert(pRenderTarget);

	float darkBlue[] = { 0.0f, 0.125f, 0.3f, 1.0f };
	pClearColor = (nullptr == pClearColor ? darkBlue : pClearColor);

	m_D3DContext->ClearRenderTargetView(pRenderTarget, pClearColor);
}

void D3DGraphic::ClearDepthStencil(ID3D11DepthStencilView* pDepthStencil, uint32_t clearFlags, float depth, uint8_t stencil)
{
	assert(pDepthStencil);

	m_D3DContext->ClearDepthStencilView(pDepthStencil, clearFlags, depth, stencil);
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
		SetRenderTarget(m_DefaultRenderTarget.Ptr());

		if (m_DefaultDepthStencil.Valid())
		{
			m_DefaultDepthStencil->Release();
		}

		CreateDepthStencilView(m_DefaultDepthStencil.Reference(), nullptr, DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);
		SetDepthStencil(m_DefaultDepthStencil.Ptr());
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

void D3DGraphic::SetInputLayout(ID3D11InputLayout* pLayout)
{
	ID3D11InputLayout* const pInputLayout = pLayout;
	if (m_D3DPipelineState.InputLayout != pInputLayout)
	{
		m_D3DPipelineState.InputLayout = pInputLayout;

		m_FlushState[eFSInputLayout] = true;
	}
}

void D3DGraphic::SetRenderTarget(ID3D11RenderTargetView* pRenderTargetView, uint32_t slot)
{
	assert(slot <= eRenderTargetCount);

	ID3D11RenderTargetView* const pRenderTarget = pRenderTargetView;
	if (m_D3DPipelineState.RenderTarget[slot] != pRenderTarget)
	{
		m_D3DPipelineState.RenderTarget[slot] = pRenderTarget;

		m_FlushState[eFSRenderTarget] = true;
	}
}

void D3DGraphic::SetDepthStencil(ID3D11DepthStencilView* pDepthStencilView)
{
	ID3D11DepthStencilView* const pDepthStencil = pDepthStencilView;
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

	if (nullptr == m_D3DPipelineState.ScissorRects)
	{
		///m_D3DPipelineState.ScissorRects = new D3D11_RECT();
	}

	if (m_D3DPipelineState.ScissorRectCount != count ||
		m_D3DPipelineState.ScissorRects != pRects)
	{
		m_D3DPipelineState.ScissorRectCount = count;

		memcpy(&m_DefaultScissorRect, pRects, sizeof(D3D11_RECT));

		m_FlushState[eFSScissorRect] = true;
	}
}

void D3DGraphic::SetSamplerStates(ID3D11SamplerState *pStates, uint32_t startSlot, uint32_t count, eShaderType type)
{
	assert(pStates && count == 1U);

	ID3D11SamplerState *samplers[1]{ pStates };
	switch (type)
	{
	case eVertexShader:
		m_D3DContext->VSSetSamplers(startSlot, count, samplers);
		break;
	case eHullShader:
		m_D3DContext->HSSetSamplers(startSlot, count, samplers);
		break;
	case eDomainShader:
		m_D3DContext->DSSetSamplers(startSlot, count, samplers);
		break;
	case eGeometryShader:
		m_D3DContext->GSSetSamplers(startSlot, count, samplers);
		break;
	case ePixelShader:
		m_D3DContext->PSSetSamplers(startSlot, count, samplers);
		break;
	case eComputeShader:
		m_D3DContext->CSSetSamplers(startSlot, count, samplers);
		break;
	}
}

void D3DGraphic::SetShaderResource(ID3D11ShaderResourceView *pSRV, uint32_t startSlot, uint32_t count, eShaderType type)
{
	assert(count == 1U);

	ID3D11ShaderResourceView *srvs[1]{ pSRV };
	switch (type)
	{
	case eVertexShader:
		m_D3DContext->VSSetShaderResources(startSlot, count, srvs);
		break;
	case eHullShader:
		m_D3DContext->HSSetShaderResources(startSlot, count, srvs);
		break;
	case eDomainShader:
		m_D3DContext->DSSetShaderResources(startSlot, count, srvs);
		break;
	case eGeometryShader:
		m_D3DContext->GSSetShaderResources(startSlot, count, srvs);
		break;
	case ePixelShader:
		m_D3DContext->PSSetShaderResources(startSlot, count, srvs);
		break;
	case eComputeShader:
		m_D3DContext->CSSetShaderResources(startSlot, count, srvs);
		break;
	}
}

void D3DGraphic::SetRasterizerState(ID3D11RasterizerState* pRS)
{
	ID3D11RasterizerState* const pRasterizerState = pRS;
	if (m_D3DPipelineState.RasterizerState != pRasterizerState)
	{
		m_D3DPipelineState.RasterizerState = pRasterizerState;

		m_FlushState[eFSRasterizerState] = true;
	}
}

void D3DGraphic::SetDepthStencilState(ID3D11DepthStencilState* pDS, uint32_t stencilRef)
{
	ID3D11DepthStencilState* const pDepthStencilState = pDS;
	if (m_D3DPipelineState.DepthStencilState != pDepthStencilState || m_D3DPipelineState.StencilRef != stencilRef)
	{
		m_D3DPipelineState.DepthStencilState = pDepthStencilState;
		m_D3DPipelineState.StencilRef = stencilRef;

		m_FlushState[eFSDepthStencilState] = true;
	}
}

void D3DGraphic::SetBlendState(ID3D11BlendState* pBS, Vec4 blendFactor, uint32_t mask)
{
	ID3D11BlendState* const pBlendState = pBS;
	if (m_D3DPipelineState.BlendState != pBlendState ||
		m_D3DPipelineState.SampleMask != mask)
	{
		m_D3DPipelineState.BlendState = pBlendState;
		memcpy(m_D3DPipelineState.BlendFactor, &blendFactor, sizeof(Vec4));
		m_D3DPipelineState.SampleMask = mask;

		m_FlushState[eFSBlendState] = true;
	}
}

void D3DGraphic::SetVertexShader(ID3D11VertexShader* pVS)
{
	assert(pVS);

	ID3D11VertexShader* const pVertexShader = pVS;
	if (m_D3DPipelineState.VertexShader != pVertexShader)
	{
		m_D3DPipelineState.VertexShader = pVertexShader;

		m_FlushState[eFSVertexShader] = true;
	}
}

void D3DGraphic::SetHullShader(ID3D11HullShader *pHS)
{
	ID3D11HullShader *const pHullShader = pHS;
	if (m_D3DPipelineState.HullShader != pHullShader)
	{
		m_D3DPipelineState.HullShader = pHullShader;

		m_FlushState[eFSHullShader] = true;
	}
}

void D3DGraphic::SetDomainShader(ID3D11DomainShader *pDS)
{
	ID3D11DomainShader *const pDomainShader = pDS;
	if (m_D3DPipelineState.DomainShader != pDomainShader)
	{
		m_D3DPipelineState.DomainShader = pDomainShader;

		m_FlushState[eFSDomainShader] = true;
	}
}

void D3DGraphic::SetPixelShader(ID3D11PixelShader* pPS)
{
	ID3D11PixelShader* const pPixelShader = pPS;
	if (m_D3DPipelineState.PixelShader != pPixelShader)
	{
		m_D3DPipelineState.PixelShader = pPixelShader;

		m_FlushState[eFSPixelShader] = true;
	}
}

void D3DGraphic::SetGeometryShader(ID3D11GeometryShader *pGS)
{
	ID3D11GeometryShader *const pGeometryShader = pGS;
	if (m_D3DPipelineState.GeometryShader != pGeometryShader)
	{
		m_D3DPipelineState.GeometryShader = pGeometryShader;

		m_FlushState[eFSGeometryShader] = true;
	}
}

void D3DGraphic::SetComputeShader(ID3D11ComputeShader *pCS)
{
	ID3D11ComputeShader *const pComputeShader = pCS;
	if (m_D3DPipelineState.ComputeShader != pComputeShader)
	{
		m_D3DPipelineState.ComputeShader = pComputeShader;

		m_FlushState[eFSComputeShader] = true;
	}
}

void D3DGraphic::SetConstantBuffer(ID3D11Buffer* pConstantBuf, uint32_t slot, eShaderType shaderType)
{
	///assert(pConstantBuf);

	switch (shaderType)
	{
	case eVertexShader:
		m_D3DContext->VSSetConstantBuffers(slot, 1U, &pConstantBuf);
		break;
	case eHullShader:
		m_D3DContext->HSSetConstantBuffers(slot, 1U, &pConstantBuf);
		break;
	case eDomainShader:
		m_D3DContext->DSSetConstantBuffers(slot, 1U, &pConstantBuf);
		break;
	case eGeometryShader:
		m_D3DContext->GSSetConstantBuffers(slot, 1U, &pConstantBuf);
		break;
	case ePixelShader:
		m_D3DContext->PSSetConstantBuffers(slot, 1U, &pConstantBuf);
		break;
	case eComputeShader:
		m_D3DContext->CSSetConstantBuffers(slot, 1U, &pConstantBuf);
		break;
	}
}

void D3DGraphic::UpdateBuffer(ID3D11Buffer* pBuffer, const void* pSource, size_t size)
{
	assert(pSource && (size > 0U));

	D3D11_MAPPED_SUBRESOURCE mapData;
	memset(&mapData, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	/// Map cannot be called with MAP_WRITE_DISCARD access
	HRCheck(m_D3DContext->Map(pBuffer, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapData));
	memcpy(mapData.pData, pSource, size);
	m_D3DContext->Unmap(pBuffer, 0U);
}

void D3DGraphic::GetBackBufferDesc(D3D11_TEXTURE2D_DESC& tex2DDesc)
{
	assert(m_SwapChain.Valid());

	Ref<ID3D11Texture2D> backBuffer;
	HRCheck(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.Reference()));

	backBuffer->GetDesc(&tex2DDesc);
}

///void D3DGraphic::ResolveSubResource(ID3D11Texture2D* pDstResource, ID3D11Texture2D* pSrcResource, uint32_t dstCount, uint32_t srcCount, DXGI_FORMAT fmt)
///{
///	assert(pDstResource && pSrcResource);
///
///	m_D3DContext->ResolveSubresource(pDstResource, dstCount, pSrcResource, srcCount, fmt);
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

	m_D3DContext->CopySubresourceRegion(pSrc, 0U, 0U, 0U, 0U, pDst, 0U, &box);
	HRCheck(m_D3DContext->Map(pSrc, 0U, D3D11_MAP_READ, 0U, &mappedSrc));
	memcpy(pDstMem, mappedSrc.pData, memSize);
	m_D3DContext->Unmap(pSrc, 0U);
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
}

void D3DGraphic::DrawQuad(float top, float left, float width, float height)
{
	static uint32_t indices[6] = { 0U, 1U, 2U, 0U, 2U, 3U };
	static Ref<ID3D11Buffer> ib;
	if (!ib.Valid())
	{
		CreateIndexBuffer(ib.Reference(), sizeof(uint32_t) * 6U, D3D11_USAGE_IMMUTABLE, indices);
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
	if (!vb.Valid())
	{
		CreateVertexBuffer(vb.Reference(), sizeof(Vertex) * 4, D3D11_USAGE_DYNAMIC, vertices, D3D11_CPU_ACCESS_WRITE);
	}

	SetVertexBuffer(vb.Ptr(), sizeof(Vertex), 0U);

	SetIndexBuffer(ib.Ptr(), DXGI_FORMAT_R32_UINT);

	DrawIndexed(6U, 0U, 0U);
}

void D3DGraphic::FlushState()
{
	if (!m_D3DContext.Valid())
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

	if (m_FlushState[eFSHullShader])
	{
		m_D3DContext->HSSetShader(m_D3DPipelineState.HullShader, nullptr, 0U);
		m_FlushState[eFSHullShader] = false;
	}

	if (m_FlushState[eFSDomainShader])
	{
		m_D3DContext->DSSetShader(m_D3DPipelineState.DomainShader, nullptr, 0U);
		m_FlushState[eFSDomainShader] = false;
	}

	if (m_FlushState[eFSGeometryShader])
	{
		m_D3DContext->GSSetShader(m_D3DPipelineState.GeometryShader, nullptr, 0U);
		m_FlushState[eFSGeometryShader] = false;
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
		m_D3DContext->RSSetScissorRects(m_D3DPipelineState.ScissorRectCount, /*m_D3DPipelineState.ScissorRects*/&m_DefaultScissorRect);
		m_FlushState[eFSScissorRect] = false;
	}

	/// Pixel Shader
	if (m_FlushState[eFSPixelShader])
	{
		m_D3DContext->PSSetShader(m_D3DPipelineState.PixelShader, nullptr, 0U);
		m_FlushState[eFSPixelShader] = false;
	}

	if (m_FlushState[eFSComputeShader])
	{
		m_D3DContext->CSSetShader(m_D3DPipelineState.ComputeShader, nullptr, 0U);
		m_FlushState[eFSComputeShader] = false;
	}

	/// Output Merge
	if (m_FlushState[eFSBlendState])
	{
		m_D3DContext->OMSetBlendState(m_D3DPipelineState.BlendState, m_D3DPipelineState.BlendFactor, m_D3DPipelineState.SampleMask);
		m_FlushState[eFSBlendState] = false;
	}

	if (m_FlushState[eFSDepthStencilState])
	{
		m_D3DContext->OMSetDepthStencilState(m_D3DPipelineState.DepthStencilState, m_D3DPipelineState.StencilRef);
		m_FlushState[eFSDepthStencilState] = false;
	}

	if (m_FlushState[eFSRenderTarget] || m_FlushState[eFSDepthStencil] || eBackBufferCount > 1U)
	{
		m_D3DContext->OMSetRenderTargets(eRenderTargetCount, m_D3DPipelineState.RenderTarget, m_D3DPipelineState.DepthStencil);
		m_FlushState[eFSRenderTarget] = m_FlushState[eFSDepthStencil] = false;
	}
}