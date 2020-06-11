#include "AssetDatabase.h"
#include <ThirdParty/gli/gli/gli.hpp>
#include <ThirdParty/stb/stb_image.h>

namespaceStart(AssetTool)

AssetTool::TextureBinary AssetDatabase::tryToGetTextureBinary(Configurations::eRenderEngine engine, const std::string& texName)
{
	assert(engine == Configurations::eVulkan);

	auto texture = tryToGetAsset(texName);

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
		binary.Format = (uint32_t)(static_cast<VkFormat>(tex.format()));
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

namespaceEnd(AssetTool)