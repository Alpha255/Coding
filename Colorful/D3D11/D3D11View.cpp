#include "D3D11View.h"
#include "D3D11Texture.h"
#include "D3D11Engine.h"
#include "D3D11Util.h"
#include "Tool/AssetTool/AssetTool.h"

void D3D11RenderTargetView::Create(D3D11Resource &resource)
{
	assert(resource.IsValid() && !IsValid());

	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	D3D11Engine::Instance().GetDevice()->CreateRenderTargetView(resource.Get(), nullptr, &pRenderTargetView);

	Reset(pRenderTargetView);
}

void D3D11RenderTargetView::CreateAsTexture(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mipSlice)
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
	D3D11Engine::Instance().GetDevice()->CreateRenderTargetView(resource.Get(), &desc, &pRenderTargetView);

	Reset(pRenderTargetView);
}

void D3D11RenderTargetView::CreateAsTextureArray(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize)
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
	D3D11Engine::Instance().GetDevice()->CreateRenderTargetView(resource.Get(), &desc, &pRenderTargetView);
	Reset(pRenderTargetView);
}

void D3D11DepthStencilView::Create(uint32_t fmt, uint32_t width, uint32_t height)
{
	assert(!IsValid());

	D3D11Texture2D resource;
	resource.Create(fmt, width, height, D3D11_BIND_DEPTH_STENCIL);

	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), nullptr, &pDepthStencilView));

	Reset(pDepthStencilView);
}

void D3D11DepthStencilView::CreateAsTexture(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice)
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
	Check(D3D11Engine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), &desc, &pDepthStencilView));

	Reset(pDepthStencilView);
}

void D3D11DepthStencilView::CreateAsTextureArray(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize)
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
	Check(D3D11Engine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), &desc, &pDepthStencilView));

	Reset(pDepthStencilView);
}

void D3D11ShaderResourceView::CreateAsTexture(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mipLevels, uint32_t)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	switch (type)
	{
	case eTexture1D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MostDetailedMip = 0U;
		desc.Texture1D.MipLevels = mipLevels;
		break;
	case eTexture2D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0U;
		desc.Texture2D.MipLevels = mipLevels;
		break;
	case eTexture3D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MostDetailedMip = 0U;
		desc.Texture3D.MipLevels = mipLevels;
		break;
	case eTextureCube:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = 0U;
		desc.TextureCube.MipLevels = mipLevels;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11ShaderResourceView *pShaderResourceView = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateShaderResourceView(resource.Get(), &desc, &pShaderResourceView));

	Reset(pShaderResourceView);
}

void D3D11ShaderResourceView::CreateAsTextureArray(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	switch (type)
	{
	case eTexture1DArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray.MostDetailedMip = 0U;
		desc.Texture1DArray.MipLevels = mipLevels;
		desc.Texture1DArray.FirstArraySlice = firstArraySlice;
		desc.Texture1DArray.ArraySize = arraySize;
		break;
	case eTexture2DArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MostDetailedMip = 0U;
		desc.Texture2DArray.MipLevels = mipLevels;
		desc.Texture2DArray.FirstArraySlice = firstArraySlice;
		desc.Texture2DArray.ArraySize = arraySize;
		break;
	case eTextureCubeArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		desc.TextureCubeArray.MostDetailedMip = 0U;
		desc.TextureCubeArray.MipLevels = mipLevels;
		desc.TextureCubeArray.First2DArrayFace = firstArraySlice;
		desc.TextureCubeArray.NumCubes = arraySize;
		break;
	default:
		assert(!"Invalid type!!!");
	}

	ID3D11ShaderResourceView *pShaderResourceView = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateShaderResourceView(resource.Get(), &desc, &pShaderResourceView));

	Reset(pShaderResourceView);
}

void D3D11ShaderResourceView::Create(const std::string &ddsName, bool)
{
	assert(!IsValid() && Base::GetFileExtension(ddsName, true) == ".dds");

	ID3D11ShaderResourceView *pShaderResourceView = nullptr;
	AssetFile textureFile(ddsName);
	AssetTool::CreateTextureFromFile(D3D11Engine::Instance().GetDevice().Get(), textureFile, nullptr, &pShaderResourceView);
	
	Reset(pShaderResourceView);
}

void D3D11UnorderedAccessView::Create(D3D11Resource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems, uint32_t flags)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)format;
	desc.ViewDimension = (D3D11_UAV_DIMENSION)dimension;
	desc.Buffer.FirstElement = firstElem;
	desc.Buffer.Flags = flags;
	desc.Buffer.NumElements = numElems;

	ID3D11UnorderedAccessView *pUnorderedAccessView = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateUnorderedAccessView(resource.Get(), &desc, &pUnorderedAccessView));

	Reset(pUnorderedAccessView);
}