#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(D3D11Sampler)
class D3D11Sampler : public D3DObject<ID3D11SamplerState>, public Sampler
{
public:
	D3D11Sampler(ID3D11Device* device, const SamplerDesc& desc);
};

NAMESPACE_END(Gfx)
#if 0
#include "Colorful/D3D/DxgiObject.h"

class D3D11RasterizerState : public D3DObject<ID3D11RasterizerState>
{
public:
	D3D11RasterizerState(const class D3D11Device& device, const GfxRasterizerStateDesc& gfxDesc);
};
using D3D11RasterizerStatePtr = std::shared_ptr<D3D11RasterizerState>;

class D3D11BlendState : public D3DObject<ID3D11BlendState>
{
public:
	D3D11BlendState(const class D3D11Device& device, const GfxBlendStateDesc& gfxDesc);
};
using D3D11BlendStatePtr = std::shared_ptr<D3D11BlendState>;

class D3D11DepthStencilState : public D3DObject<ID3D11DepthStencilState>
{
public:
	D3D11DepthStencilState(const class D3D11Device& device, const GfxDepthStencilStateDesc& gfxDesc);
};
using D3D11DepthStencilStatePtr = std::shared_ptr<D3D11DepthStencilState>;
#endif