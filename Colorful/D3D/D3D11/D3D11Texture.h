#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(D3D11Texture)
class D3D11Texture final : public D3DObject<ID3D11Resource>
{
public:
	D3D11Texture(ID3D11Device* device, const TextureDesc& desc);
};

DECLARE_SHARED_PTR(D3D11Texture1D)
class D3DTexture1D final : public D3DObject<ID3D11Texture1D>, public Texture
{
public:
	D3DTexture1D(ID3D11Device* device, const TextureDesc& desc);
};

DECLARE_SHARED_PTR(D3D11Texture2D)
class D3DTexture2D final : public D3DObject<ID3D11Texture2D>, public Texture
{
public:
	D3DTexture2D(ID3D11Device* device, const TextureDesc& desc);
};

DECLARE_SHARED_PTR(D3D11Texture3D)
class D3DTexture3D final : public D3DObject<ID3D11Texture3D>, public Texture
{
public:
	D3DTexture3D(ID3D11Device* device, const TextureDesc& desc);
};

NAMESPACE_END(Gfx)