#if 0
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
	std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> subResourceData(new D3D11_SUBRESOURCE_DATA[mipLevels * arrayLayers]());
	assert(subResourceData);
	const byte8_t* start = reinterpret_cast<const byte8_t*>(data);
	const byte8_t* end = start + dataSize;
	uint32_t index = 0u;
	for (uint32_t i = 0u; i < arrayLayers; ++i)
	{
		uint32_t tWidth = width;
		uint32_t tHeight = height;
		uint32_t tDepth = depth;

		for (uint32_t j = 0u; j < mipLevels; ++j)
		{
			size_t numBytes = 0u;
			size_t rowBytes = 0u;
			size_t numRows = 0u;

			caculateSurfaceInfo(tWidth, tHeight, format, numBytes, rowBytes, numRows);
			assert(numBytes <= std::numeric_limits<uint32_t>().max());
			assert(rowBytes <= std::numeric_limits<uint32_t>().max());

			subResourceData[index].pSysMem = start;
			subResourceData[index].SysMemPitch = (uint32_t)rowBytes;
			subResourceData[index].SysMemSlicePitch = (uint32_t)numBytes;
			++index;

			assert(start + (numBytes * tDepth) <= end);
			start += numBytes * tDepth;

			tWidth = tWidth >> 1u;
			tHeight = tHeight >> 1u;
			tDepth = tDepth >> 1u;

			tWidth = tWidth == 0u ? 1u : tWidth;
			tHeight = tHeight == 0u ? 1u : tHeight;
			tDepth = tDepth == 0u ? 1u : tDepth;
		}
	}

	ID3D11Resource* resource = nullptr;
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
		GfxVerifyD3D(device->CreateTexture1D(&desc, data == nullptr ? nullptr : subResourceData.get(), &texture1D));
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
		GfxVerifyD3D(device->CreateTexture2D(&desc, data == nullptr ? nullptr : subResourceData.get(), &texture2D));
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
		GfxVerifyD3D(device->CreateTexture3D(&desc, data == nullptr ? nullptr : subResourceData.get(), &texture3D));
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

void D3D11Texture::caculateSurfaceInfo(
	uint32_t width, 
	uint32_t height, 
	DXGI_FORMAT format, 
	__out size_t& numBytes, 
	__out size_t& rowBytes, 
	__out size_t& numRows) const
{
	numBytes = rowBytes = numRows = 0u;

	bool8_t BCFormat = false;
	bool8_t Packed = false;
	bool8_t Planar = false;
	size_t bytesByFormat = 0u;

	switch (format)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		BCFormat = true;
		bytesByFormat = 8u;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		BCFormat = true;
		bytesByFormat = 16u;
		break;

	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_YUY2:
		Packed = true;
		bytesByFormat = 4u;
		break;

	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		Packed = true;
		bytesByFormat = 8u;
		break;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
		Planar = true;
		bytesByFormat = 2u;
		break;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		Planar = true;
		bytesByFormat = 4u;
		break;

	default:
		break;
	}

	if (BCFormat)
	{
		size_t numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = std::max<size_t>(1u, (width + 3u) / 4u);
		}
		size_t numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = std::max<size_t>(1u, (height + 3u) / 4u);
		}
		rowBytes = numBlocksWide * bytesByFormat;
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if (Packed)
	{
		rowBytes = ((width + 1u) >> 1) * bytesByFormat;
		numRows = height;
		numBytes = rowBytes * height;
	}
	else if (format == DXGI_FORMAT_NV11)
	{
		rowBytes = ((width + 3u) >> 2) * 4u;
		numRows = height * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
		numBytes = rowBytes * numRows;
	}
	else if (Planar)
	{
		rowBytes = ((width + 1u) >> 1) * bytesByFormat;
		numBytes = (rowBytes * height) + ((rowBytes * height + 1u) >> 1);
		numRows = height + ((height + 1u) >> 1);
	}
	else
	{
		size_t bpp = bitsPerPixel(format);
		assert(bpp);

		rowBytes = (width * bpp + 7u) / 8u; // round up to nearest byte
		numRows = height;
		numBytes = rowBytes * height;
	}
}

size_t D3D11Texture::bitsPerPixel(DXGI_FORMAT format) const
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128u;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96u;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	case DXGI_FORMAT_Y416:
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		return 64u;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_AYUV:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_YUY2:
		return 32u;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		return 24u;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_A8P8:
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return 16u;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
	case DXGI_FORMAT_NV11:
		return 12u;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
		return 8u;

	case DXGI_FORMAT_R1_UNORM:
		return 1u;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 4u;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8u;

	default:
		return 0u;
	}
}
#endif
