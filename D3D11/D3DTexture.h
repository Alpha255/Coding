#pragma once

#include "D3DObject.h"

class D3DTexture1D : public D3DResource
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
		const D3D11_SUBRESOURCE_DATA *pSubResData = nullptr);

	void CreateRandomTexture();
};

class D3DTexture2D : public D3DResource
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
		const D3D11_SUBRESOURCE_DATA *pSubResData = nullptr);
};

class D3DTexture3D : public D3DResource
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
		const D3D11_SUBRESOURCE_DATA *pSubResData = nullptr);
};