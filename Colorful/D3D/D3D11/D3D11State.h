#pragma once

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

class D3D11SamplerState : public D3DObject<ID3D11SamplerState>, public GfxSampler
{
public:
	D3D11SamplerState(const class D3D11Device& device, const GfxSamplerDesc& gfxDesc);
};
using D3D11SamplerStatePtr = std::shared_ptr<D3D11SamplerState>;
