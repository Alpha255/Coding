#pragma once

#include "Colorful/D3D/D3D11/D3D11Texture.h"
#if 0

class D3D11ShaderResourceView : public D3DObject<ID3D11ShaderResourceView>, public GfxTexture
{
public:
	D3D11ShaderResourceView(const class D3D11Device& device, const std::string& name);
	D3D11ShaderResourceView(
		const class D3D11Device& device,
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void* data,
		size_t dataSize);
protected:
private:
};
using D3D11ShaderResourceViewPtr = std::shared_ptr<D3D11ShaderResourceView>;

class D3D11RenderTargetView : public D3DObject<ID3D11RenderTargetView>
{
};
using D3D11RenderTargetViewPtr = std::shared_ptr<D3D11RenderTargetView>;

class D3D11DepthStencilView : public D3DObject<ID3D11DepthStencilView>
{
};
using D3D11DepthStencilViewPtr = std::shared_ptr<D3D11DepthStencilView>; 
#endif