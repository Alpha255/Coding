#if 0
#include "AssetDatabase.h"
#include <ThirdParty/gli/gli/gli.hpp>
#include <ThirdParty/stb/stb_image.h>
#include <ThirdParty/DirectXTex/DDSTextureLoader/DDSTextureLoader.h>

namespaceStart(AssetTool)

uint32_t toRFormat(gli::format fmt)
{
	switch (fmt)
	{
		case gli::FORMAT_UNDEFINED: return eUnknownFormat;
		case gli::FORMAT_BGRA4_UNORM_PACK16: return eBGRA4_UNorm;
		case gli::FORMAT_B5G6R5_UNORM_PACK16: return eB5G6R5_UNorm;
		case gli::FORMAT_BGR5A1_UNORM_PACK16: return eBGR5A1_UNorm;
		case gli::FORMAT_R8_UNORM_PACK8: return eR8_UNorm;
		case gli::FORMAT_R8_SNORM_PACK8: return eR8_SNorm;
		case gli::FORMAT_R8_UINT_PACK8: return eR8_UInt;
		case gli::FORMAT_R8_SINT_PACK8: return eR8_SInt;
		case gli::FORMAT_RG8_UNORM_PACK8: return eRG8_UNorm;
		case gli::FORMAT_RG8_SNORM_PACK8: return eRG8_SNorm;
		case gli::FORMAT_RG8_UINT_PACK8: return eRG8_UInt;
		case gli::FORMAT_RG8_SINT_PACK8: return eRG8_SInt;
		case gli::FORMAT_RGBA8_UNORM_PACK8: return eRGBA8_UNorm;
		case gli::FORMAT_RGBA8_SNORM_PACK8: return eRGBA8_SNorm;
		case gli::FORMAT_RGBA8_UINT_PACK8: return eRGBA8_UInt;
		case gli::FORMAT_RGBA8_SINT_PACK8: return eRGBA8_SInt;
		case gli::FORMAT_RGBA8_SRGB_PACK8: return eRGBA8_UNorm_SRGB;
		case gli::FORMAT_BGRA8_SRGB_PACK8: return eBGRA8_UNorm_SRGB;
		case gli::FORMAT_RGB10A2_UNORM_PACK32: return eRGB10A2_UNorm;
		case gli::FORMAT_RGB10A2_UINT_PACK32: return eRGB10A2_UInt;
		case gli::FORMAT_R16_UNORM_PACK16: return eR16_UNorm;
		case gli::FORMAT_R16_SNORM_PACK16: return eR16_SNorm;
		case gli::FORMAT_R16_UINT_PACK16: return eR16_UInt;
		case gli::FORMAT_R16_SINT_PACK16: return eR16_SInt;
		case gli::FORMAT_R16_SFLOAT_PACK16: return eR16_Float;
		case gli::FORMAT_RG16_UNORM_PACK16: return eRG16_UNorm;
		case gli::FORMAT_RG16_SNORM_PACK16: return eRG16_SNorm;
		case gli::FORMAT_RG16_UINT_PACK16: return eRG16_UInt;
		case gli::FORMAT_RG16_SINT_PACK16: return eRG16_SInt;
		case gli::FORMAT_RG16_SFLOAT_PACK16: return eRG16_Float;
		case gli::FORMAT_RGBA16_UNORM_PACK16: return eRGBA16_UNorm;
		case gli::FORMAT_RGBA16_SNORM_PACK16: return eRGBA16_SNorm;
		case gli::FORMAT_RGBA16_UINT_PACK16: return eRGBA16_UInt;
		case gli::FORMAT_RGBA16_SINT_PACK16: return eRGBA16_SInt;
		case gli::FORMAT_RGBA16_SFLOAT_PACK16: return eRGBA16_Float;
		case gli::FORMAT_R32_UINT_PACK32: return eR32_UInt;
		case gli::FORMAT_R32_SINT_PACK32: return eR32_SInt;
		case gli::FORMAT_R32_SFLOAT_PACK32: return eR32_Float;
		case gli::FORMAT_RG32_UINT_PACK32: return eRG32_UInt;
		case gli::FORMAT_RG32_SINT_PACK32: return eRG32_SInt;
		case gli::FORMAT_RG32_SFLOAT_PACK32: return eRG32_Float;
		case gli::FORMAT_RGB32_UINT_PACK32: return eRGB32_UInt;
		case gli::FORMAT_RGB32_SINT_PACK32: return eRGB32_SInt;
		case gli::FORMAT_RGB32_SFLOAT_PACK32: return eRGB32_Float;
		case gli::FORMAT_RGBA32_UINT_PACK32: return eRGBA32_UInt;
		case gli::FORMAT_RGBA32_SINT_PACK32: return eRGBA32_SInt;
		case gli::FORMAT_RGBA32_SFLOAT_PACK32: return eRGBA32_Float;
		case gli::FORMAT_RG11B10_UFLOAT_PACK32: return eRG11B10_Float;
		case gli::FORMAT_RGB9E5_UFLOAT_PACK32: return eRGB9E5_SharedXP;
		case gli::FORMAT_D16_UNORM_PACK16: return eD16_UNorm;
		case gli::FORMAT_D24_UNORM_S8_UINT_PACK32: return eD24_UNorm_S8_UInt;
		case gli::FORMAT_D32_SFLOAT_S8_UINT_PACK64: return eD32_Float_S8X24_Uint;
		case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8: return eBC1_UNorm;
		case gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8: return eBC1_UNorm_SRGB;
		case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16: return eBC3_UNorm;
		case gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16: return eBC3_UNorm_SRGB;
		case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16: return eBC5_UNorm;
	}

	assert(0);
	return 0u;
}

AssetTool::TextureBinary AssetDatabase::tryToGetTextureBinary(Configurations::eRenderEngine engine, const std::string& texName)
{
	auto texture = tryToGetAsset(texName);
	assert(texture);

	TextureBinary binary;

	if (texture->type() == Asset::eDDSTexture || texture->type() == Asset::eVulkanTexture)
	{
		gli::texture tex = gli::load(texture->fullPath());
		binary.Size = tex.size();
		binary.Width = tex.extent().x;
		binary.Height = tex.extent().y;
		binary.Depth = tex.extent().z;
		binary.MipLevels = (uint32_t)tex.levels();
		binary.ArrayLayers = (uint32_t)tex.layers();
		binary.Format = engine == Configurations::eVulkan ? (uint32_t)(static_cast<VkFormat>(tex.format())) : toRFormat(tex.format());
		binary.Images.resize(binary.ArrayLayers);
		binary.SharedBinary.reset(new byte[binary.Size]());
		verify(memcpy_s(binary.SharedBinary.get(), binary.Size, tex.data(), binary.Size) == 0);

		switch (tex.target())
		{
		case gli::TARGET_1D:
		{
			gli::texture1d gliImage1D(tex);
			assert(binary.ArrayLayers == 1u);
			binary.Images[0].resize(gliImage1D.levels());
			for (uint32_t i = 0u; i < gliImage1D.levels(); ++i)
			{
				binary.Images[0][i].Width = gliImage1D[i].extent().x;
				binary.Images[0][i].Height = gliImage1D[i].extent().y;
				binary.Images[0][i].Depth = gliImage1D[i].extent().z;
				binary.Images[0][i].Size = gliImage1D[i].size();
			}
			binary.Type = eTexture1D;
		}
		break;
		case gli::TARGET_1D_ARRAY:
		{
			gli::texture1d_array gliImage1D_Array(tex);
			for (uint32_t i = 0u; i < binary.ArrayLayers; ++i)
			{
				gli::texture1d gliImage1D = gliImage1D_Array[i];
				binary.Images[i].resize(gliImage1D.levels());
				for (uint32_t j = 0u; j < gliImage1D.levels(); ++j)
				{
					binary.Images[i][j].Width = gliImage1D[j].extent().x;
					binary.Images[i][j].Height = gliImage1D[j].extent().y;
					binary.Images[i][j].Depth = gliImage1D[j].extent().z;
					binary.Images[i][j].Size = gliImage1D[j].size();
				}
			}
			binary.Type = eTexture1DArray;
		}
		break;
		case gli::TARGET_2D:
		{
			gli::texture2d gliImage2D(tex);
			assert(binary.ArrayLayers == 1u);
			binary.Images[0].resize(gliImage2D.levels());
			for (uint32_t i = 0u; i < gliImage2D.levels(); ++i)
			{
				binary.Images[0][i].Width = gliImage2D[i].extent().x;
				binary.Images[0][i].Height = gliImage2D[i].extent().y;
				binary.Images[0][i].Depth = gliImage2D[i].extent().z;
				binary.Images[0][i].Size = gliImage2D[i].size();
			}
			binary.Type = eTexture2D;
		}
		break;
		case gli::TARGET_2D_ARRAY:
		{
			gli::texture2d_array gliImage2D_Array(tex);
			for (uint32_t i = 0u; i < binary.ArrayLayers; ++i)
			{
				gli::texture2d gliImage2D = gliImage2D_Array[i];
				binary.Images[i].resize(gliImage2D.levels());
				for (uint32_t j = 0u; j < gliImage2D.levels(); ++j)
				{
					binary.Images[i][j].Width = gliImage2D[j].extent().x;
					binary.Images[i][j].Height = gliImage2D[j].extent().y;
					binary.Images[i][j].Depth = gliImage2D[j].extent().z;
					binary.Images[i][j].Size = gliImage2D[j].size();
				}
			}
			binary.Type = eTexture2DArray;
		}
		break;
		case gli::TARGET_3D:
		{
			gli::texture3d gliImage3D(tex);
			assert(binary.ArrayLayers == 1u);
			binary.Images[0].resize(gliImage3D.levels());
			for (uint32_t i = 0u; i < gliImage3D.levels(); ++i)
			{
				binary.Images[0][i].Width = gliImage3D[i].extent().x;
				binary.Images[0][i].Height = gliImage3D[i].extent().y;
				binary.Images[0][i].Depth = gliImage3D[i].extent().z;
				binary.Images[0][i].Size = gliImage3D[i].size();
			}
			binary.Type = eTexture3D;
		}
		break;
		case gli::TARGET_CUBE:
		{
			gli::texture_cube gliImageCube(tex);
			assert(binary.ArrayLayers == 1u);
			binary.Images.resize(gliImageCube.faces());
			for (uint32_t i = 0u; i < gliImageCube.faces(); ++i)
			{
				gli::texture2d gliImageFace = gliImageCube[i];
				binary.Images[i].resize(gliImageFace.levels());
				for (uint32_t j = 0u; j < gliImageFace.levels(); ++j)
				{
					binary.Images[i][j].Width = gliImageFace[j].extent().x;
					binary.Images[i][j].Height = gliImageFace[j].extent().y;
					binary.Images[i][j].Size = gliImageFace[j].size();
				}
			}
			binary.Type = eTextureCube;
		}
		break;
		default:
			assert(0);
			break;
		}
	}
	else if (texture->type() == Asset::eWICTexture)
	{
		int32_t width = 0u;
		int32_t height = 0u;
		int32_t channels = 0u;
		auto pixels = stbi_load(texture->fullPath().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		binary.Width = width;
		binary.Height = height;
		binary.Size = width * height * STBI_rgb_alpha;
		binary.Depth = 1u;
		binary.MipLevels = 1u;
		binary.ArrayLayers = 1u;
		binary.Type = eTexture2D;
		binary.Format = VK_FORMAT_R8G8B8A8_SRGB;
		binary.Images.resize(1u);
		binary.Images[0].resize(1u);
		binary.Images[0][0].Width = width;
		binary.Images[0][0].Height = height;
		binary.Images[0][0].Depth = 1u;
		binary.Images[0][0].Size = binary.Size;
		binary.SharedBinary.reset(new byte[binary.Size]());
		verify(memcpy_s(binary.SharedBinary.get(), binary.Size, pixels, binary.Size) == 0);
		stbi_image_free(pixels);
	}
	else
	{
		assert(0);
	}

	return binary;
}

ID3D11ShaderResourceView* AssetDatabase::tryToLoadD3DTextureFromFile(ID3D11Device* device, const AssetPtr& asset)
{
	assert(device && asset);
	assert(asset->type() == Asset::eWICTexture || asset->type() == Asset::eDDSTexture);

	std::string texturePath = asset->fullPath();
	std::wstring wTexturePath(texturePath.cbegin(), texturePath.cend());

	ID3D11ShaderResourceView* result = nullptr;
	verify(DirectX::CreateDDSTextureFromFile(
		device,
		wTexturePath.c_str(),
		nullptr,
		&result,
		0u,
		nullptr) == S_OK);
	return result;
}

namespaceEnd(AssetTool)
#endif