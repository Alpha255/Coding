#include "AssetTool.h"
#include "Gear/Public/Extension/Log.h"
#include <ThirdParty/SPIRV-Cross/spirv_hlsl.hpp>
#include <ThirdParty/gli/gli/gli.hpp>

namespaceStart(assetTool)

std::string getShaderCode(const std::shared_ptr<byte> &shaderBinary, size_t shaderBinarySize, appConfig::eRenderEngine engine)
{
	std::vector<uint32_t> spirvBinary(shaderBinarySize / sizeof(uint32_t));
	verify(memcpy_s((void *)spirvBinary.data(), shaderBinarySize, (void *)shaderBinary.get(), shaderBinarySize) == 0);

	std::string result;

	if (engine == appConfig::eD3D11)
	{
		spirv_cross::CompilerHLSL hlsl(std::move(spirvBinary));
		spirv_cross::CompilerHLSL::Options options;
		options.shader_model = 50u;
		hlsl.set_hlsl_options(options);

		result = hlsl.compile();
	}
	else
	{
		assert(0);
	}

	return result;
}

/// Macros, Includes
rAsset::rShaderBinary compileD3DShader(eRShaderUsage usage, const std::string &shaderCode)
{
	const char8_t *const shaderModels[eRShaderUsage_MaxEnum] =
	{
		"vs_5_0",
		"hs_5_0",
		"ds_5_0",
		"gs_5_0",
		"ps_5_0",
		"cs_5_0"
	};

	ID3DBlob *pResult = nullptr;
	ID3DBlob *pErrMsg = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	auto compileResult = D3DCompile(
		(const void *)shaderCode.c_str(),
		shaderCode.length(),
		nullptr,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		shaderModels[usage],
		flags,
		0U,
		&pResult,
		&pErrMsg);

	if (compileResult != 0)
	{
		logger::instance().log(logger::eError, "Failed to compile shader, error info: %s", (char8_t *)pErrMsg->GetBufferPointer());
		assert(0);
	}

	size_t bufferSize = pResult->GetBufferSize();
	rAsset::rShaderBinary result;
	result.Size = bufferSize;
	result.Binary.reset(new byte[bufferSize]);
	verify(memcpy_s((void *)result.Binary.get(), bufferSize, (void *)pResult->GetBufferPointer(), bufferSize) == 0);
	
	return result;
}

rAsset::rShaderBinary getShaderBinary(appConfig::eRenderEngine engine, eRShaderUsage usage,	assetFilePtr &shaderAssetPtr)
{
	assert(shaderAssetPtr);

	auto pCode = shaderAssetPtr->getData();
	assert(pCode);

	std::string inputFile(shaderAssetPtr->getFullPath());
	std::string outputPath = file::getFileDirectory(getApplicationPath()) + "\\Intermediate\\AssetTool";
	if (!file::isDirectoryExists(outputPath))
	{
		file::createDirectory(outputPath);
	}
	std::string outputFile = outputPath + "\\";
	outputFile += shaderAssetPtr->getName() + ".spirv";

	std::string commandline = getEnvironmentVariable("VK_SDK_PATH") + "\\Bin\\glslangValidator ";

	switch (usage)
	{
	case eVertexShader:
		commandline += "-S vert ";
		commandline += "-DVertexShader ";
		break;
	case eFragmentShader:
		commandline += "-S frag ";
		commandline += "-DFragmentShader ";
		break;
	default:
		assert(0);
		break;
	}

	commandline += "-e main ";

#if defined(_DEBUG)
	commandline += "-g ";
#endif
	commandline += "-V ";

	commandline += "-o ";
	commandline += outputFile;
	commandline += " ";
	commandline += inputFile;

	assert(gear::executeProcess(commandline, true));

	gear::file spirvFile(outputFile);
	size_t spirvSize = spirvFile.getSize();
	assert((spirvSize % sizeof(uint32_t)) == 0u);

	rAsset::rShaderBinary result;

	if (engine == appConfig::eVulkan)
	{
		result.Binary.reset(new byte[spirvSize]());
		result.Size = spirvSize;
		verify(memcpy_s((void *)result.Binary.get(), spirvSize, (void *)spirvFile.getData(file::eBinary).get(), spirvSize) == 0);
	}
	else if (engine == appConfig::eD3D11)
	{
		std::string shaderCode = getShaderCode(spirvFile.getData(file::eBinary), spirvSize, engine);
		assert(shaderCode.length() > 0u);
		result = compileD3DShader(usage, shaderCode);
	}
	else
	{
		assert(0);
	}

	return result;
}

rAsset::rTextureBinary getTextureBinary(appConfig::eRenderEngine engine, const assetFilePtr &textureAssetPtr)
{
	assert(textureAssetPtr && engine == appConfig::eVulkan);

	rAsset::rTextureBinary result;
	gli::texture gliImage = gli::load(textureAssetPtr->getFullPath());
	result.Size = gliImage.size();
	result.Width = gliImage.extent().x;
	result.Height = gliImage.extent().y;
	result.Depth = gliImage.extent().z;
	result.MipLevels = (uint32_t)gliImage.levels();
	result.ArrayLayers = (uint32_t)gliImage.layers();
	result.Images.resize(result.ArrayLayers);
	result.Binary.reset(new byte[result.Size]());
	verify(memcpy_s(result.Binary.get(), result.Size, gliImage.data(), result.Size) == 0);

	switch (gliImage.target())
	{
	case gli::TARGET_1D:
	{
		gli::texture1d gliImage1D(gliImage);
		assert(result.ArrayLayers == 1u);
		result.Images[0].resize(gliImage1D.levels());
		for (uint32_t i = 0u; i < gliImage1D.levels(); ++i)
		{
			result.Images[0][i].Width = gliImage1D[i].extent().x;
			result.Images[0][i].Size = gliImage1D[i].size();
		}
		result.Type = eTexture1D;
	}
		break;
	case gli::TARGET_1D_ARRAY:
	{
		gli::texture1d_array gliImage1D_Array(gliImage);
		for (uint32_t i = 0u; i < result.ArrayLayers; ++i)
		{
			gli::texture1d gliImage1D = gliImage1D_Array[i];
			result.Images[i].resize(gliImage1D.levels());
			for (uint32_t j = 0u; j < gliImage1D.levels(); ++j)
			{
				result.Images[i][j].Width = gliImage1D[j].extent().x;
				result.Images[i][j].Size = gliImage1D[j].size();
			}
		}
		result.Type = eTexture1DArray;
	}
		break;
	case gli::TARGET_2D:
	{
		gli::texture2d gliImage2D(gliImage);
		assert(result.ArrayLayers == 1u);
		result.Images[0].resize(gliImage2D.levels());
		for (uint32_t i = 0u; i < gliImage2D.levels(); ++i)
		{
			result.Images[0][i].Width = gliImage2D[i].extent().x;
			result.Images[0][i].Height = gliImage2D[i].extent().y;
			result.Images[0][i].Size = gliImage2D[i].size();
		}
		result.Type = eTexture2D;
	}
		break;
	case gli::TARGET_2D_ARRAY:
	{
		gli::texture2d_array gliImage2D_Array(gliImage);
		for (uint32_t i = 0u; i < result.ArrayLayers; ++i)
		{
			gli::texture2d gliImage2D = gliImage2D_Array[i];
			result.Images[i].resize(gliImage2D.levels());
			for (uint32_t j = 0u; j < gliImage2D.levels(); ++j)
			{
				result.Images[i][j].Width = gliImage2D[j].extent().x;
				result.Images[i][j].Height = gliImage2D[j].extent().y;
				result.Images[i][j].Size = gliImage2D[j].size();
			}
		}
		result.Type = eTexture2DArray;
	}
		break;
	case gli::TARGET_3D:
	{
		gli::texture3d gliImage3D(gliImage);
		assert(result.ArrayLayers == 1u);
		result.Images[0].resize(gliImage3D.levels());
		for (uint32_t i = 0u; i < gliImage3D.levels(); ++i)
		{
			result.Images[0][i].Width = gliImage3D[i].extent().x;
			result.Images[0][i].Height = gliImage3D[i].extent().y;
			result.Images[0][i].Depth = gliImage3D[i].extent().z;
			result.Images[0][i].Size = gliImage3D[i].size();
		}
		result.Type = eTexture3D;
	}
		break;
	case gli::TARGET_CUBE:
	{
		gli::texture_cube gliImageCube(gliImage);
		assert(result.ArrayLayers == 1u);
		result.Images.resize(gliImageCube.faces());
		for (uint32_t i = 0u; i < gliImageCube.faces(); ++i)
		{
			gli::texture2d gliImageFace = gliImageCube[i];
			result.Images[i].resize(gliImageFace.levels());
			for (uint32_t j = 0u; j < gliImageFace.levels(); ++j)
			{
				result.Images[i][j].Width = gliImageFace[j].extent().x;
				result.Images[i][j].Height = gliImageFace[j].extent().y;
				result.Images[i][j].Size = gliImageFace[j].size();
			}
		}
		result.Type = eTextureCube;
	}
		break;
	default:
		assert(0);
		break;
	}

	return result;
}

namespaceEnd(assetTool)