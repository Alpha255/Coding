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

void D3DDepthStencilView::Create(D3DResource &resource, uint32_t format, uint32_t width, uint32_t height, uint32_t dimension)
{
	assert(!IsValid());

	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	if (resource.IsValid())
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = (DXGI_FORMAT)format;
		desc.ViewDimension = (D3D11_DSV_DIMENSION)dimension;
		desc.Flags = 0U;
		switch (dimension)
		{
		case D3D11_DSV_DIMENSION_TEXTURE1D:
			desc.Texture1D.MipSlice = 0U;
			break;
		case D3D11_DSV_DIMENSION_TEXTURE2D:
			desc.Texture2D.MipSlice = 0U;
			break;
		default:
			assert("Unsupported yet!!!");
			break;
		}

		HRCheck(D3DEngine::Instance().GetDevice()->CreateDepthStencilView(resource.Get(), &desc, &pDepthStencilView));
	}
	else
	{
		D3DTexture2D surface;
		surface.Create(format, width, height, D3D11_BIND_DEPTH_STENCIL);
		HRCheck(D3DEngine::Instance().GetDevice()->CreateDepthStencilView(surface.Get(), nullptr, &pDepthStencilView));
	}

	MakeObject(pDepthStencilView);
}

void D3DShaderResourceView::Create(D3DResource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems)
{
	assert(resource.IsValid() && !IsValid());

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = (DXGI_FORMAT)format;
	desc.ViewDimension = (D3D11_SRV_DIMENSION)dimension;
	switch (dimension)
	{
	case D3D11_SRV_DIMENSION_TEXTURE1D:
		desc.Texture1D.MipLevels = 1U;
		desc.Texture1D.MostDetailedMip = 0U;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		desc.Texture2D.MipLevels = 1U;
		desc.Texture2D.MostDetailedMip = 0U;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		desc.Texture3D.MipLevels = 1U;
		desc.Texture3D.MostDetailedMip = 0U;
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		desc.TextureCube.MipLevels = 1U;
		desc.TextureCube.MostDetailedMip = 0U;
		break;
	case D3D11_SRV_DIMENSION_BUFFER:
		desc.Buffer.FirstElement = firstElem;
		desc.Buffer.NumElements = numElems;
	default:
		assert("Unsupported yet!!");
		break;
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