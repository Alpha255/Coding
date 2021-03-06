#include "Colorful/Gfx/GfxFormat.h"
#include <dxgiformat.h>
#include <ThirdParty/Vulkan-Headers/include/vulkan/vulkan.h>
#include <ThirdParty/KTX-Software/lib/gl_format.h>

NAMESPACE_START(Gfx)

static const std::vector<FormatAttribute> s_FormatAttributes
{
	{ EFormat::Unknown,                  DXGI_FORMAT::DXGI_FORMAT_UNKNOWN,                    VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     0u,   "Unknown" },
	{ EFormat::D16_UNorm,                DXGI_FORMAT::DXGI_FORMAT_D16_UNORM,                  VkFormat::VK_FORMAT_D16_UNORM,               GL_DEPTH_COMPONENT16,                   16u,  "D16_UNorm" },
	{ EFormat::D32_Float,                DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,                  VkFormat::VK_FORMAT_D32_SFLOAT,              GL_DEPTH_COMPONENT32F,                  32u,  "D32_Float" },
	{ EFormat::D32_Float_S8X24_Uint,     DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT,       VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,      GL_DEPTH32F_STENCIL8,                   64u,  "D32_Float_S8X24_Uint" },
	{ EFormat::D24_UNorm_S8_UInt,        DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,          VkFormat::VK_FORMAT_D24_UNORM_S8_UINT,       GL_DEPTH24_STENCIL8,                    32u,  "D24_UNorm_S8_UInt" },
	{ EFormat::RGBA32_Typeless,          DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_TYPELESS,      VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,     GL_RGBA32F,                             128u, "RGBA32_Typeless" },
	{ EFormat::RGBA32_Float,             DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,         VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,     GL_RGBA32F,                             128u, "RGBA32_Float" },
	{ EFormat::RGBA32_UInt,              DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT,          VkFormat::VK_FORMAT_R32G32B32A32_UINT,       GL_RGBA32UI,                            128u, "RGBA32_UInt" },
	{ EFormat::RGBA32_SInt,              DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT,          VkFormat::VK_FORMAT_R32G32B32A32_SINT,       GL_RGBA32I,                             128u, "RGBA32_SInt" },
	{ EFormat::RGB32_Typeless,           DXGI_FORMAT::DXGI_FORMAT_R32G32B32_TYPELESS,         VkFormat::VK_FORMAT_R32G32B32_SFLOAT,        GL_RGB32F,                              96u,  "RGB32_Typeless" },
	{ EFormat::RGB32_Float,              DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,            VkFormat::VK_FORMAT_R32G32B32_SFLOAT,        GL_RGB32F,                              96u,  "RGB32_Float" },
	{ EFormat::RGB32_UInt,               DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT,             VkFormat::VK_FORMAT_R32G32B32_UINT,          GL_RGB32UI,                             96u,  "RGB32_UInt" },
	{ EFormat::RGB32_SInt,               DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT,             VkFormat::VK_FORMAT_R32G32B32_SINT,          GL_RGB32I,                              96u,  "RGB32_SInt" },
	{ EFormat::RGBA16_Typeless,          DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_TYPELESS,      VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,     GL_RGBA16F,                             64u,  "RGBA16_Typeless" },
	{ EFormat::RGBA16_Float,             DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT,         VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,     GL_RGBA16F,                             64u,  "RGBA16_Float" },
	{ EFormat::RGBA16_UNorm,             DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM,         VkFormat::VK_FORMAT_R16G16B16A16_UNORM,      GL_RGBA16,                              64u,  "RGBA16_UNorm" },
	{ EFormat::RGBA16_UInt,              DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT,          VkFormat::VK_FORMAT_R16G16B16A16_UINT,       GL_RGBA16UI,                            64u,  "RGBA16_UInt" },
	{ EFormat::RGBA16_SNorm,             DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SNORM,         VkFormat::VK_FORMAT_R16G16B16A16_SNORM,      GL_RGBA16_SNORM,                        64u,  "RGBA16_SNorm" },
	{ EFormat::RGBA16_SInt,              DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SINT,          VkFormat::VK_FORMAT_R16G16B16A16_SINT,       GL_RGBA16I,                             64u,  "RGBA16_SInt" },
	{ EFormat::RG32_Typeless,            DXGI_FORMAT::DXGI_FORMAT_R32G32_TYPELESS,            VkFormat::VK_FORMAT_R32G32_SFLOAT,           GL_RG32F,                               64u,  "RG32_Typeless" },
	{ EFormat::RG32_Float,               DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,               VkFormat::VK_FORMAT_R32G32_SFLOAT,           GL_RG32F,                               64u,  "RG32_Float" },
	{ EFormat::RG32_UInt,                DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT,                VkFormat::VK_FORMAT_R32G32_UINT,             GL_RG32UI,                              64u,  "RG32_UInt" },
	{ EFormat::RG32_SInt,                DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT,                VkFormat::VK_FORMAT_R32G32_SINT,             GL_RG32I,                               64u,  "RG32_SInt" },
	{ EFormat::R32G8X24_Typeless,        DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS,          VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,      GL_DEPTH32F_STENCIL8,                   64u,  "R32G8X24_Typeless" },
	{ EFormat::R32_Float_X8X24_Typeless, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,   VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,      GL_DEPTH32F_STENCIL8,                   64u,  "R32_Float_X8X24_Typeless" },
	{ EFormat::X32_Typeless_G8X24_UInt,  DXGI_FORMAT::DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,    VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     64u,  "X32_Typeless_G8X24_UInt" },
	{ EFormat::RGB10A2_Typeless,         DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_TYPELESS,       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "RGB10A2_Typeless" },
	{ EFormat::RGB10A2_UNorm,            DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM,          VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "RGB10A2_UNorm" },
	{ EFormat::RGB10A2_UInt,             DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UINT,           VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "RGB10A2_UInt" },
	{ EFormat::RG11B10_Float,            DXGI_FORMAT::DXGI_FORMAT_R11G11B10_FLOAT,            VkFormat::VK_FORMAT_B10G11R11_UFLOAT_PACK32, GL_R11F_G11F_B10F,                      32u,  "RG11B10_Float" },
	{ EFormat::RGBA8_Typeless,           DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_TYPELESS,          VkFormat::VK_FORMAT_R8G8B8A8_UNORM,          GL_RGBA8,                               32u,  "RGBA8_Typeless" },
	{ EFormat::RGBA8_UNorm,              DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,             VkFormat::VK_FORMAT_R8G8B8A8_UNORM,          GL_RGBA8,                               32u,  "RGBA8_UNorm" },
	{ EFormat::RGBA8_UNorm_SRGB,         DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,        VkFormat::VK_FORMAT_R8G8B8A8_SRGB,           GL_SRGB8_ALPHA8,                        32u,  "RGBA8_UNorm_SRGB" },
	{ EFormat::RGBA8_UInt,               DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT,              VkFormat::VK_FORMAT_R8G8B8A8_UINT,           GL_RGBA8UI,                             32u,  "RGBA8_UInt" },
	{ EFormat::RGBA8_SNorm,              DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM,             VkFormat::VK_FORMAT_R8G8B8A8_SNORM,          GL_RGBA8_SNORM,                         32u,  "RGBA8_SNorm" },
	{ EFormat::RGBA8_SInt,               DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT,              VkFormat::VK_FORMAT_R8G8B8A8_SINT,           GL_RGBA8I,                              32u,  "RGBA8_SInt" },
	{ EFormat::RG16_Typeless,            DXGI_FORMAT::DXGI_FORMAT_R16G16_TYPELESS,            VkFormat::VK_FORMAT_R16G16_SFLOAT,           GL_RG16F,                               32u,  "RG16_Typeless" },
	{ EFormat::RG16_Float,               DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT,               VkFormat::VK_FORMAT_R16G16_SFLOAT,           GL_RG16F,                               32u,  "RG16_Float" },
	{ EFormat::RG16_UNorm,               DXGI_FORMAT::DXGI_FORMAT_R16G16_UNORM,               VkFormat::VK_FORMAT_R16G16_UNORM,            GL_RG16,                                32u,  "RG16_UNorm" },
	{ EFormat::RG16_UInt,                DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT,                VkFormat::VK_FORMAT_R16G16_UINT,             GL_RG16UI,                              32u,  "RG16_UInt" },
	{ EFormat::RG16_SNorm,               DXGI_FORMAT::DXGI_FORMAT_R16G16_SNORM,               VkFormat::VK_FORMAT_R16G16_SNORM,            GL_RG16_SNORM,                          32u,  "RG16_SNorm" },
	{ EFormat::RG16_SInt,                DXGI_FORMAT::DXGI_FORMAT_R16G16_SINT,                VkFormat::VK_FORMAT_R16G16_SINT,             GL_RG16I,                               32u,  "RG16_SInt" },
	{ EFormat::R32_Typeless,             DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS,               VkFormat::VK_FORMAT_R32_SFLOAT,              GL_R32F,                                32u,  "R32_Typeless" },
	{ EFormat::R32_Float,                DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,                  VkFormat::VK_FORMAT_R32_SFLOAT,              GL_R32F,                                32u,  "R32_Float" },
	{ EFormat::R32_UInt,                 DXGI_FORMAT::DXGI_FORMAT_R32_UINT,                   VkFormat::VK_FORMAT_R32_UINT,                GL_R32UI,                               32u,  "R32_UInt" },
	{ EFormat::R32_SInt,                 DXGI_FORMAT::DXGI_FORMAT_R32_SINT,                   VkFormat::VK_FORMAT_R32_SINT,                GL_R32I,                                32u,  "R32_SInt" },
	{ EFormat::R24G8_Typeless,           DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS,             VkFormat::VK_FORMAT_D24_UNORM_S8_UINT,       GL_DEPTH24_STENCIL8,                    32u,  "R24G8_Typeless" },
	{ EFormat::R24_UNorm_X8_Typeless,    DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS,      VkFormat::VK_FORMAT_D24_UNORM_S8_UINT,       GL_DEPTH24_STENCIL8,                    32u,  "R24_UNorm_X8_Typeless" },
	{ EFormat::X24_Typelesss_G8_UInt,    DXGI_FORMAT::DXGI_FORMAT_X24_TYPELESS_G8_UINT,       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "X24_Typelesss_G8_UInt" },
	{ EFormat::RG8_Typeless,             DXGI_FORMAT::DXGI_FORMAT_R8G8_TYPELESS,              VkFormat::VK_FORMAT_R8G8_UNORM,              GL_RG8,                                 16u,  "RG8_Typeless" },
	{ EFormat::RG8_UNorm,                DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM,                 VkFormat::VK_FORMAT_R8G8_UNORM,              GL_RG8,                                 16u,  "RG8_UNorm" },
	{ EFormat::RG8_UInt,                 DXGI_FORMAT::DXGI_FORMAT_R8G8_UINT,                  VkFormat::VK_FORMAT_R8G8_UINT,               GL_RG8UI,                               16u,  "RG8_UInt" },
	{ EFormat::RG8_SNorm,                DXGI_FORMAT::DXGI_FORMAT_R8G8_SNORM,                 VkFormat::VK_FORMAT_R8G8_SNORM,              GL_RG8_SNORM,                           16u,  "RG8_SNorm" },
	{ EFormat::RG8_SInt,                 DXGI_FORMAT::DXGI_FORMAT_R8G8_SINT,                  VkFormat::VK_FORMAT_R8G8_SINT,               GL_RG8I,                                16u,  "RG8_SInt" },
	{ EFormat::R16_Typeless,             DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS,               VkFormat::VK_FORMAT_R16_SFLOAT,              GL_R16F,                                16u,  "R16_Typeless" },
	{ EFormat::R16_Float,                DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT,                  VkFormat::VK_FORMAT_R16_SFLOAT,              GL_R16F,                                16u,  "R16_Float" },
	{ EFormat::R16_UNorm,                DXGI_FORMAT::DXGI_FORMAT_R16_UNORM,                  VkFormat::VK_FORMAT_R16_UNORM,               GL_R16,                                 16u,  "R16_UNorm" },
	{ EFormat::R16_UInt,                 DXGI_FORMAT::DXGI_FORMAT_R16_UINT,                   VkFormat::VK_FORMAT_R16_UINT,                GL_R16UI,                               16u,  "R16_UInt" },
	{ EFormat::R16_SNorm,                DXGI_FORMAT::DXGI_FORMAT_R16_SNORM,                  VkFormat::VK_FORMAT_R16_SNORM,               GL_R16_SNORM,                           16u,  "R16_SNorm" },
	{ EFormat::R16_SInt,                 DXGI_FORMAT::DXGI_FORMAT_R16_SINT,                   VkFormat::VK_FORMAT_R16_SINT,                GL_R16I,                                16u,  "R16_SInt" },
	{ EFormat::R8_Typeless,              DXGI_FORMAT::DXGI_FORMAT_R8_TYPELESS,                VkFormat::VK_FORMAT_R8_UNORM,                GL_R8,                                  8u,   "R8_Typeless" },
	{ EFormat::R8_UNorm,                 DXGI_FORMAT::DXGI_FORMAT_R8_UNORM,                   VkFormat::VK_FORMAT_R8_UNORM,                GL_R8,                                  8u,   "R8_UNorm" },
	{ EFormat::R8_UInt,                  DXGI_FORMAT::DXGI_FORMAT_R8_UINT,                    VkFormat::VK_FORMAT_R8_UINT,                 GL_R8UI,                                8u,   "R8_UInt" },
	{ EFormat::R8_SNorm,                 DXGI_FORMAT::DXGI_FORMAT_R8_SNORM,                   VkFormat::VK_FORMAT_R8_SNORM,                GL_R8_SNORM,                            8u,   "R8_SNorm" },
	{ EFormat::R8_SInt,                  DXGI_FORMAT::DXGI_FORMAT_R8_SINT,                    VkFormat::VK_FORMAT_R8_SINT,                 GL_R8I,                                 8u,   "R8_SInt" },
	{ EFormat::A8_UNorm,                 DXGI_FORMAT::DXGI_FORMAT_A8_UNORM,                   VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     8u,   "A8_UNorm" },
	{ EFormat::R1_UNorm,                 DXGI_FORMAT::DXGI_FORMAT_R1_UNORM,                   VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     1u,   "R1_UNorm" },
	{ EFormat::RGB9E5_SharedXP,          DXGI_FORMAT::DXGI_FORMAT_R9G9B9E5_SHAREDEXP,         VkFormat::VK_FORMAT_UNDEFINED,               GL_RGB9_E5,                             21u,  "RGB9E5_SharedXP" },
	{ EFormat::RG8_BG8_UNorm,            DXGI_FORMAT::DXGI_FORMAT_R8G8_B8G8_UNORM,            VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "RG8_BG8_UNorm " },
	{ EFormat::GR8_GB8_UNorm,            DXGI_FORMAT::DXGI_FORMAT_G8R8_G8B8_UNORM,            VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "GR8_GB8_UNorm" },
	{ EFormat::BC1_Typeless,             DXGI_FORMAT::DXGI_FORMAT_BC1_TYPELESS,               VkFormat::VK_FORMAT_BC1_RGB_UNORM_BLOCK,     GL_COMPRESSED_RGB_S3TC_DXT1_EXT,        4u,   "BC1_Typeless" },
	{ EFormat::BC1_UNorm,                DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM,                  VkFormat::VK_FORMAT_BC1_RGB_UNORM_BLOCK,     GL_COMPRESSED_RGB_S3TC_DXT1_EXT,        4u,   "BC1_UNorm" },
	{ EFormat::BC1_UNorm_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM_SRGB,             VkFormat::VK_FORMAT_BC1_RGB_SRGB_BLOCK,      GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,       4u,   "BC1_UNorm_SRGB" },
	{ EFormat::BC2_Typeless,             DXGI_FORMAT::DXGI_FORMAT_BC2_TYPELESS,               VkFormat::VK_FORMAT_BC2_UNORM_BLOCK,         GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       8u,   "BC2_Typeless" },
	{ EFormat::BC2_UNorm,                DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM,                  VkFormat::VK_FORMAT_BC2_UNORM_BLOCK,         GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       8u,   "BC2_UNorm" },
	{ EFormat::BC2_UNorm_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM_SRGB,             VkFormat::VK_FORMAT_BC2_SRGB_BLOCK,          GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, 8u,   "BC2_UNorm_SRGB" },
	{ EFormat::BC3_Typeless,             DXGI_FORMAT::DXGI_FORMAT_BC3_TYPELESS,               VkFormat::VK_FORMAT_BC3_UNORM_BLOCK,         GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       8u,   "BC3_Typeless" },
	{ EFormat::BC3_UNorm,                DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM,                  VkFormat::VK_FORMAT_BC3_UNORM_BLOCK,         GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       8u,   "BC3_UNorm" },
	{ EFormat::BC3_UNorm_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM_SRGB,             VkFormat::VK_FORMAT_BC3_SRGB_BLOCK,          GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, 8u,   "BC3_UNorm_SRGB" },
	{ EFormat::BC4_Typeless,             DXGI_FORMAT::DXGI_FORMAT_BC4_TYPELESS,               VkFormat::VK_FORMAT_BC4_UNORM_BLOCK,         GL_COMPRESSED_RED_RGTC1,                4u,   "BC4_Typeless" },
	{ EFormat::BC4_UNorm,                DXGI_FORMAT::DXGI_FORMAT_BC4_UNORM,                  VkFormat::VK_FORMAT_BC4_UNORM_BLOCK,         GL_COMPRESSED_RED_RGTC1,                4u,   "BC4_UNorm" },
	{ EFormat::BC4_SNorm,                DXGI_FORMAT::DXGI_FORMAT_BC4_SNORM,                  VkFormat::VK_FORMAT_BC4_UNORM_BLOCK,         GL_COMPRESSED_RED_RGTC1,                4u,   "BC4_SNorm" },
	{ EFormat::BC5_Typeless,             DXGI_FORMAT::DXGI_FORMAT_BC5_TYPELESS,               VkFormat::VK_FORMAT_BC5_UNORM_BLOCK,         GL_COMPRESSED_RG_RGTC2,                 8u,   "BC5_Typeless" },
	{ EFormat::BC5_UNorm,                DXGI_FORMAT::DXGI_FORMAT_BC5_UNORM,                  VkFormat::VK_FORMAT_BC5_UNORM_BLOCK,         GL_COMPRESSED_RG_RGTC2,                 8u,   "BC5_UNorm" },
	{ EFormat::BC5_SNorm,                DXGI_FORMAT::DXGI_FORMAT_BC5_SNORM,                  VkFormat::VK_FORMAT_BC5_SNORM_BLOCK,         GL_COMPRESSED_SIGNED_RG_RGTC2,          8u,   "BC5_SNorm" },
	{ EFormat::B5G6R5_UNorm,             DXGI_FORMAT::DXGI_FORMAT_B5G6R5_UNORM,               VkFormat::VK_FORMAT_B5G6R5_UNORM_PACK16,     0u,                                     16u,  "B5G6R5_UNorm" },
	{ EFormat::BGR5A1_UNorm,             DXGI_FORMAT::DXGI_FORMAT_B5G5R5A1_UNORM,             VkFormat::VK_FORMAT_B5G5R5A1_UNORM_PACK16,   0u,                                     16u,  "BGR5A1_UNorm" },
	{ EFormat::BGRA8_Typeless,           DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_TYPELESS,          VkFormat::VK_FORMAT_B8G8R8A8_UNORM,          0u,                                     32u,  "BGRA8_Typeless" },
	{ EFormat::BGRA8_UNorm,              DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,             VkFormat::VK_FORMAT_B8G8R8A8_UNORM,          0u,                                     32u,  "BGRA8_UNorm" },
	{ EFormat::BGRA8_UNorm_SRGB,         DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,        VkFormat::VK_FORMAT_B8G8R8A8_SRGB,           0u,                                     32u,  "BGRA8_UNorm_SRGB" },
	{ EFormat::BGRX8_Typeless,           DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_TYPELESS,          VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "BGRX8_Typeless" },
	{ EFormat::BGRX8_UNorm,              DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_UNORM,             VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "BGRX8_UNorm" },
	{ EFormat::BGRX8_UNorm_SRGB,         DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,        VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "BGRX8_UNorm_SRGB" },
	{ EFormat::RGB10_XR_Bias_A2_UNorm,   DXGI_FORMAT::DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "RGB10_XR_Bias_A2_UNorm" },
	{ EFormat::BC6H_Typeless,            DXGI_FORMAT::DXGI_FORMAT_BC6H_TYPELESS,              VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK,       GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,  8u,   "BC6H_Typeless" },
	{ EFormat::BC6H_UF16,                DXGI_FORMAT::DXGI_FORMAT_BC6H_UF16,                  VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK,       GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,  8u,   "BC6H_UF16" },
	{ EFormat::BC6H_SF16,                DXGI_FORMAT::DXGI_FORMAT_BC6H_SF16,                  VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK,       GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,    8u,   "BC6H_SF16" },
	{ EFormat::BC7_Typeless,             DXGI_FORMAT::DXGI_FORMAT_BC7_TYPELESS,               VkFormat::VK_FORMAT_BC7_UNORM_BLOCK,         GL_COMPRESSED_RGBA_BPTC_UNORM,          8u,   "BC7_Typeless" },
	{ EFormat::BC7_UNorm,                DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM,                  VkFormat::VK_FORMAT_BC7_UNORM_BLOCK,         GL_COMPRESSED_RGBA_BPTC_UNORM,          8u,   "BC7_UNorm" },
	{ EFormat::BC7_UNorm_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM_SRGB,             VkFormat::VK_FORMAT_BC7_SRGB_BLOCK,          GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,    8u,   "BC7_UNorm_SRGB" },
	{ EFormat::AYUV,                     DXGI_FORMAT::DXGI_FORMAT_AYUV,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "AYUV" },
	{ EFormat::Y410,                     DXGI_FORMAT::DXGI_FORMAT_Y410,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "Y410" },
	{ EFormat::Y416,                     DXGI_FORMAT::DXGI_FORMAT_Y416,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     64u,  "Y416" },
	{ EFormat::NV12,                     DXGI_FORMAT::DXGI_FORMAT_NV12,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     12u,  "NV12" },
	{ EFormat::P010,                     DXGI_FORMAT::DXGI_FORMAT_P010,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     24u,  "P010" },
	{ EFormat::P016,                     DXGI_FORMAT::DXGI_FORMAT_P016,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     24u,  "P016" },
	{ EFormat::Opaque_420,               DXGI_FORMAT::DXGI_FORMAT_420_OPAQUE,                 VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     12u,  "Opaque_420" },
	{ EFormat::YUY2,                     DXGI_FORMAT::DXGI_FORMAT_YUY2,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     32u,  "YUY2" },
	{ EFormat::Y210,                     DXGI_FORMAT::DXGI_FORMAT_Y210,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     64u,  "Y210" },
	{ EFormat::Y216,                     DXGI_FORMAT::DXGI_FORMAT_Y216,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     64u,  "Y216" },
	{ EFormat::NV11,                     DXGI_FORMAT::DXGI_FORMAT_NV11,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     12u,  "NV11" },
	{ EFormat::AI44,                     DXGI_FORMAT::DXGI_FORMAT_AI44,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     8u,   "AI44" },
	{ EFormat::IA44,                     DXGI_FORMAT::DXGI_FORMAT_IA44,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     8u,   "IA44" },
	{ EFormat::P8,                       DXGI_FORMAT::DXGI_FORMAT_P8,                         VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     8u,   "P8" },
	{ EFormat::AP8,                      DXGI_FORMAT::DXGI_FORMAT_A8P8,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     16u,  "AP8" },
	{ EFormat::BGRA4_UNorm,              DXGI_FORMAT::DXGI_FORMAT_B4G4R4A4_UNORM,             VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     16u,  "BGRA4_UNorm" },
	{ EFormat::P208,                     DXGI_FORMAT::DXGI_FORMAT_P208,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     0u,   "P208" },
	{ EFormat::V208,                     DXGI_FORMAT::DXGI_FORMAT_V208,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     0u,   "V208" },
	{ EFormat::V408,                     DXGI_FORMAT::DXGI_FORMAT_V408,                       VkFormat::VK_FORMAT_UNDEFINED,               0u,                                     0u,   "V408" },
	{ EFormat::Force_UInt,               DXGI_FORMAT::DXGI_FORMAT_FORCE_UINT,                 VkFormat::VK_FORMAT_R8G8B8A8_UINT,           GL_RGBA8UI,                             0u,   "Force_UInt" },
};

FormatAttribute FormatAttribute::attribute(EFormat format)
{
	assert(s_FormatAttributes[static_cast<uint32_t>(format)].Format == format);
	return s_FormatAttributes[static_cast<uint32_t>(format)];
}

FormatAttribute FormatAttribute::attribute(const char8_t* const format)
{
	for (auto& attr : s_FormatAttributes)
	{
		if (_stricmp(attr.Name, format) == 0)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

FormatAttribute FormatAttribute::attribute_Vk(uint32_t format)
{
	for (auto& attr : s_FormatAttributes)
	{
		if (attr.VulkanFormat == format)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

FormatAttribute FormatAttribute::attribute_DXGI(uint32_t format)
{
	for (auto& attr : s_FormatAttributes)
	{
		if (static_cast<uint32_t>(attr.DXGIFromat) == format)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

FormatAttribute FormatAttribute::attribute_GL(uint32_t format)
{
	for (auto& attr : s_FormatAttributes)
	{
		if (attr.GLFormat == format)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

uint32_t FormatAttribute::toDXGIFormat(EFormat format)
{
	assert(s_FormatAttributes[static_cast<uint32_t>(format)].Format == format);
	return s_FormatAttributes[static_cast<uint32_t>(format)].DXGIFromat;
}

uint32_t FormatAttribute::toVulkanFormat(EFormat format)
{
	assert(s_FormatAttributes[static_cast<uint32_t>(format)].Format == format);
	return s_FormatAttributes[static_cast<uint32_t>(format)].VulkanFormat;
}

EFormat FormatAttribute::toSRGBFormat(EFormat format)
{
	switch (format)
	{
	case EFormat::BC1_UNorm:   return EFormat::BC1_UNorm_SRGB;
	case EFormat::BC2_UNorm:   return EFormat::BC2_UNorm_SRGB;
	case EFormat::BGRA8_UNorm: return EFormat::BGRA8_UNorm_SRGB;
	case EFormat::BGRX8_UNorm: return EFormat::BGRX8_UNorm_SRGB;
	case EFormat::BC7_UNorm:   return EFormat::BC7_UNorm_SRGB;
	case EFormat::RGBA8_UNorm: return EFormat::RGBA8_UNorm_SRGB;
	}
	return format;
}

NAMESPACE_END(Gfx)
