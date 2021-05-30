#pragma once

#include "Colorful/D3D/D3D11/D3D11Texture.h"

NAMESPACE_START(Gfx)

class D3D11TextureView : public Texture
{
public:
	using Texture::Texture;
protected:
	ID3D11Resource* createTexture(ID3D11Device* device, const TextureDesc& desc);
	D3D11TexturePtr m_Texture = nullptr;
};
DECLARE_SHARED_PTR(D3D11TextureView)

class D3D11ShaderResourceView : public D3DObject<ID3D11ShaderResourceView>, public D3D11TextureView
{
public:
	D3D11ShaderResourceView(ID3D11Device* device, const TextureDesc& desc);
};
DECLARE_SHARED_PTR(D3D11ShaderResourceView)

class D3D11RenderTargetView : public D3DObject<ID3D11RenderTargetView>, public D3D11TextureView
{
public:
	D3D11RenderTargetView(ID3D11Device* device, const TextureDesc& desc);
};
DECLARE_SHARED_PTR(D3D11RenderTargetView)

class D3D11DepthStencilView : public D3DObject<ID3D11DepthStencilView>, public D3D11TextureView
{
public:
	D3D11DepthStencilView(ID3D11Device* device, const TextureDesc& desc);
};
DECLARE_SHARED_PTR(D3D11DepthStencilView)

class D3D11UnorderedAccessView : public D3DObject<ID3D11UnorderedAccessView>
{
public:
	D3D11UnorderedAccessView(ID3D11Device* device);
};
DECLARE_SHARED_PTR(D3D11DepthStencilView)

NAMESPACE_END(Gfx)