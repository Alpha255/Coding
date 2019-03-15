#pragma once

#include "D3D11Object.h"

class D3D11RenderTargetView : public D3DObject<ID3D11RenderTargetView>
{
public:
	void Create(D3D11Resource &resource);
	void CreateAsBuffer();
	void CreateAsTexture(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mipSlice);
	void CreateAsTextureArray(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
	void CreateAs3DTexture();
};

class D3D11DepthStencilView : public D3DObject<ID3D11DepthStencilView>
{
public:
	void Create(uint32_t fmt, uint32_t width, uint32_t height);
	void CreateAsBuffer();
	void CreateAsTexture(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice);
	void CreateAsTextureArray(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
};

class D3D11ShaderResourceView : public D3DObject<ID3D11ShaderResourceView>
{
public:
	void Create(const char *pDdsName, bool bSRGB = false);

	void CreateAsBuffer();
	void CreateAsBufferEx();
	void CreateAsTexture(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mostDetailedMip, uint32_t mipLevels);
	void CreateAsTextureArray(eRViewType type, D3D11Resource &resource, uint32_t fmt, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
protected:
};

class D3D11UnorderedAccessView : public D3DObject<ID3D11UnorderedAccessView>
{
public:
	void Create(D3D11Resource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems, uint32_t flags);
};
