#pragma once

#include "D3DObject.h"

class D3DRenderTargetView : public D3DObject<ID3D11RenderTargetView>
{
public:
	void Create(D3DResource &resource);
};

class D3DDepthStencilView : public D3DObject<ID3D11DepthStencilView>
{
public:
	enum eFlag
	{
		eDepthOnly = D3D11_CLEAR_DEPTH,
		eStencilOnly = D3D11_CLEAR_STENCIL,
		eDepthStencil = eDepthOnly | eStencilOnly
	};

	void Create(D3DResource &resource, uint32_t format, uint32_t width, uint32_t height, uint32_t dimension);
};

class D3DShaderResourceView : public D3DObject<ID3D11ShaderResourceView>
{
public:
	void Create(D3DResource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems);

	void Create(const char *pDdsName);
};

class D3DUnorderedAccessView : public D3DObject<ID3D11UnorderedAccessView>
{
public:
	void Create(D3DResource &resource, uint32_t format, uint32_t dimension, uint32_t firstElem, uint32_t numElems, uint32_t flags);
};
