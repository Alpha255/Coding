#include "D3D11Util.h"
#include "Util/dds.h"
#include "Util/ResourceFile.h"
#include "D3D11Engine.h"
#include "D3D11Texture.h"

/// Helper functions for texture loaders and screen grabber
///
/// Copyright (c) Microsoft Corporation. All rights reserved.
/// Licensed under the MIT License.

NamespaceBegin(D3DUtil)

#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

inline size_t GetBitsPerPixel(DXGI_FORMAT fmt)
{
	switch (fmt)
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

inline DXGI_FORMAT GetDXGIFormat(const DirectX::DDS_PIXELFORMAT& ddpf)
{
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
}

inline DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case DXGI_FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	default:
		return format;
	}
}

inline void GetSurfaceInfo(
	_In_ size_t width,
	_In_ size_t height,
	_In_ DXGI_FORMAT fmt,
	_Out_opt_ size_t *outNumBytes,
	_Out_opt_ size_t *outRowBytes,
	_Out_opt_ size_t *outNumRows)
{
	uint64_t numBytes = 0;
	uint64_t rowBytes = 0;
	uint64_t numRows = 0;

	bool bc = false;
	bool packed = false;
	bool planar = false;
	size_t bpe = 0;
	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bc = true;
		bpe = 8;
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
		bc = true;
		bpe = 16;
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_YUY2:
		packed = true;
		bpe = 4;
		break;
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		packed = true;
		bpe = 8;
		break;
	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	case DXGI_FORMAT_P208:
#endif
		planar = true;
		bpe = 2;
		break;
	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		planar = true;
		bpe = 4;
		break;
	default:
		break;
	}

	if (bc)
	{
		uint64_t numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = std::max<uint64_t>(1u, (uint64_t(width) + 3u) / 4u);
		}
		uint64_t numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = std::max<uint64_t>(1u, (uint64_t(height) + 3u) / 4u);
		}
		rowBytes = numBlocksWide * bpe;
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if (packed)
	{
		rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
		numRows = uint64_t(height);
		numBytes = rowBytes * height;
	}
	else if (fmt == DXGI_FORMAT_NV11)
	{
		rowBytes = ((uint64_t(width) + 3u) >> 2) * 4u;
		numRows = uint64_t(height) * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
		numBytes = rowBytes * numRows;
	}
	else if (planar)
	{
		rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
		numBytes = (rowBytes * uint64_t(height)) + ((rowBytes * uint64_t(height) + 1u) >> 1);
		numRows = height + ((uint64_t(height) + 1u) >> 1);
	}
	else
	{
		size_t bpp = GetBitsPerPixel(fmt);
		assert(bpp);

		rowBytes = (uint64_t(width) * bpp + 7u) / 8u; // round up to nearest byte
		numRows = uint64_t(height);
		numBytes = rowBytes * height;
	}

#if defined(_M_IX86) || defined(_M_ARM) || defined(_M_HYBRID_X86_ARM64)
	static_assert(sizeof(size_t) == 4, "Not a 32-bit platform!");
	assert(numBytes <= UINT32_MAX && rowBytes <= UINT32_MAX && numRows <= UINT32_MAX);
#else
	static_assert(sizeof(size_t) == 8, "Not a 64-bit platform!");
#endif

	if (outNumBytes)
	{
		*outNumBytes = static_cast<size_t>(numBytes);
	}
	if (outRowBytes)
	{
		*outRowBytes = static_cast<size_t>(rowBytes);
	}
	if (outNumRows)
	{
		*outNumRows = static_cast<size_t>(numRows);
	}
}

void FillInitData(
	_In_ uint32_t width,
	_In_ uint32_t height,
	_In_ uint32_t depth,
	_In_ uint32_t mipCount,
	_In_ uint32_t arraySize,
	_In_ DXGI_FORMAT format,
	_In_ size_t maxsize,
	_In_ size_t bitSize,
	_In_reads_bytes_(bitSize) const uint8_t *bitData,
	_Out_ size_t &twidth,
	_Out_ size_t &theight,
	_Out_ size_t &tdepth,
	_Out_ size_t &skipMip,
	_Out_writes_(mipCount * arraySize) D3D11_SUBRESOURCE_DATA *initData)
{
	assert(bitData && initData);

	skipMip = 0;
	twidth = 0;
	theight = 0;
	tdepth = 0;

	size_t NumBytes = 0;
	size_t RowBytes = 0;
	const uint8_t *pSrcBits = bitData;
	const uint8_t *pEndBits = bitData + bitSize;

	size_t index = 0;
	for (size_t j = 0; j < arraySize; j++)
	{
		size_t w = width;
		size_t h = height;
		size_t d = depth;
		for (size_t i = 0; i < mipCount; i++)
		{
			GetSurfaceInfo(w, h, format, &NumBytes, &RowBytes, nullptr);

			assert(NumBytes <= UINT32_MAX && RowBytes <= UINT32_MAX);

			if ((mipCount <= 1) || !maxsize || (w <= maxsize && h <= maxsize && d <= maxsize))
			{
				if (!twidth)
				{
					twidth = w;
					theight = h;
					tdepth = d;
				}

				assert(index < mipCount * arraySize);
				_Analysis_assume_(index < mipCount * arraySize);
				initData[index].pSysMem = pSrcBits;
				initData[index].SysMemPitch = static_cast<UINT>(RowBytes);
				initData[index].SysMemSlicePitch = static_cast<UINT>(NumBytes);
				++index;
			}
			else if (!j)
			{
				// Count number of skipped mipmaps (first item only)
				++skipMip;
			}

			assert((pSrcBits + (NumBytes * d)) <= pEndBits);

			pSrcBits += NumBytes * d;

			w = w >> 1;
			h = h >> 1;
			d = d >> 1;
			if (w == 0)
			{
				w = 1;
			}
			if (h == 0)
			{
				h = 1;
			}
			if (d == 0)
			{
				d = 1;
			}
		}
	}

	assert(index > 0);
}

void UpdateSubResource(
	uint32_t format,
	uint32_t width,
	uint32_t height,
	uint32_t arraySize,
	uint32_t mipCount,
	size_t bitSize,
	const uint8_t *pBitData,
	ID3D11Resource *pResource,
	ID3D11ShaderResourceView *pShaderResourceView)
{
	size_t numBytes = 0U;
	size_t rowBytes = 0U;
	GetSurfaceInfo(width, height, (DXGI_FORMAT)format, &numBytes, &rowBytes, nullptr);
	assert(numBytes <= bitSize);
	assert(numBytes <= UINT32_MAX && rowBytes <= UINT32_MAX);

	if (arraySize > 1U)
	{
		const uint8_t *pSrcBits = pBitData;
		const uint8_t *pEndBits = pBitData + bitSize;
		for (uint32_t i = 0U; i < arraySize; ++i)
		{
			assert((pSrcBits + numBytes) <= pEndBits);

			uint32_t res = D3D11CalcSubresource(0U, i, mipCount);
			D3D11Engine::Instance().GetIMContext()->UpdateSubresource(pResource, res, nullptr, pSrcBits, (uint32_t)rowBytes, (uint32_t)numBytes);
			pSrcBits += numBytes;
		}
	}
	else
	{
		D3D11Engine::Instance().GetIMContext()->UpdateSubresource(pResource, 0U, nullptr, pBitData, (uint32_t)rowBytes, (uint32_t)numBytes);
	}

	D3D11Engine::Instance().GetIMContext()->GenerateMips(pShaderResourceView);
}

void CreateShaderResourceView(
	bool autoGen,
	uint32_t dim, 
	uint32_t width, 
	uint32_t height, 
	uint32_t depth,
	uint32_t mipCount,
	uint32_t arraySize,
	uint32_t format,
	uint32_t usage,
	uint32_t bindFlags,
	uint32_t cpuAccessFlags,
	uint32_t miscFlags,
	bool bCubemap,
	const D3D11_SUBRESOURCE_DATA *pSubResData,
	size_t bitSize,
	const uint8_t *pBitData,
	_Out_ ID3D11ShaderResourceView **ppShaderResourceView)
{
	switch (dim)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
	{
		D3D11Texture1D texture1D;
		texture1D.Create(format, width, bindFlags, mipCount, arraySize, cpuAccessFlags, miscFlags, usage, (D3D11SubResourceData *)pSubResData);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = (DXGI_FORMAT)format;
		if (arraySize > 1U)
		{
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			desc.Texture1DArray.MipLevels = mipCount;
			desc.Texture1DArray.ArraySize = arraySize;
		}
		else
		{
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			desc.Texture1D.MipLevels = mipCount;
		}

		HRCheck(D3D11Engine::Instance().GetDevice()->CreateShaderResourceView(texture1D.Get(), &desc, ppShaderResourceView));

		if (autoGen)
		{
			UpdateSubResource(format, width, height, arraySize, mipCount, bitSize, pBitData, texture1D.Get(), *ppShaderResourceView);
		}
		break;
	}
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		D3D11Texture2D texture2D;
		texture2D.Create(format, width, height, bindFlags, mipCount, arraySize, cpuAccessFlags, miscFlags, usage, (D3D11SubResourceData *)pSubResData);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = (DXGI_FORMAT)format;

		if (bCubemap)
		{
			if (arraySize > 6U)
			{
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
				desc.TextureCubeArray.MipLevels = mipCount;
				desc.TextureCubeArray.NumCubes = arraySize / 6U;
			}
			else
			{
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				desc.TextureCube.MipLevels = mipCount;
			}
		}
		else if (arraySize > 1U)
		{
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipLevels = mipCount;
			desc.Texture2DArray.ArraySize = arraySize;
		}
		else
		{
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = mipCount;
		}

		HRCheck(D3D11Engine::Instance().GetDevice()->CreateShaderResourceView(texture2D.Get(), &desc, ppShaderResourceView));

		if (autoGen)
		{
			UpdateSubResource(format, width, height, arraySize, mipCount, bitSize, pBitData, texture2D.Get(), *ppShaderResourceView);
		}
		break;
	}
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
	{
		D3D11Texture3D texture3D;
		texture3D.Create(format, width, height, depth, bindFlags, mipCount, cpuAccessFlags, miscFlags, usage, (D3D11SubResourceData *)pSubResData);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = (DXGI_FORMAT)format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipLevels = mipCount;

		HRCheck(D3D11Engine::Instance().GetDevice()->CreateShaderResourceView(texture3D.Get(), &desc, ppShaderResourceView));

		if (autoGen)
		{
			UpdateSubResource(format, width, height, arraySize, mipCount, bitSize, pBitData, texture3D.Get(), *ppShaderResourceView);
		}
		break;
	}
	default:
		assert(!"Invalid resource dimension!");
		break;
	}
}

void CreateShaderResourceViewFromDds(const char *pDdsName, _Out_ ID3D11ShaderResourceView **ppShaderResourceView, bool bSRGB)
{
	assert(ppShaderResourceView && *ppShaderResourceView == nullptr);

	ResourceFile ddsFile(pDdsName);
	assert(ddsFile.IsValid());

	assert(ddsFile.GetSize() >= sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t));

	auto ddsTexData = std::unique_ptr<uint8_t>(ddsFile.Load());
	const uint8_t *pData = ddsTexData.get();

	assert(*reinterpret_cast<const uint32_t *>(pData) == DirectX::DDS_MAGIC);

	auto pHeader = reinterpret_cast<const DirectX::DDS_HEADER *>(pData + sizeof(uint32_t));
	assert(pHeader->size == sizeof(DirectX::DDS_HEADER) && pHeader->ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));

	bool bDXT10Header = false;
	if ((pHeader->ddspf.flags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == pHeader->ddspf.fourCC))
	{
		assert(ddsFile.GetSize() >= sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10));
		bDXT10Header = true;
	}

	ptrdiff_t offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + (bDXT10Header ? sizeof(DirectX::DDS_HEADER_DXT10) : 0U);

	const uint8_t *pBitData = pData + offset;
	const size_t bitSize = ddsFile.GetSize() - offset;

	uint32_t width = pHeader->width;
	uint32_t height = pHeader->height;
	uint32_t depth = pHeader->depth;
	uint32_t dim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
	uint32_t arraySize = 1U;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	uint32_t mipCount = pHeader->mipMapCount == 0 ? 1 : pHeader->mipMapCount;
	uint32_t miscFlags = 0U;
	bool bCubemap = false;

	if (bDXT10Header)
	{
		auto pD3D10Ext = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const char*>(pHeader) + sizeof(DirectX::DDS_HEADER));
		arraySize = pD3D10Ext->arraySize;
		assert(arraySize > 0U);

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

		format = pD3D10Ext->dxgiFormat;

		switch (pD3D10Ext->resourceDimension)
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			assert((pHeader->flags & DDS_HEIGHT) && height == 1U);
			height = depth = 1U;
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			if (pD3D10Ext->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				arraySize *= 6U;
				bCubemap = true;
			}
			depth = 1U;
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			assert((pHeader->flags & DDS_HEADER_FLAGS_VOLUME) && arraySize <= 1U);
			break;
		default:
			assert(!"ERROR: Unknown resource dimensio!");
		}

		dim = pD3D10Ext->resourceDimension;
	}
	else
	{
		format = GetDXGIFormat(pHeader->ddspf);
		assert(format != DXGI_FORMAT_UNKNOWN);

		if (pHeader->flags & DDS_HEADER_FLAGS_VOLUME)
		{
			dim = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
		}
		else
		{
			if (pHeader->caps2 & DDS_CUBEMAP)
			{
				assert((pHeader->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
				arraySize = 6U;
				bCubemap = true;
			}

			depth = 1;
			dim = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
		}
		assert(GetBitsPerPixel(format) != 0);
	}

	if ((miscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) && (dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D) && (arraySize % 6) == 0)
	{
		bCubemap = true;
	}

	assert(mipCount <= D3D11_REQ_MIP_LEVELS);

	switch (dim)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		assert(arraySize <= D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION && width <= D3D11_REQ_TEXTURE1D_U_DIMENSION);
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		if (bCubemap)
		{
			assert(arraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION && width <= D3D11_REQ_TEXTURECUBE_DIMENSION && height <= D3D11_REQ_TEXTURECUBE_DIMENSION);
		}
		else
		{
			assert(arraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION && width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION && height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		assert(arraySize <= 1U && width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION && height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION && depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION);
		break;
	default:
		assert(!"ERROR: Unknown resource dimensio!");
	}

	bool bAutoGen = false;
	if (1U == mipCount && ppShaderResourceView)
	{
		uint32_t supportFmt = 0U;
		HRCheck(D3D11Engine::Instance().GetDevice()->CheckFormatSupport(format, &supportFmt));

		if (supportFmt & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)
		{
			if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE3D || D3D11Engine::Instance().GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0)
			{
				bAutoGen = false;
			}
		}
	}

	if (bSRGB)
	{
		format = GetSRGBFormat(format);
	}

	if (bCubemap && dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
	{
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}
	else
	{
		miscFlags &= ~D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	if (bAutoGen)
	{
		mipCount = (!mipCount) ? -1 : mipCount;

		CreateShaderResourceView(
			true,
			dim, 
			width, 
			height, 
			depth, 
			mipCount, 
			arraySize, 
			format, 
			D3D11_USAGE_DEFAULT, 
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 
			0U, 
			miscFlags, 
			bCubemap, 
			nullptr, 
			bitSize, 
			pBitData,
			ppShaderResourceView);
	}
	else
	{
		size_t maxSize = ddsFile.GetSize();
		auto initData = std::unique_ptr<D3D11_SUBRESOURCE_DATA>(new D3D11_SUBRESOURCE_DATA[mipCount * arraySize]());

		size_t skipMip = 0U;
		size_t twidth = 0U;
		size_t theight = 0U;
		size_t tdepth = 0U;
		FillInitData(width, height, depth, mipCount, arraySize, format, maxSize, bitSize, pBitData, twidth, theight, tdepth, skipMip, initData.get());

		CreateShaderResourceView(
			false,
			dim,
			width,
			height,
			depth,
			mipCount,
			arraySize,
			format,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0U,
			miscFlags,
			bCubemap,
			initData.get(),
			bitSize,
			pBitData,
			ppShaderResourceView);

		if (0U == maxSize && mipCount > 1U)
		{
			assert(!"WTF!");
			switch (D3D11Engine::Instance().GetDevice()->GetFeatureLevel())
			{
			case D3D_FEATURE_LEVEL_9_1:
			case D3D_FEATURE_LEVEL_9_2:
				if (bCubemap)
				{
					maxSize = 512U;
				}
				else
				{
					maxSize = (dim == D3D11_RESOURCE_DIMENSION_TEXTURE3D) ? 256U : 2048U;
				}
				break;
			case D3D_FEATURE_LEVEL_9_3:
				maxSize = (dim == D3D11_RESOURCE_DIMENSION_TEXTURE3D) ? 256U : 4096U;
				break;
			default:
				maxSize = (dim == D3D11_RESOURCE_DIMENSION_TEXTURE3D) ? 2048U : 8192U;
				break;
			}
		}
	}
}

NamespaceEnd(D3DUtil)