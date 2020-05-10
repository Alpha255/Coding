#include "GfxResource.h"

size_t GfxVertexAttributes::formatStride(eRFormat format)
{
	size_t stride = 0ull;

	switch (format)
	{
	case eD16_UNorm:                
		stride = 16ull; 
		break;
	case eD32_Float:                
		stride = 32ull;
		break;
	case eD32_Float_S8X24_Uint:     
		stride = 32ull + 32ull;
		break;
	case eD24_UNorm_S8_UInt:        
		stride = 24ull + 8ull;
		break;
	case eRGBA32_Typeless:
	case eRGBA32_Float:
	case eRGBA32_UInt:
	case eRGBA32_SInt:
		stride = 32ull * 4ull;
		break;
	case eRGB32_Typeless:
	case eRGB32_Float:   
	case eRGB32_UInt:    
	case eRGB32_SInt:
		stride = 32ull * 3ull;
		break;
	case eRGBA16_Typeless:
	case eRGBA16_Float:   
	case eRGBA16_UNorm:   
	case eRGBA16_UInt:    
	case eRGBA16_SNorm:   
	case eRGBA16_SInt:
		stride = 16ull * 4ull;
		break;
	case eRG32_Typeless:
	case eRG32_Float:   
	case eRG32_UInt:    
	case eRG32_SInt:
		stride = 32ull * 2ull;
		break;
	case eR32G8X24_Typeless:        
		stride = 64ull;
		break;
	case eR32_Float_X8X24_Typeless: 
		stride = 32ull + 32ull;
		break;
	case eX32_Typeless_G8X24_UInt:  
		stride = 32ull + 32ull;
		break;
	case eRGB10A2_Typeless:
	case eRGB10A2_UNorm:   
	case eRGB10A2_UInt:
		stride = 10ull * 3ull + 2ull;
		break;
	case eRG11B10_Float:            
		stride = 11ull * 2ull + 10ull;
		break;
	case eRGBA8_Typeless:  
	case eRGBA8_UNorm:     
	case eRGBA8_UNorm_SRGB:
	case eRGBA8_UInt:      
	case eRGBA8_SNorm:     
	case eRGBA8_SInt:
		stride = 8ull * 4ull;
		break;
	case eRG16_Typeless:
	case eRG16_Float:   
	case eRG16_UNorm:   
	case eRG16_UInt:    
	case eRG16_SNorm:   
	case eRG16_SInt:
		stride = 16ull * 2ull;
		break;
	case eR32_Typeless:
	case eR32_Float:   
	case eR32_UInt:    
	case eR32_SInt:
		stride = 32ull;
		break;
	case eR24G8_Typeless:           
		stride = 24ull + 8ull;
		break;
	case eR24_UNorm_X8_Typeless:    
		stride = 24ull + 8ull;
		break;
	case eX24_Typelesss_G8_UInt:    
		stride = 24ull + 8ull;
		break;
	case eRG8_Typeless:
	case eRG8_UNorm:   
	case eRG8_UInt:    
	case eRG8_SNorm:   
	case eRG8_SInt:
		stride = 8ull * 2ull;
		break;
	case eR16_Typeless:
	case eR16_Float:   
	case eR16_UNorm:   
	case eR16_UInt:    
	case eR16_SNorm:   
	case eR16_SInt:
		stride = 16ull;
		break;
	case eR8_Typeless:
	case eR8_UNorm:   
	case eR8_UInt:    
	case eR8_SNorm:   
	case eR8_SInt:
		stride = 8ull;
		break;
	case eA8_UNorm:                 
		stride = 8ull;
		break;
	case eRGB9E5_SharedXP:          
		stride = 9ull * 3ull + 5ull;
		break;
	case eRG8_BG8_UNorm:
	case eGR8_GB8_UNorm:
		stride = 8ull * 4ull;     
		break;
	case eB5G6R5_UNorm:             
		stride = 5ull + 6ull + 5ull;
		break;
	case eBGR5A1_UNorm:             
		stride = 5ull * 3ull + 1ull;
		break;
	case eBGRA8_UNorm:
	case eBGRX8_UNorm:
		stride = 8ull * 4ull;
		break;
	case eRGB10_XR_Bias_A2_UNorm:   
		stride = 10ull * 3ull + 2ull;
		break;
	case eBGRA8_Typeless:  
	case eBGRA8_UNorm_SRGB:
	case eBGRX8_Typeless:  
	case eBGRX8_UNorm_SRGB:
		stride = 8ull * 4ull;
		break;
	case eForce_UInt:
		stride = 32ull;
		break;
	case eR1_UNorm:
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
	}

	assert(stride % 8ull == 0ull);
	return stride / 8ull;
}
