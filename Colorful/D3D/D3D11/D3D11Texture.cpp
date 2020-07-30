#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3D11Texture::D3D11Texture(
	const D3D11Device& device, 
	eRTextureType type, 
	DXGI_FORMAT format,
	uint32_t width, 
	uint32_t height, 
	uint32_t depth, 
	uint32_t mipLevels, 
	uint32_t arrayLayers, 
	uint32_t bindFlags,
	const void* data, 
	size_t dataSize)
{
	ID3D11Resource* resource = nullptr;
	D3D11_SUBRESOURCE_DATA subResource
	{
		data,
		(uint32_t)dataSize,
		0u
	};

	if (type == eTexture1D || type == eTexture1DArray)
	{
		D3D11_TEXTURE1D_DESC desc
		{
			width,
			mipLevels,
			arrayLayers,
			format,
			D3D11_USAGE_DEFAULT,
			bindFlags,
			0u,
			0u
		};
		ID3D11Texture1D* texture1D = nullptr;
		GfxVerifyD3D(device->CreateTexture1D(&desc, data == nullptr ? nullptr : &subResource, &texture1D));
		resource = static_cast<ID3D11Resource*>(texture1D);
	}
	else if (type == eTexture2D || type == eTexture2DArray)
	{
		D3D11_TEXTURE2D_DESC desc
		{
			width,
			height,
			mipLevels,
			arrayLayers,
			format,
			{
				1u,
				0u
			},
			D3D11_USAGE_DEFAULT,
			bindFlags,
			0u,
			0u
		};
		ID3D11Texture2D* texture2D = nullptr;
		GfxVerifyD3D(device->CreateTexture2D(&desc, data == nullptr ? nullptr : &subResource, &texture2D));
		resource = static_cast<ID3D11Resource*>(texture2D);
	}
	else if (type == eTexture3D)
	{
		D3D11_TEXTURE3D_DESC desc
		{
			width,
			height,
			depth,
			mipLevels,
			format,
			D3D11_USAGE_DEFAULT,
			bindFlags,
			0u,
			0u
		};
		ID3D11Texture3D* texture3D = nullptr;
		GfxVerifyD3D(device->CreateTexture3D(&desc, data == nullptr ? nullptr : &subResource, &texture3D));
		resource = static_cast<ID3D11Resource*>(texture3D);
	}
	else if (type == eTextureCube || type == eTextureCubeArray)
	{
		assert(0);
	}
	else
	{
		assert(0);
	}

	reset(resource);
}
