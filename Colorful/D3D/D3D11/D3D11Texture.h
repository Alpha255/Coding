#pragma once
#if 0
#include "Colorful/Public/GfxResource.h"

class D3D11Texture : public D3DObject<ID3D11Resource>
{
public:
	D3D11Texture(
		const class D3D11Device& device,
		eRTextureType type,
		DXGI_FORMAT format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		uint32_t bindFlags,
		const void* data,
		size_t dataSize);
protected:
	void caculateSurfaceInfo(
		uint32_t width,
		uint32_t height,
		DXGI_FORMAT format,
		__out size_t& numBytes,
		__out size_t& rowBytes,
		__out size_t& numRows) const;

	size_t bitsPerPixel(DXGI_FORMAT format) const;
private:
};
using D3D11TexturePtr = std::shared_ptr<D3D11Texture>;
#endif