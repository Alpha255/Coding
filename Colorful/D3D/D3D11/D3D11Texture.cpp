#include "Colorful/D3D/D3D11/D3D11Texture.h"
#include "Colorful/D3D/D3D11/D3D11Map.h"

NAMESPACE_START(Gfx)

D3D11Texture::D3D11Texture(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.Subresources.size());
	for (int i = 0u; i < desc.Subresources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data.get() + desc.Subresources[i].Offset,
			desc.Subresources[i].RowBytes,
			desc.Subresources[i].SliceBytes
		};
	}

	if (desc.Dimension == ETextureType::T_1D || desc.Dimension == ETextureType::T_1D_Array)
	{
		D3D11_TEXTURE1D_DESC createDesc
		{
			desc.Width,
			desc.MipLevels,
			desc.ArraySize,
			D3D11Map::format(desc.Format),
			D3D11Map::usage(desc.Usage),
			D3D11Map::bindFlags(desc.BindFlags),
			0u,
			0u
		};
		VERIFY_D3D(device->CreateTexture1D(&createDesc, desc.Data ? subresources.data() : nullptr, reinterpret_cast<ID3D11Texture1D**>(reference())));
	}
	else if (desc.Dimension == ETextureType::T_2D || desc.Dimension == ETextureType::T_2D_Array || 
		desc.Dimension == ETextureType::T_Cube || desc.Dimension == ETextureType::T_Cube_Array)
	{
		D3D11_TEXTURE2D_DESC createDesc
		{
			desc.Width,
			desc.Height,
			desc.MipLevels,
			desc.ArraySize,
			D3D11Map::format(desc.Format),
			{
				desc.SampleCount,
				0u
			},
			D3D11Map::usage(desc.Usage),
			D3D11Map::bindFlags(desc.BindFlags),
			0u,
			(desc.Dimension == ETextureType::T_Cube || desc.Dimension == ETextureType::T_Cube_Array) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0u
		};
		VERIFY_D3D(device->CreateTexture2D(&createDesc, desc.Data ? subresources.data() : nullptr, reinterpret_cast<ID3D11Texture2D**>(reference())));
	}
	else if (desc.Dimension == ETextureType::T_3D)
	{
		D3D11_TEXTURE3D_DESC createDesc
		{
			desc.Width,
			desc.Height,
			desc.Depth,
			desc.MipLevels,
			D3D11Map::format(desc.Format),
			D3D11Map::usage(desc.Usage),
			D3D11Map::bindFlags(desc.BindFlags),
			0u,
			0u
		};
		VERIFY_D3D(device->CreateTexture3D(&createDesc, desc.Data ? subresources.data() : nullptr, reinterpret_cast<ID3D11Texture3D**>(reference())));
	}
	else
	{
		assert(0);
	}
}

D3D11Texture1D::D3D11Texture1D(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.Subresources.size());
	for (int i = 0u; i < desc.Subresources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data.get() + desc.Subresources[i].Offset,
			desc.Subresources[i].RowBytes,
			desc.Subresources[i].SliceBytes
		};
	}

	D3D11_TEXTURE1D_DESC createDesc
	{
		desc.Width,
		desc.MipLevels,
		desc.ArraySize,
		D3D11Map::format(desc.Format),
		D3D11Map::usage(desc.Usage),
		D3D11Map::bindFlags(desc.BindFlags),
		0u,
		0u
	};
	VERIFY_D3D(device->CreateTexture1D(&createDesc, desc.Data ? subresources.data() : nullptr, reference()));
}

D3D11Texture2D::D3D11Texture2D(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.Subresources.size());
	for (int i = 0u; i < desc.Subresources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data.get() + desc.Subresources[i].Offset,
			desc.Subresources[i].RowBytes,
			desc.Subresources[i].SliceBytes
		};
	}

	D3D11_TEXTURE2D_DESC createDesc
	{
		desc.Width,
		desc.Height,
		desc.MipLevels,
		desc.ArraySize,
		D3D11Map::format(desc.Format),
		{
			desc.SampleCount,
			0u
		},
		D3D11Map::usage(desc.Usage),
		D3D11Map::bindFlags(desc.BindFlags),
		0u,
		(desc.Dimension == ETextureType::T_Cube || desc.Dimension == ETextureType::T_Cube_Array) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0u
	};
	VERIFY_D3D(device->CreateTexture2D(&createDesc, desc.Data ? subresources.data() : nullptr, reference()));
}

D3D11Texture3D::D3D11Texture3D(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.Subresources.size());
	for (int i = 0u; i < desc.Subresources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data.get() + desc.Subresources[i].Offset,
			desc.Subresources[i].RowBytes,
			desc.Subresources[i].SliceBytes
		};
	}

	D3D11_TEXTURE3D_DESC createDesc
	{
		desc.Width,
		desc.Height,
		desc.Depth,
		desc.MipLevels,
		D3D11Map::format(desc.Format),
		D3D11Map::usage(desc.Usage),
		D3D11Map::bindFlags(desc.BindFlags),
		0u,
		0u
	};
	VERIFY_D3D(device->CreateTexture3D(&createDesc, desc.Data ? subresources.data() : nullptr, reference()));
}

NAMESPACE_END(Gfx)
