#pragma once

#include "D3D11Object.h"

struct D3D11SubResourceData : public D3D11_SUBRESOURCE_DATA 
{
	inline D3D11SubResourceData(const void *pMem, uint32_t memPitch, uint32_t memSlicePitch)
	{
		D3D11_SUBRESOURCE_DATA::pSysMem = pMem;
		D3D11_SUBRESOURCE_DATA::SysMemPitch = memPitch;
		D3D11_SUBRESOURCE_DATA::SysMemSlicePitch = memSlicePitch;
	}
};

class D3D11Texture1D : public D3D11Resource
{
public:
	void Create(
		uint32_t fmt,
		uint32_t width,
		uint32_t bindFlags,
		uint32_t mipLevels = 1U,
		uint32_t arraySize = 1U,
		uint32_t cpuFlags = 0U,
		uint32_t miscFlags = 0U,
		uint32_t usage = 0U,
		const D3D11SubResourceData *pSubResourceData = nullptr);

	void CreateRandomTexture();
};

class D3D11Texture2D : public D3D11Resource
{
public:
	void Create(
		uint32_t fmt,
		uint32_t width,
		uint32_t height,
		uint32_t bindFlags,
		uint32_t mipLevels = 0U,
		uint32_t arraySize = 1U,
		uint32_t cpuFlags = 0U,
		uint32_t miscFlags = 0U,
		uint32_t usage = 0U,
		const D3D11SubResourceData *pSubResourceData = nullptr);
};

class D3D11Texture3D : public D3D11Resource
{
public:
	void Create(
		uint32_t fmt,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t bindFlags,
		uint32_t mipLevels = 0U,
		uint32_t cpuFlags = 0U,
		uint32_t miscFlags = 0U,
		uint32_t usage = 0U,
		const D3D11SubResourceData *pSubResourceData = nullptr);
};