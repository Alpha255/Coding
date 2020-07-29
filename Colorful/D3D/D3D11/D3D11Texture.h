#pragma once

#include "Colorful/Public/GfxResource.h"

class D3DTexture : public D3DObject<ID3D11Resource>, public GfxTexture
{
public:
	D3DTexture(const std::string& name, const class D3D11Device& device);
protected:
private:
};
using D3DTexturePtr = std::shared_ptr<D3DTexture>;