#pragma once

#include "D3DObject.h"

class D3DView
{
public:
	enum eViewType
	{
		eUnknown,
		eBuffer,
		eTexture1D,
		eTexture2D,
		eTexture3D,
		eTextureCube,
		eTexture1DArray,
		eTexture2DArray,
		eTextureCubeArray
	};

	enum eDepthStencilViewFlags
	{
		eReadonlyDepth = D3D11_DSV_READ_ONLY_DEPTH,
		eReadonlyStencil = D3D11_DSV_READ_ONLY_STENCIL,
		eReadonlyDepthStencil = eReadonlyDepth | eReadonlyStencil
	};
};

class D3DRenderTargetView : public D3DObject<ID3D11RenderTargetView>, public D3DView
{
public:
	void Create(D3DResource &resource);
	void CreateAsBuffer();
	void CreateAsTexture(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mipSlice);
	void CreateAsTextureArray(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
	void CreateAs3DTexture();
};

class D3DDepthStencilView : public D3DObject<ID3D11DepthStencilView>, public D3DView
{
public:
	enum eClearFlag
	{
		eDepthOnly = D3D11_CLEAR_DEPTH,
		eStencilOnly = D3D11_CLEAR_STENCIL,
		eDepthStencil = eDepthOnly | eStencilOnly
	};

	void Create(uint32_t fmt, uint32_t width, uint32_t height);
	void CreateAsBuffer();
	void CreateAsTexture(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice);
	void CreateAsTextureArray(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t flags, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
};

class D3DShaderResourceView : public D3DObject<ID3D11ShaderResourceView>, public D3DView
{
public:
	void Create(const char *pDdsName, bool bSRGB = false);

	void CreateAsBuffer();
	void CreateAsBufferEx();
	void CreateAsTexture(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mostDetailedMip, uint32_t mipLevels);
	void CreateAsTextureArray(eViewType type, D3DResource &resource, uint32_t fmt, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
protected:
};

class D3DUnorderedAccessView : public D3DObject<ID3D11UnorderedAccessView>, public D3DView
{
public:
	void Create(D3DResource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems, uint32_t flags);
};
