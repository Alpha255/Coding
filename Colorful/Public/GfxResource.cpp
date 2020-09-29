#include "Colorful/Public/GfxResource.h"

NAMESPACE_START(Gfx)

size_t VertexAttributes::stride(EFormat format, size_t alignment)
{
	size_t stride = 0ull;

	switch (format)
	{
	case EFormat::D16_UNorm:               stride = 16ull; break;
	case EFormat::D32_Float:               stride = 32ull; break;
	case EFormat::D32_Float_S8X24_Uint:    stride = 32ull + 8ull + 24ull; break;
	case EFormat::D24_UNorm_S8_UInt:       stride = 24ull + 8ull; break;
	case EFormat::RGBA32_Typeless:
	case EFormat::RGBA32_Float:
	case EFormat::RGBA32_UInt:
	case EFormat::RGBA32_SInt:             stride = 32ull * 4ull; break;
	case EFormat::RGB32_Typeless:
	case EFormat::RGB32_Float:   
	case EFormat::RGB32_UInt:    
	case EFormat::RGB32_SInt:              stride = 32ull * 3ull; break;
	case EFormat::RGBA16_Typeless:
	case EFormat::RGBA16_Float:   
	case EFormat::RGBA16_UNorm:   
	case EFormat::RGBA16_UInt:    
	case EFormat::RGBA16_SNorm:   
	case EFormat::RGBA16_SInt:             stride = 16ull * 4ull; break;
	case EFormat::RG32_Typeless:
	case EFormat::RG32_Float:   
	case EFormat::RG32_UInt:    
	case EFormat::RG32_SInt:               stride = 32ull * 2ull; break;
	case EFormat::R32G8X24_Typeless:       stride = 32ull + 8ull + 24ull; break;
	case EFormat::R32_Float_X8X24_Typeless:
	case EFormat::X32_Typeless_G8X24_UInt: stride = 32ull + 8ull + 24ull; break;
	case EFormat::RGB10A2_Typeless:
	case EFormat::RGB10A2_UNorm:   
	case EFormat::RGB10A2_UInt:            stride = 10ull * 3ull + 2ull; break;
	case EFormat::RG11B10_Float:           stride = 11ull * 2ull + 10ull; break;
	case EFormat::RGBA8_Typeless:  
	case EFormat::RGBA8_UNorm:     
	case EFormat::RGBA8_UNorm_SRGB:
	case EFormat::RGBA8_UInt:      
	case EFormat::RGBA8_SNorm:     
	case EFormat::RGBA8_SInt:              stride = 8ull * 4ull; break;
	case EFormat::RG16_Typeless:
	case EFormat::RG16_Float:   
	case EFormat::RG16_UNorm:   
	case EFormat::RG16_UInt:    
	case EFormat::RG16_SNorm:   
	case EFormat::RG16_SInt:               stride = 16ull * 2ull; break;
	case EFormat::R32_Typeless:
	case EFormat::R32_Float:   
	case EFormat::R32_UInt:    
	case EFormat::R32_SInt:                stride = 32ull; break;
	case EFormat::R24G8_Typeless:
	case EFormat::R24_UNorm_X8_Typeless:
	case EFormat::X24_Typelesss_G8_UInt:   stride = 24ull + 8ull; break;
	case EFormat::RG8_Typeless:
	case EFormat::RG8_UNorm:   
	case EFormat::RG8_UInt:    
	case EFormat::RG8_SNorm:   
	case EFormat::RG8_SInt:                stride = 8ull * 2ull; break;
	case EFormat::R16_Typeless:
	case EFormat::R16_Float:   
	case EFormat::R16_UNorm:   
	case EFormat::R16_UInt:    
	case EFormat::R16_SNorm:   
	case EFormat::R16_SInt:                stride = 16ull; break;
	case EFormat::R8_Typeless:
	case EFormat::R8_UNorm:   
	case EFormat::R8_UInt:    
	case EFormat::R8_SNorm:   
	case EFormat::R8_SInt:
	case EFormat::A8_UNorm:                stride = 8ull; break;
	case EFormat::RGB9E5_SharedXP:         stride = 9ull * 3ull + 5ull; break;
	case EFormat::RG8_BG8_UNorm:
	case EFormat::GR8_GB8_UNorm:           stride = 8ull * 4ull; break;
	case EFormat::B5G6R5_UNorm:            stride = 5ull + 6ull + 5ull; break;
	case EFormat::BGR5A1_UNorm:            stride = 5ull * 3ull + 1ull; break;
	case EFormat::BGRA8_UNorm:
	case EFormat::BGRX8_UNorm:             stride = 8ull * 4ull; break;
	case EFormat::RGB10_XR_Bias_A2_UNorm:  stride = 10ull * 3ull + 2ull; break;
	case EFormat::BGRA8_Typeless:  
	case EFormat::BGRA8_UNorm_SRGB:
	case EFormat::BGRX8_Typeless:  
	case EFormat::BGRX8_UNorm_SRGB:        stride = 8ull * 4ull; break;
	case EFormat::Force_UInt:              stride = 32ull; break;
	}

	assert(stride && stride % 8ull == 0ull);
	return alignment == 1ull ? stride / 8ull : stride / 8ull;
}

NAMESPACE_END(Gfx)
