#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/stb/stb_image.h>
#include <ThirdParty/DirectXTex/DirectXTex/DDS.h>
#include <ThirdParty/KTX-Software/include/ktx.h>

NAMESPACE_START(Gfx)

#if !defined(MAKEFOURCC)
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

enum class ETextureFormat : uint8_t
{
	Unsupported,
	DDS,
	KTX,
	PNG,
	JPEG,
	BMP,
	TGA,
	PSD,
	GIF,
	HDR,
	PIC,
	PNM
};

struct TextureFormatAttribute
{
	ETextureFormat Format = ETextureFormat::Unsupported;
	std::string Extension;
	std::string Description;
};

class TextureLoader
{
public:
	static TextureDesc load(AssetPtr asset, bool8_t sRGB)
	{
		assert(asset);
		std::string path(std::move(asset->fullPath()));
		auto format = getFormat(asset->fullPath());
		assert(format != ETextureFormat::Unsupported);

		auto dataPtr = asset->data(File::EMode::Binary);
		const byte8_t* data = dataPtr.get();
		const size_t dataSize = asset->size();
		assert(dataSize <= std::numeric_limits<int32_t>().max());

		TextureDesc desc{};

		if (format == ETextureFormat::DDS)
		{
			desc = std::move(load_DDS(data, dataSize));
		}
		else if (format == ETextureFormat::KTX)
		{
			desc = std::move(load_KTX(data, dataSize));
		}
		else
		{
			desc = std::move(load_General(data, dataSize));
		}

		if (sRGB)
		{
			desc.Format = FormatAttribute::toSRGBFormat(desc.Format);
		}

		return desc;
	}
protected:
	static ETextureFormat getFormat(const std::string& path)
	{
		std::string extension(std::move(File::extension(path, true)));
		for each (auto& attr in s_SupportedFormats)
		{
			if (attr.Extension == extension)
			{
				return attr.Format;
			}
		}
		return ETextureFormat::Unsupported;
	}

	static DXGI_FORMAT pixelFormatToDXGIFormat(const DirectX::DDS_PIXELFORMAT& ddpf)
	{
#define IS_BITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )
		if (ddpf.flags & DDS_RGB)
		{
			switch (ddpf.RGBBitCount)
			{
			case 32:
				if (IS_BITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				{
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				}
				if (IS_BITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
				{
					return DXGI_FORMAT_B8G8R8A8_UNORM;
				}
				if (IS_BITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0))
				{
					return DXGI_FORMAT_B8G8R8X8_UNORM;
				}
				if (IS_BITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
				{
					return DXGI_FORMAT_R10G10B10A2_UNORM;
				}
				if (IS_BITMASK(0x0000ffff, 0xffff0000, 0, 0))
				{
					return DXGI_FORMAT_R16G16_UNORM;
				}
				if (IS_BITMASK(0xffffffff, 0, 0, 0))
				{
					return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
				}
				break;
			case 24:
				break;
			case 16:
				if (IS_BITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
				{
					return DXGI_FORMAT_B5G5R5A1_UNORM;
				}
				if (IS_BITMASK(0xf800, 0x07e0, 0x001f, 0))
				{
					return DXGI_FORMAT_B5G6R5_UNORM;
				}
				if (IS_BITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
				{
					return DXGI_FORMAT_B4G4R4A4_UNORM;
				}
				break;
			}
		}
		else if (ddpf.flags & DDS_LUMINANCE)
		{
			if (8 == ddpf.RGBBitCount)
			{
				if (IS_BITMASK(0xff, 0, 0, 0))
				{
					return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
				}
				if (IS_BITMASK(0x00ff, 0, 0, 0xff00))
				{
					return DXGI_FORMAT_R8G8_UNORM; // Some DDS writers assume the bitcount should be 8 instead of 16
				}
			}
			if (16 == ddpf.RGBBitCount)
			{
				if (IS_BITMASK(0xffff, 0, 0, 0))
				{
					return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
				}
				if (IS_BITMASK(0x00ff, 0, 0, 0xff00))
				{
					return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
				}
			}
		}
		else if (ddpf.flags & DDS_ALPHA)
		{
			if (8 == ddpf.RGBBitCount)
			{
				return DXGI_FORMAT_A8_UNORM;
			}
		}
		else if (ddpf.flags & DDS_BUMPDUDV)
		{
			if (16 == ddpf.RGBBitCount)
			{
				if (IS_BITMASK(0x00ff, 0xff00, 0, 0))
				{
					return DXGI_FORMAT_R8G8_SNORM; // D3DX10/11 writes this out as DX10 extension
				}
			}

			if (32 == ddpf.RGBBitCount)
			{
				if (IS_BITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				{
					return DXGI_FORMAT_R8G8B8A8_SNORM; // D3DX10/11 writes this out as DX10 extension
				}
				if (IS_BITMASK(0x0000ffff, 0xffff0000, 0, 0))
				{
					return DXGI_FORMAT_R16G16_SNORM; // D3DX10/11 writes this out as DX10 extension
				}
			}
		}
		else if (ddpf.flags & DDS_FOURCC)
		{
			if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC1_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC2_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC3_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC2_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC3_UNORM;
			}
			if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC4_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC4_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC4_SNORM;
			}
			if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC5_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC5_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
			{
				return DXGI_FORMAT_BC5_SNORM;
			}
			if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
			{
				return DXGI_FORMAT_R8G8_B8G8_UNORM;
			}
			if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
			{
				return DXGI_FORMAT_G8R8_G8B8_UNORM;
			}
			if (MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf.fourCC)
			{
				return DXGI_FORMAT_YUY2;
			}
			switch (ddpf.fourCC)
			{
			case 36:  return DXGI_FORMAT_R16G16B16A16_UNORM;
			case 110: return DXGI_FORMAT_R16G16B16A16_SNORM;
			case 111: return DXGI_FORMAT_R16_FLOAT;
			case 112: return DXGI_FORMAT_R16G16_FLOAT;
			case 113: return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case 114: return DXGI_FORMAT_R32_FLOAT;
			case 115: return DXGI_FORMAT_R32G32_FLOAT;
			case 116: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}
		return DXGI_FORMAT_UNKNOWN;
#undef IS_BITMASK
	}

	static uint32_t getBytesInfos(uint32_t width, uint32_t height, EFormat format, __out uint32_t& sliceBytes, __out uint32_t& rowBytes)
	{
		bool8_t compressed = false;
		bool8_t planar = false;
		bool8_t packed = false;
		uint32_t rows = 0u;
		uint32_t bytes = 0u;

		switch (format)
		{
		case EFormat::BC1_Typeless:
		case EFormat::BC1_UNorm:
		case EFormat::BC1_UNorm_SRGB:
		case EFormat::BC4_Typeless:
		case EFormat::BC4_UNorm:
		case EFormat::BC4_SNorm:
			compressed = true;
			bytes = 8u;
			break;
		case EFormat::BC2_Typeless:
		case EFormat::BC2_UNorm:
		case EFormat::BC2_UNorm_SRGB:
		case EFormat::BC3_Typeless:
		case EFormat::BC3_UNorm:
		case EFormat::BC3_UNorm_SRGB:
		case EFormat::BC5_Typeless:
		case EFormat::BC5_UNorm:
		case EFormat::BC5_SNorm:
		case EFormat::BC6H_Typeless:
		case EFormat::BC6H_UF16:
		case EFormat::BC6H_SF16:
		case EFormat::BC7_Typeless:
		case EFormat::BC7_UNorm:
		case EFormat::BC7_UNorm_SRGB:
			compressed = true;
			bytes = 16u;
			break;
		case EFormat::RG8_BG8_UNorm:
		case EFormat::GR8_GB8_UNorm:
		case EFormat::YUY2:
			packed = true;
			bytes = 4u;
			break;
		case EFormat::Y210:
		case EFormat::Y216:
			packed = true;
			bytes = 8u;
			break;
		case EFormat::NV12:
		case EFormat::Opaque_420:
			planar = true;
			bytes = 2u;
			break;
		case EFormat::P010:
		case EFormat::P016:
			planar = true;
			bytes = 4u;
			break;
		default:
			break;
		}

		if (compressed)
		{
			uint32_t blockWide = 0u;
			if (width > 0u)
			{
				blockWide = std::max<uint32_t>(1u, (width + 3u) / 4u);
			}
			uint32_t blockHigh = 0u;
			if (height > 0u)
			{
				blockHigh = std::max<uint32_t>(1u, (height + 3u) / 4u);
			}
			rowBytes = blockWide * bytes;
			rows = blockHigh;
			sliceBytes = rowBytes * blockHigh;
		}
		else if (packed)
		{
			rowBytes = ((width + 1u) >> 1u) * bytes;
			rows = height;
			sliceBytes = rowBytes * height;
		}
		else if (format == EFormat::NV11)
		{
			rowBytes = ((width + 3u) >> 2u) * 4u;
			rows = height * 2u;
			sliceBytes = rowBytes * rows;
		}
		else if (planar)
		{
			rowBytes = ((width + 1u) >> 1u) * bytes;
			sliceBytes = (rowBytes * height) + ((rowBytes * height + 1u) >> 1u);
			rows = height + ((height + 1u) >> 1u);
		}
		else
		{
			bytes = FormatAttribute::attribute(format).Stride;
			assert(bytes);

			rowBytes = (width * bytes + 7u) / 8u;
			rows = height;
			sliceBytes = rowBytes * height;
		}

		return rows;
	}

	static TextureDesc load_General(const byte8_t* data, size_t size)
	{
		int32_t width = 0u;
		int32_t height = 0u;
		int32_t channels = 0u;
		auto pixels = stbi_load_from_memory(data, static_cast<int32_t>(size), &width, &height, &channels, STBI_rgb_alpha);

		TextureDesc desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = EFormat::RGBA8_UNorm;
		desc.Dimension = ETextureType::T_2D;
		desc.Subresources.resize(1u);
		desc.Subresources[0].Width = width;
		desc.Subresources[0].Height = height;
		desc.BindFlags = EBindFlags::Bind_ShaderResource;
		getBytesInfos(width, height, EFormat::RGBA8_UNorm, desc.Subresources[0].SliceBytes, desc.Subresources[0].RowBytes);
		uint32_t bytes = width * FormatAttribute::attribute(EFormat::RGBA8_UNorm).Stride / static_cast<uint32_t>(BITS_IN_BYTES()) * height;
		assert(bytes == desc.Subresources[0].SliceBytes);
		desc.Data.reset(new byte8_t[bytes]());
		VERIFY(memcpy_s(desc.Data.get(), bytes, pixels, bytes) == 0);

		stbi_image_free(pixels);

		return desc;
	}

	static TextureDesc load_DDS(const byte8_t* data, size_t size)
	{
		assert(data && size);
		assert(size >= (sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)));
		assert(*reinterpret_cast<const uint32_t*>(data) == DirectX::DDS_MAGIC);

		bool8_t dxt10Header = false;
		auto header = reinterpret_cast<const DirectX::DDS_HEADER*>(data + sizeof(uint32_t));
		assert(header->size == sizeof(DirectX::DDS_HEADER) && header->ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));
		if ((header->ddspf.flags & DDS_FOURCC) && (header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0')))
		{
			assert(size >= (sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10)));
			dxt10Header = true;
		}

		auto offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + (dxt10Header ? sizeof(DirectX::DDS_HEADER_DXT10) : 0u);
		auto bitSize = size - offset;
		auto bitData = data + offset;

		bool8_t cubemap = false;
		TextureDesc desc{};
		desc.Width = header->width;
		desc.Height = header->height;
		desc.Depth = header->depth == 0u ? 1u : header->depth;
		desc.MipLevels = header->mipMapCount == 0u ? 1u : header->mipMapCount;
		desc.BindFlags = EBindFlags::Bind_ShaderResource;
		assert(desc.MipLevels <= D3D11_REQ_MIP_LEVELS);

		if (dxt10Header)
		{
			auto dxtHeader = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const uint8_t*>(header) + sizeof(DirectX::DDS_HEADER));
			assert(dxtHeader && dxtHeader->arraySize);

			desc.ArraySize = dxtHeader->arraySize;
			assert(dxtHeader->dxgiFormat != DXGI_FORMAT_AI44 && 
				dxtHeader->dxgiFormat != DXGI_FORMAT_IA44 &&
				dxtHeader->dxgiFormat != DXGI_FORMAT_P8 &&
				dxtHeader->dxgiFormat != DXGI_FORMAT_A8P8);
			auto formatAttr = FormatAttribute::attribute_DXGI(dxtHeader->dxgiFormat);
			assert(formatAttr.Stride);

			desc.Format = formatAttr.Format;
			switch (dxtHeader->resourceDimension)
			{
			case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
				if (header->flags & DDS_HEIGHT)
				{
					assert(desc.Height == 1u);
				}
				desc.Height = desc.Depth = 1u;
				desc.Dimension = desc.ArraySize > 1u ? ETextureType::T_1D_Array : ETextureType::T_1D;
				break;
			case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
				if (dxtHeader->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
				{
					desc.Dimension = desc.ArraySize > 1u ? ETextureType::T_Cube_Array : ETextureType::T_Cube;
					desc.ArraySize *= 6u;
					cubemap = true;
				}
				else
				{
					desc.Dimension = desc.ArraySize > 1u ? ETextureType::T_2D_Array : ETextureType::T_2D;
				}
				desc.Depth = 1u;
				break;
			case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
				assert(header->flags & DDS_HEADER_FLAGS_VOLUME);
				desc.Dimension = ETextureType::T_3D;
				break;
			case D3D11_RESOURCE_DIMENSION_BUFFER:
				desc.Dimension = ETextureType::Buffer;
				break;
			}
		}
		else
		{
			auto formatAttr = FormatAttribute::attribute_DXGI(pixelFormatToDXGIFormat(header->ddspf));
			assert(formatAttr.Format != EFormat::Unknown && formatAttr.Stride);
			desc.Format = formatAttr.Format;

			if (header->flags & DDS_HEADER_FLAGS_VOLUME)
			{
				desc.Dimension = ETextureType::T_3D;
			}
			else
			{
				if (header->caps2 & DDS_CUBEMAP)
				{
					assert((header->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
					desc.ArraySize = 6u;
					cubemap = true;
					desc.Dimension = ETextureType::T_Cube;
				}
				else
				{
					desc.Dimension = desc.ArraySize > 1u ? ETextureType::T_2D_Array : ETextureType::T_2D;
				}
			}
		}

		switch (desc.Dimension)
		{
		case ETextureType::T_2D:
		case ETextureType::T_2D_Array:
			assert(desc.ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
				desc.Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION &&
				desc.Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
			break;
		case ETextureType::T_Cube:
		case ETextureType::T_Cube_Array:
			assert(desc.ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
				desc.Width <= D3D11_REQ_TEXTURECUBE_DIMENSION &&
				desc.Height <= D3D11_REQ_TEXTURECUBE_DIMENSION);
			break;
		case ETextureType::T_3D:
			assert(desc.ArraySize == 1u && 
			desc.Width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
			desc.Height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
			desc.Depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION);
			break;
		}

		/// gen mipmaps
		desc.Data.reset(new byte8_t[bitSize]());
		VERIFY(memcpy_s(desc.Data.get(), bitSize, bitData, bitSize) == 0);
		desc.Subresources.resize(desc.MipLevels * desc.ArraySize);
		uint32_t index = 0u;
		for (uint32_t arrayIndex = 0u; arrayIndex < desc.ArraySize; ++arrayIndex)
		{
			uint32_t width = desc.Width;
			uint32_t height = desc.Height;
			uint32_t depth = desc.Depth;
			for (uint32_t mipIndex = 0u; mipIndex < desc.MipLevels; ++mipIndex)
			{
				desc.Subresources[index].Width = width;
				desc.Subresources[index].Height = height;
				desc.Subresources[index].Depth = depth;
				desc.Subresources[index].MipIndex = mipIndex;
				desc.Subresources[index].ArrayIndex = arrayIndex;

				getBytesInfos(width, height, desc.Format, desc.Subresources[index].SliceBytes, desc.Subresources[index].RowBytes);
				assert((bitData + desc.Subresources[index].SliceBytes * depth) <= (bitData + bitSize));
				desc.Subresources[index].Offset = index == 0u ? 0u : desc.Subresources[index].Offset + desc.Subresources[index].SliceBytes * depth;

				width = width >> 1;
				height = height >> 1;
				depth = depth >> 1;
				width = width == 0u ? 1u : width;
				height = height == 0u ? 1u : height;
				depth = depth == 0u ? 1u : depth;
				
				++index;
			}
		}

		return desc;
	}

	static TextureDesc load_KTX(const byte8_t* data, size_t size)
	{
		TextureDesc desc{};
		ktxTexture *ktx = nullptr;
		VERIFY(KTX_SUCCESS == ktxTexture_CreateFromMemory(data, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx));

		desc.Width = ktx->baseWidth;
		desc.Height = ktx->baseHeight;
		desc.Depth = ktx->baseDepth;
		desc.MipLevels = ktx->numLevels;
		desc.ArraySize = ktx->numLayers;
		desc.BindFlags = EBindFlags::Bind_ShaderResource;
		switch (ktx->numDimensions)
		{
		case 1: desc.Dimension = ktx->isArray ? 
			ETextureType::T_1D_Array : ETextureType::T_1D; 
			break;
		case 2: desc.Dimension = ktx->isCubemap ? 
			(ktx->isArray ? ETextureType::T_Cube_Array : ETextureType::T_Cube) : 
			(ktx->isArray ? ETextureType::T_2D_Array : ETextureType::T_2D); 
			break;
		case 3:
			desc.Dimension = ETextureType::T_3D;
			break;
		default:
			assert(0);
			break;
		}

		FormatAttribute formatAttr{};
		if (ktx->classId == ktxTexture1_c)
		{
			ktxTexture1* ktx1 = reinterpret_cast<ktxTexture1*>(ktx);
			formatAttr = FormatAttribute::attribute_GL(ktx1->glInternalformat);
		}
		else
		{
			ktxTexture2* ktx2 = reinterpret_cast<ktxTexture2*>(ktx);
			formatAttr = FormatAttribute::attribute_Vk(ktx2->vkFormat);
		}
		assert(formatAttr.Format != EFormat::Unknown);

		desc.Format = formatAttr.Format;
		desc.Subresources.resize(ktx->numLevels * ktx->numLayers);

		uint32_t index = 0u;
		for (uint32_t arrayIndex = 0u; arrayIndex < desc.ArraySize; ++arrayIndex)
		{
			uint32_t width = desc.Width;
			uint32_t height = desc.Height;
			uint32_t depth = desc.Depth;
			for (uint32_t mipIndex = 0u; mipIndex < desc.MipLevels; ++mipIndex)
			{
				size_t offset = 0u;
				VERIFY(ktxTexture_GetImageOffset(ktx, mipIndex, arrayIndex, 0u, &offset) == KTX_SUCCESS); /// Wrong param order...

				desc.Subresources[index].Width = width;
				desc.Subresources[index].Height = height;
				desc.Subresources[index].Depth = depth;
				getBytesInfos(width, height, desc.Format, desc.Subresources[index].SliceBytes, desc.Subresources[index].RowBytes);

				desc.Subresources[index].Offset = static_cast<uint32_t>(offset);
				assert(ktxTexture_GetImageSize(ktx, mipIndex) == desc.Subresources[index].SliceBytes);

				width = width >> 1;
				height = height >> 1;
				depth = depth >> 1;
				width = width == 0u ? 1u : width;
				height = height == 0u ? 1u : height;
				depth = depth == 0u ? 1u : depth;

				++index;
			}
		}

		auto bytes = ktxTexture_GetDataSize(ktx);
		desc.Data.reset(new byte8_t[bytes]());
		VERIFY(memcpy_s(desc.Data.get(), bytes, ktxTexture_GetData(ktx), bytes) == 0);
		ktxTexture_Destroy(ktx);

		return desc;
	}

private:
	static std::vector<TextureFormatAttribute> s_SupportedFormats;
};

std::vector<TextureFormatAttribute> TextureLoader::s_SupportedFormats =
{
	{ ETextureFormat::DDS,  ".dds", "DirectDraw Surface" },
	{ ETextureFormat::KTX,  ".ktx", "Khronos Texture" },
	{ ETextureFormat::PNG,  ".png", "Portable Network Graphics" },
	{ ETextureFormat::JPEG, ".jpg", "Joint Photographic Experts Group" },
	{ ETextureFormat::BMP,  ".bmp", "Bitmap" },
	{ ETextureFormat::TGA,  ".tga", "Truevision Graphics Adapter" },
	{ ETextureFormat::PSD,  ".psd", "Photoshop Document" },
	{ ETextureFormat::GIF,  ".gif", "Graphics Interchange Format" },
	{ ETextureFormat::HDR,  ".hdr", "High Dynamic Range" },
	{ ETextureFormat::PIC,  ".pic", "QuickTime Picture?" },
	{ ETextureFormat::PNM,  ".pgm", "Portable Anymap Format" }  /// Or .ppm
};

NAMESPACE_END(Gfx)