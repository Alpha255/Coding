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

void buildShaderReflections(appConfig::eRenderEngine engine, rAsset::rShaderBinary &binary)
{
	if (engine == appConfig::eVulkan)
	{
		std::vector<uint32_t> spirv(binary.Size / sizeof(uint32_t));
		verify(memcpy_s((void *)spirv.data(), binary.Size, (void *)binary.Binary.get(), binary.Size) == 0);
		spirv_cross::Compiler compiler(std::move(spirv));
		auto activeVars = compiler.get_active_interface_variables();
		spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources(activeVars);
		compiler.set_enabled_interface_variables(std::move(activeVars));

		auto pushReflection = [&](
			const spirv_cross::SmallVector<spirv_cross::Resource> &resources,
			VkDescriptorType type,
			bool8_t useConstantRange = false)
		{
			for each (auto &res in resources)
			{
				rShader::rReflectionInfo reflection
				{
					///compiler.get_decoration(res.id, spv::DecorationDescriptorSet), uniform (set = 0, binding = 1)
					(uint32_t)type,
					useConstantRange ? UINT32_MAX : compiler.get_decoration(res.id, spv::DecorationBinding)
				};
				binary.Reflections.emplace_back(std::move(reflection));
			}
		};

		pushReflection(shaderResources.uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		pushReflection(shaderResources.push_constant_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, true);
		pushReflection(shaderResources.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		pushReflection(shaderResources.storage_images, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		pushReflection(shaderResources.sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		pushReflection(shaderResources.separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		pushReflection(shaderResources.separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER);
	}
	else if (engine == appConfig::eD3D11)
	{

	}
	else
	{
		assert(0);
	}
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

	buildShaderReflections(engine, result);

	return result;
}

rAsset::rTextureBinary getTextureBinary(appConfig::eRenderEngine engine, const assetFilePtr &textureAssetPtr)
{
	assert(textureAssetPtr && engine == appConfig::eVulkan);

	rAsset::rTextureBinary binary;
	gli::texture tex = gli::load(textureAssetPtr->getFullPath());
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

	return binary;
}

namespaceEnd(assetTool)