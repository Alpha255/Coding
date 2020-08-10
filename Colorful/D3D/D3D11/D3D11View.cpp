#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "AssetTool/AssetDatabase.h"

D3D11ShaderResourceView::D3D11ShaderResourceView(const D3D11Device& device, const std::string& name)
{
	assert(device.isValid());
	auto texAsset = AssetTool::AssetDatabase::instance().tryToGetAsset(name);
	if (texAsset->type() == AssetTool::Asset::eDDSTexture || texAsset->type() == AssetTool::Asset::eWICTexture)
	{
		ID3D11ShaderResourceView* view = AssetTool::AssetDatabase::instance().tryToLoadD3DTextureFromFile(device.get(), texAsset);
		reset(view);
	}
	else if (texAsset->type() == AssetTool::Asset::eVulkanTexture)
	{
		auto texBinary = AssetTool::AssetDatabase::instance().tryToGetTextureBinary(Configurations::eD3D11, name);
		new(this)D3D11ShaderResourceView(
			device,
			texBinary.Type,
			(eRFormat)texBinary.Format,
			texBinary.Width,
			texBinary.Height,
			texBinary.Depth,
			texBinary.MipLevels,
			texBinary.ArrayLayers,
			(const void*)texBinary.SharedBinary.get(),
			texBinary.Size);
	}
}

D3D11ShaderResourceView::D3D11ShaderResourceView(
	const D3D11Device& device, 
	eRTextureType type, 
	eRFormat format, 
	uint32_t width, 
	uint32_t height, 
	uint32_t depth, 
	uint32_t mipLevels, 
	uint32_t arrayLayers, 
	const void* data, 
	size_t dataSize)
{
	assert(device.isValid());
	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = D3D11Enum::toFormat(format);

	auto resource = std::make_shared<D3D11Texture>(
		device,
		type,
		desc.Format,
		width,
		height,
		depth,
		mipLevels,
		arrayLayers,
		D3D11_BIND_SHADER_RESOURCE,
		data,
		dataSize);

	switch (type)
	{
	case eTexture1D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		desc.Texture1D = D3D11_TEX1D_SRV
		{
			0u,
			mipLevels
		};
		break;
	case eTexture1DArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		desc.Texture1DArray = D3D11_TEX1D_ARRAY_SRV
		{
			0u,
			mipLevels,
			0u,
			arrayLayers
		};
		break;
	case eTexture2D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D = D3D11_TEX2D_SRV
		{
			0u,
			mipLevels
		};
		break;
	case eTexture2DArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray = D3D11_TEX2D_ARRAY_SRV
		{
			0u,
			mipLevels,
			0u,
			arrayLayers
		};
		break;
	case eTexture3D:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D = D3D11_TEX3D_SRV
		{
			0u,
			mipLevels
		};
		break;
	case eTextureCube:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube = D3D11_TEXCUBE_SRV
		{
			0u,
			mipLevels
		};
		break;
	case eTextureCubeArray:
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		desc.TextureCubeArray = D3D11_TEXCUBE_ARRAY_SRV
		{
			0u,
			mipLevels,
			0u,
			arrayLayers / 6u
		};
		break;
	default:
		assert(0);
	}

	ID3D11ShaderResourceView* view = nullptr;
	GfxVerifyD3D(device->CreateShaderResourceView(resource->get(), &desc, &view));
	reset(view);
}
