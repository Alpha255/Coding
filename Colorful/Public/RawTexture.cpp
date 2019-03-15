#include "RawTexture.h"
#include "ResourceFile.h"
#include <d3d11.h>
#include "dds.h"
#include "System.h"
#include <vulkan.h>

size_t RawTexture::GetBitsPerPixel(uint32_t format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128U;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96U;

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
		return 64U;

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
		return 32U;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		return 24U;

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
		return 16U;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
	case DXGI_FORMAT_NV11:
		return 12U;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
		return 8U;

	case DXGI_FORMAT_R1_UNORM:
		return 1U;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 4U;

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
		return 8;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	case DXGI_FORMAT_V408:
		return 24U;

	case DXGI_FORMAT_P208:
	case DXGI_FORMAT_V208:
		return 16U;
#endif /// (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	default:
		return 0;
	}
}

uint32_t RawTexture::GetDXGIFormat(const DirectX::DDS_PIXELFORMAT &ddpf)
{
	#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

	if (ddpf.flags & DDS_RGB)
	{
		/// Note that sRGB formats are written using the "DX10" extended header

		switch (ddpf.RGBBitCount)
		{
		case 32:
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
			{
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			}

			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
			{
				return DXGI_FORMAT_B8G8R8X8_UNORM;
			}

			/// No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

			/// Note that many common DDS reader/writers (including D3DX) swap the
			/// the RED/BLUE masks for 10:10:10:2 formats. We assume
			/// below that the 'backwards' header mask is being used since it is most
			/// likely written by D3DX. The more robust solution is to use the 'DX10'
			/// header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

			/// For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
			if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
			{
				return DXGI_FORMAT_R10G10B10A2_UNORM;
			}

			/// No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16G16_UNORM;
			}

			if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
			{
				/// Only 32-bit color channel format in D3D9 was R32F
				return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
			}
			break;

		case 24:
			/// No 24bpp DXGI formats aka D3DFMT_R8G8B8
			break;

		case 16:
			if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
			{
				return DXGI_FORMAT_B5G5R5A1_UNORM;
			}
			if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
			{
				return DXGI_FORMAT_B5G6R5_UNORM;
			}

			/// No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5

			if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
			{
				return DXGI_FORMAT_B4G4R4A4_UNORM;
			}

			/// No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4

			/// No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
			break;
		}
	}
	else if (ddpf.flags & DDS_LUMINANCE)
	{
		if (8 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R8_UNORM; /// D3DX10/11 writes this out as DX10 extension
			}

			/// No DXGI format maps to ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4

			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
			{
				return DXGI_FORMAT_R8G8_UNORM; /// Some DDS writers assume the bitcount should be 8 instead of 16
			}
		}

		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16_UNORM; /// D3DX10/11 writes this out as DX10 extension
			}
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
			{
				return DXGI_FORMAT_R8G8_UNORM; /// D3DX10/11 writes this out as DX10 extension
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
			if (ISBITMASK(0x00ff, 0xff00, 0x0000, 0x0000))
			{
				return DXGI_FORMAT_R8G8_SNORM; /// D3DX10/11 writes this out as DX10 extension
			}
		}

		if (32 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				return DXGI_FORMAT_R8G8B8A8_SNORM; /// D3DX10/11 writes this out as DX10 extension
			}
			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16G16_SNORM; /// D3DX10/11 writes this out as DX10 extension
			}

			/// No DXGI format maps to ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) aka D3DFMT_A2W10V10U10
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

		/// While pre-multiplied alpha isn't directly supported by the DXGI formats,
		/// they are basically the same as these BC formats so they can be mapped
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

		/// BC6H and BC7 are written using the "DX10" extended header

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

		/// Check for D3DFORMAT enums being set here
		switch (ddpf.fourCC)
		{
		case 36: /// D3DFMT_A16B16G16R16
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		case 110: /// D3DFMT_Q16W16V16U16
			return DXGI_FORMAT_R16G16B16A16_SNORM;

		case 111: /// D3DFMT_R16F
			return DXGI_FORMAT_R16_FLOAT;

		case 112: /// D3DFMT_G16R16F
			return DXGI_FORMAT_R16G16_FLOAT;

		case 113: /// D3DFMT_A16B16G16R16F
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case 114: /// D3DFMT_R32F
			return DXGI_FORMAT_R32_FLOAT;

		case 115: /// D3DFMT_G32R32F
			return DXGI_FORMAT_R32G32_FLOAT;

		case 116: /// D3DFMT_A32B32G32R32F
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;

	#undef ISBITMASK
}

void RawTexture::TranslateToVulkanDimension()
{
	switch (Dimension)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D: Dimension = VK_IMAGE_TYPE_1D; break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D: Dimension = VK_IMAGE_TYPE_2D; break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D: Dimension = VK_IMAGE_TYPE_3D; break;
	default:
		assert(0);
		break;
	}
}

void RawTexture::TranslateToVulkanFormat()
{
	switch (Format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:     Format = VK_FORMAT_R8G8B8A8_UNORM;           break;
	case DXGI_FORMAT_B8G8R8A8_UNORM:     Format = VK_FORMAT_B8G8R8A8_UNORM;           break;
	case DXGI_FORMAT_R16G16_UNORM:       Format = VK_FORMAT_R16G16_UNORM;             break;
	case DXGI_FORMAT_R32_FLOAT:          Format = VK_FORMAT_R32_SFLOAT;               break;
	case DXGI_FORMAT_R8_UNORM:           Format = VK_FORMAT_R8_UNORM;                 break;
	case DXGI_FORMAT_R8G8_UNORM:         Format = VK_FORMAT_R8G8_UNORM;               break;
	case DXGI_FORMAT_R16_UNORM:          Format = VK_FORMAT_R16_UNORM;                break;
	case DXGI_FORMAT_R8G8_SNORM:         Format = VK_FORMAT_R8G8_SNORM;               break;
	case DXGI_FORMAT_R8G8B8A8_SNORM:     Format = VK_FORMAT_R8G8B8A8_SNORM;           break;
	case DXGI_FORMAT_R16G16_SNORM:       Format = VK_FORMAT_R16G16_SNORM;             break;
	case DXGI_FORMAT_BC1_UNORM:          Format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;     break;
	case DXGI_FORMAT_BC2_UNORM:          Format = VK_FORMAT_BC2_UNORM_BLOCK;          break;
	case DXGI_FORMAT_BC3_UNORM:          Format = VK_FORMAT_BC3_UNORM_BLOCK;          break;
	case DXGI_FORMAT_BC4_UNORM:          Format = VK_FORMAT_BC4_UNORM_BLOCK;          break;
	case DXGI_FORMAT_BC4_SNORM:          Format = VK_FORMAT_BC4_SNORM_BLOCK;          break;
	case DXGI_FORMAT_BC5_UNORM:          Format = VK_FORMAT_BC5_UNORM_BLOCK;          break;
	case DXGI_FORMAT_BC5_SNORM:          Format = VK_FORMAT_BC5_SNORM_BLOCK;          break;
	case DXGI_FORMAT_R16G16B16A16_UNORM: Format = VK_FORMAT_R16G16B16A16_UNORM;       break;
	case DXGI_FORMAT_R16G16B16A16_SNORM: Format = VK_FORMAT_R16G16B16A16_SNORM;       break;
	case DXGI_FORMAT_R16_FLOAT:          Format = VK_FORMAT_R16_SFLOAT;               break;
	case DXGI_FORMAT_R16G16_FLOAT:       Format = VK_FORMAT_R16G16_SFLOAT;            break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: Format = VK_FORMAT_R16G16B16A16_SFLOAT;      break;
	case DXGI_FORMAT_R32G32_FLOAT:       Format = VK_FORMAT_R32G32_SFLOAT;            break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT: Format = VK_FORMAT_R32G32B32A32_SFLOAT;      break;
	case DXGI_FORMAT_UNKNOWN:            Format = VK_FORMAT_UNDEFINED;                break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:  Format = VK_FORMAT_A2R10G10B10_UNORM_PACK32; break;
	case DXGI_FORMAT_B5G5R5A1_UNORM:     Format = VK_FORMAT_B5G5R5A1_UNORM_PACK16;    break;
	case DXGI_FORMAT_B5G6R5_UNORM:       Format = VK_FORMAT_B5G6R5_UNORM_PACK16;      break;
	case DXGI_FORMAT_B4G4R4A4_UNORM:     Format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;    break;

	///case DXGI_FORMAT_B8G8R8X8_UNORM: Format = VK_FORMAT_B8G8R8X8_UNORM; break;
	///case DXGI_FORMAT_A8_UNORM: Format = VK_FORMAT_A8_UNORM; break;
	///case DXGI_FORMAT_R8G8_B8G8_UNORM: Format = VK_FORMAT_R8G8_B8G8_UNORM; break;
	///case DXGI_FORMAT_G8R8_G8B8_UNORM: Format = VK_FORMAT_G8R8_G8B8_UNORM; break;
	///case DXGI_FORMAT_YUY2: Format = VK_FORMAT_YUY2; break;

	default:
		System::Log("Not supported yet!");
		assert(0);
		break;
	}
}

void RawTexture::CreateFromDds(const char *pFileName, bool bUsingVulkan)
{
	ResourceFile ddsFile(pFileName);
	assert(ddsFile.IsValid());

	RawTex2D = gli::texture2d(gli::load(ddsFile.GetFilePath()));
	assert(!RawTex2D.empty());

	Width = RawTex2D[0].extent().x;
	Height = RawTex2D[0].extent().y;
	MipLevels = (uint32_t)RawTex2D.levels();
	Depth = 1U;
	ArraySize = 1U;
	Dimension = VK_IMAGE_TYPE_2D;
	RawSize = RawTex2D.size();
	RawData.reset((uint8_t *)RawTex2D[0].data());

	SetupMipMaps();

#if 0

	RawSize = ddsFile.GetSize();

	RawData.reset(ddsFile.Load());

	assert(RawSize >= sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t));

	const uint8_t *pData = RawData.get();

	assert(*reinterpret_cast<const uint32_t *>(pData) == DirectX::DDS_MAGIC);

	auto pHeader = reinterpret_cast<const DirectX::DDS_HEADER *>(pData + sizeof(uint32_t));
	assert(pHeader->size == sizeof(DirectX::DDS_HEADER) && pHeader->ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));

	bool bDXT10Header = false;
	if ((pHeader->ddspf.flags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == pHeader->ddspf.fourCC))
	{
		assert(ddsFile.GetSize() >= sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10));
		bDXT10Header = true;
	}

	Width = pHeader->width;
	Height = pHeader->height;
	Depth = pHeader->depth;
	MipLevels = pHeader->mipMapCount;
	ArraySize = 1U;

	if (bDXT10Header)
	{
		auto pD3D10Ext = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const char*>(pHeader) + sizeof(DirectX::DDS_HEADER));
		ArraySize = pD3D10Ext->arraySize;
		assert(ArraySize > 0U);

		switch (pD3D10Ext->dxgiFormat)
		{
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
		case DXGI_FORMAT_A8P8:
			assert(!"ERROR: DDSTextureLoader does not support video textures. Consider using DirectXTex instead.\n");
			break;
		default:
			assert(GetBitsPerPixel(pD3D10Ext->dxgiFormat) != 0U);
			break;
		}

		Format = pD3D10Ext->dxgiFormat;

		switch (pD3D10Ext->resourceDimension)
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			assert((pHeader->flags & DDS_HEIGHT) && Height == 1U);
			Height = Depth = 1U;
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			if (pD3D10Ext->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				ArraySize *= 6U;
				CubeMap = true;
			}
			Depth = 1U;
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			assert((pHeader->flags & DDS_HEADER_FLAGS_VOLUME) && ArraySize <= 1U);
			break;
		default:
			assert(!"ERROR: Unknown resource dimensio!");
		}

		Dimension = pD3D10Ext->resourceDimension;
	}
	else
	{
		Format = GetDXGIFormat(pHeader->ddspf);
		assert(Format != DXGI_FORMAT_UNKNOWN);

		if (pHeader->flags & DDS_HEADER_FLAGS_VOLUME)
		{
			Dimension = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
		}
		else
		{
			if (pHeader->caps2 & DDS_CUBEMAP)
			{
				assert((pHeader->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
				ArraySize = 6U;
				CubeMap = true;
			}

			Depth = 1;
			Dimension = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
		}
		assert(GetBitsPerPixel(Format) != 0);
	}

	assert(MipLevels <= D3D11_REQ_MIP_LEVELS);

	switch (Dimension)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		assert(ArraySize <= D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION && Width <= D3D11_REQ_TEXTURE1D_U_DIMENSION);
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		if (CubeMap)
		{
			assert(ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION && Width <= D3D11_REQ_TEXTURECUBE_DIMENSION && Height <= D3D11_REQ_TEXTURECUBE_DIMENSION);
		}
		else
		{
			assert(ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION && Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION && Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		assert(ArraySize <= 1U && Width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION && Height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION && Depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION);
		break;
	default:
		assert(!"ERROR: Unknown resource dimensio!");
	}

	if (bUsingVulkan)
	{
		TranslateToVulkanDimension();

		TranslateToVulkanFormat();
	}

	SetupMipMaps();
#endif
}

void RawTexture::SetupMipMaps()
{
	RawMipTextures.resize(MipLevels);

	for (uint32_t i = 0U; i < MipLevels; ++i)
	{
		RawMipTextures[i].MipWidth = RawTex2D[i].extent().x;
		RawMipTextures[i].MipHeight = RawTex2D[i].extent().y;
		RawMipTextures[i].MipSize = RawTex2D[i].size();
		RawMipTextures[i].RawMipData = (uint8_t *)RawTex2D[i].data();
		RawMipTextures[i].Offset = (ptrdiff_t)(RawMipTextures[i].RawMipData - RawData.get());
	}
}