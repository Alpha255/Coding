#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

class D3D11Texture
{
public:
	inline uint32_t calcSubresource(uint32_t mipSlice, uint32_t arraySlice, uint32_t mipLevels)
	{
		return mipSlice + (arraySlice * mipLevels);
	}

	inline uint32_t calcSubresource(uint32_t mipSlice, uint32_t arraySlice, uint32_t planeSlice, uint32_t mipLevels, uint32_t arraySize)
	{
		return mipSlice + (arraySlice * mipLevels) + (planeSlice * mipLevels * arraySize);
	}
};
DECLARE_SHARED_PTR(D3D11Texture)

class D3D11Texture1D final : public D3DObject<ID3D11Texture1D>, public D3D11Texture
{
public:
	D3D11Texture1D(ID3D11Device* device, const TextureDesc& desc);
};
DECLARE_SHARED_PTR(D3D11Texture1D)

class D3D11Texture2D final : public D3DObject<ID3D11Texture2D>, public D3D11Texture
{
public:
	D3D11Texture2D(ID3D11Device* device, const TextureDesc& desc);
};
DECLARE_SHARED_PTR(D3D11Texture2D)

class D3D11Texture3D final : public D3DObject<ID3D11Texture3D>, public D3D11Texture
{
public:
	D3D11Texture3D(ID3D11Device* device, const TextureDesc& desc);
};
DECLARE_SHARED_PTR(D3D11Texture3D)

class D3D11Sampler : public D3DObject<ID3D11SamplerState>, public Sampler
{
public:
	D3D11Sampler(ID3D11Device* device, const SamplerDesc& desc);
};
DECLARE_SHARED_PTR(D3D11Sampler)

NAMESPACE_END(Gfx)