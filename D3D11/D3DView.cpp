#include "D3DView.h"
#include "D3DTexture.h"
#include "D3DEngine.h"
#include "System.h"

#include <DirectXTK/Inc/DDSTextureLoader.h>

void D3DRenderTargetView::Create(D3DResource &resource)
{
	assert(resource.IsValid() && !IsValid());

	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	D3DEngine::Instance().GetDevice()->CreateRenderTargetView(resource.Get(), nullptr, &pRenderTargetView);

	MakeObject(pRenderTargetView);
}

void D3DRenderTargetView::CreateAsTexture(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mipSlice)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;

	switch (type)
	{
	case eTexture1D:
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = mipSlice;
		break;
	case eTexture2D:
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = mipSlice;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	D3DEngine::Instance().GetDevice()->CreateRenderTargetView(resource.Get(), &desc, &pRenderTargetView);

	MakeObject(pRenderTargetView);
}

void D3DRenderTargetView::CreateAsTextureArray(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;

	switch (type)
	{
	case eTexture1DArray:
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MipSlice = mipSlice;
		desc.Texture1DArray.FirstArraySlice = firstArraySlice;
		desc.Texture1DArray.ArraySize = arraySize;
		break;
	case eTexture2DArray:
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = mipSlice;
		desc.Texture2DArray.FirstArraySlice = firstArraySlice;
		desc.Texture2DArray.ArraySize = arraySize;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	D3DEngine::Instance().GetDevice()->CreateRenderTargetView(resource.Get(), &desc, &pRenderTargetView);
	MakeObject(pRenderTargetView);
}

void D3DDepthStencilView::Create(uint32_t fmt, uint32_t width, uint32_t height)
{
	assert(!IsValid());

	D3DTexture2D resource;
	resource.Create(fmt, width, height, D3D11_BIND_DEPTH_STENCIL);

	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), nullptr, &pDepthStencilView));

	MakeObject(pDepthStencilView);
}

void D3DDepthStencilView::CreateAsTexture(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	desc.Flags = flags;
	switch (type)
	{
	case eTexture1D:
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = mipSlice;
		break;
	case eTexture2D:
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = mipSlice;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), &desc, &pDepthStencilView));

	MakeObject(pDepthStencilView);
}

void D3DDepthStencilView::CreateAsTextureArray(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	desc.Flags = flags;
	switch (type)
	{
	case eTexture1DArray:
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MipSlice = mipSlice;
		desc.Texture1DArray.FirstArraySlice = firstArraySlice;
		desc.Texture1DArray.ArraySize = arraySize;
		break;
	case eTexture2DArray:
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = mipSlice;
		desc.Texture2DArray.FirstArraySlice = firstArraySlice;
		desc.Texture2DArray.ArraySize = arraySize;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), &desc, &pDepthStencilView));

	MakeObject(pDepthStencilView);
}

void D3DShaderResourceView::CreateAsTexture(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mostDetailedMip, uint32_t mipLevels)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	switch (type)
	{
	case eTexture1D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MostDetailedMip = mostDetailedMip;
		desc.Texture1D.MipLevels = mipLevels;
		break;
	case eTexture2D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = mostDetailedMip;
		desc.Texture2D.MipLevels = mipLevels;
		break;
	case eTexture3D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MostDetailedMip = mostDetailedMip;
		desc.Texture3D.MipLevels = mipLevels;
		break;
	case eTextureCube:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = mostDetailedMip;
		desc.TextureCube.MipLevels = mipLevels;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11ShaderResourceView *pShaderResourceView = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateShaderResourceView(resource.Get(), &desc, &pShaderResourceView));

	MakeObject(pShaderResourceView);
}

void D3DShaderResourceView::CreateAsTextureArray(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	switch (type)
	{
	case eTexture1DArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MostDetailedMip = mostDetailedMip;
		desc.Texture1DArray.MipLevels = mipLevels;
		desc.Texture1DArray.FirstArraySlice = firstArraySlice;
		desc.Texture1DArray.ArraySize = arraySize;
		break;
	case eTexture2DArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MostDetailedMip = mostDetailedMip;
		desc.Texture2DArray.MipLevels = mipLevels;
		desc.Texture2DArray.FirstArraySlice = firstArraySlice;
		desc.Texture2DArray.ArraySize = arraySize;
		break;
	case eTextureCubeArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		desc.TextureCubeArray.MostDetailedMip = mostDetailedMip;
		desc.TextureCubeArray.MipLevels = mipLevels;
		desc.TextureCubeArray.First2DArrayFace = firstArraySlice;
		desc.TextureCubeArray.NumCubes = arraySize;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11ShaderResourceView *pShaderResourceView = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateShaderResourceView(resource.Get(), &desc, &pShaderResourceView));

	MakeObject(pShaderResourceView);
}

void D3DShaderResourceView::Create(const char *pDdsName)
{
	assert(pDdsName && !IsValid());

	bool bDDS = System::IsStrEndwith(pDdsName, ".dds");
	if (!bDDS)
	{
		assert(0);
	}

	std::string ddsFilePath = System::ResourceFilePath(pDdsName, System::eTexture);
	std::wstring wddsFilePath(ddsFilePath.begin(), ddsFilePath.end());

	ID3D11ShaderResourceView *pShaderResourceView = nullptr;
	HRCheck(DirectX::CreateDDSTextureFromFile(D3DEngine::Instance().GetDevice().Get(), wddsFilePath.c_str(), nullptr, &pShaderResourceView));
	
	MakeObject(pShaderResourceView);
}

void D3DUnorderedAccessView::Create(D3DResource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems, uint32_t flags)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)format;
	desc.ViewDimension = (D3D11_UAV_DIMENSION)dimension;
	desc.Buffer.FirstElement = firstElem;
	desc.Buffer.Flags = flags;
	desc.Buffer.NumElements = numElems;

	ID3D11UnorderedAccessView *pUnorderedAccessView = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateUnorderedAccessView(resource.Get(), &desc, &pUnorderedAccessView));

	MakeObject(pUnorderedAccessView);
}