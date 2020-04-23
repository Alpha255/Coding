#include "RGpuResource.h"

uint32_t rVertexAttributes::getUsageStride(eRVertexUsage usage)
{
	assert(usage < eRVertexUsge_MaxEnum);
	switch (usage)
	{
	case ePosition:
	case eColor:
		return 4u;
	case eNormal:
	case eBinNormal:
	case eTangent:
		return 3u;
	case eTexcoord:
		return 2u;
	}
	return (uint32_t)-1;
}

size_t rVertexAttributes::getFormatStride(eRFormat format)
{
	switch (format)
	{
	case eD16_UNorm:                return 16ull;
	case eD32_Float:                return 32ull;
	case eD32_Float_S8X24_Uint:     return 32ull + 32ull;
	case eD24_UNorm_S8_UInt:        return 24ull + 8ull;
	case eRGBA32_Typeless:
	case eRGBA32_Float:
	case eRGBA32_UInt:
	case eRGBA32_SInt:
		return 32ull * 4ull;
	case eRGB32_Typeless:
	case eRGB32_Float:   
	case eRGB32_UInt:    
	case eRGB32_SInt:
		return 32ull * 3ull;
	case eRGBA16_Typeless:
	case eRGBA16_Float:   
	case eRGBA16_UNorm:   
	case eRGBA16_UInt:    
	case eRGBA16_SNorm:   
	case eRGBA16_SInt:
		return 16ull * 4ull;
	case eRG32_Typeless:
	case eRG32_Float:   
	case eRG32_UInt:    
	case eRG32_SInt:
		return 32ull * 2ull;
	case eR32G8X24_Typeless:        return 64ull;
	case eR32_Float_X8X24_Typeless: return 32ull + 32ull;
	case eX32_Typeless_G8X24_UInt:  return 32ull + 32ull;
	case eRGB10A2_Typeless:
	case eRGB10A2_UNorm:   
	case eRGB10A2_UInt:
		return 10ull * 3ull + 2ull;
	case eRG11B10_Float:            return 11ull * 2ull + 10ull;
	case eRGBA8_Typeless:  
	case eRGBA8_UNorm:     
	case eRGBA8_UNorm_SRGB:
	case eRGBA8_UInt:      
	case eRGBA8_SNorm:     
	case eRGBA8_SInt:
		return 8ull * 4ull;
	case eRG16_Typeless:
	case eRG16_Float:   
	case eRG16_UNorm:   
	case eRG16_UInt:    
	case eRG16_SNorm:   
	case eRG16_SInt:
		return 16ull * 2ull;
	case eR32_Typeless:
	case eR32_Float:   
	case eR32_UInt:    
	case eR32_SInt:
		return 32ull;
	case eR24G8_Typeless:           return 24ull + 8ull;
	case eR24_UNorm_X8_Typeless:    return 24ull + 8ull;
	case eX24_Typelesss_G8_UInt:    return 24ull + 8ull;
	case eRG8_Typeless:
	case eRG8_UNorm:   
	case eRG8_UInt:    
	case eRG8_SNorm:   
	case eRG8_SInt:
		return 8ull * 2ull;
	case eR16_Typeless:
	case eR16_Float:   
	case eR16_UNorm:   
	case eR16_UInt:    
	case eR16_SNorm:   
	case eR16_SInt:
		return 16ull;
	case eR8_Typeless:
	case eR8_UNorm:   
	case eR8_UInt:    
	case eR8_SNorm:   
	case eR8_SInt:
		return 8ull;
	case eA8_UNorm:                 return 8ull;
	case eR1_UNorm:                 return 1ull;
	case eRGB9E5_SharedXP:          return 9ull * 3ull + 5ull;
	case eRG8_BG8_UNorm:
	case eGR8_GB8_UNorm:
		return 8ull * 4ull;     
	case eB5G6R5_UNorm:             return 5ull + 6ull + 5ull;
	case eBGR5A1_UNorm:             return 5ull * 3ull + 1ull;
	case eBGRA8_UNorm:
	case eBGRX8_UNorm:
		return 8ull * 4ull;
	case eRGB10_XR_Bias_A2_UNorm:   return 10ull * 3ull + 2ull;
	case eBGRA8_Typeless:  
	case eBGRA8_UNorm_SRGB:
	case eBGRX8_Typeless:  
	case eBGRX8_UNorm_SRGB:
		return 8ull * 4ull;
	case eBC1_Typeless:
	case eBC1_UNorm:
	case eBC1_UNorm_SRGB:
	case eBC2_Typeless:
	case eBC2_UNorm:
	case eBC2_UNorm_SRGB:
	case eBC3_Typeless:
	case eBC3_UNorm:
	case eBC3_UNorm_SRGB:
	case eBC4_Typeless:
	case eBC4_UNorm:
	case eBC4_SNorm:
	case eBC5_Typeless:
	case eBC5_UNorm:
	case eBC5_SNorm:
	case eBC6H_Typeless: 
	case eBC6H_UF16:     
	case eBC6H_SF16:     
	case eBC7_Typeless:  
	case eBC7_UNorm:     
	case eBC7_UNorm_SRGB:
	case eAYUV:          
	case eY410:          
	case eY416:          
	case eNV12:          
	case eP010:          
	case eP016:          
	case e420_Opaque:    
	case eYUY2:          
	case eY210:          
	case eY216:          
	case eNV11:          
	case eAI44:          
	case eIA44:          
	case eP8:            
	case eAP8:           
	case eBGRA4_UNorm:   
	case eP208:          
	case eV208:          
	case eV408:
		assert(0);
		break;
	case eForce_UInt:               return 32ull;
	}

	return (size_t)-1;
}
