#include "AssetTool.h"
#include "Gear/Gear.h"
#include <ThirdParty/SPIRV-Cross/spirv_hlsl.hpp>
#include <ThirdParty/gli/gli/gli.hpp>

namespaceStart(assetTool)

rShaderBinary rAssetBucket::getShaderBinary(eRShaderUsage usage, const std::string &shaderName)
{
	assert(usage < eRShaderUsage_MaxEnum);
	rShaderBinary shaderBinary = m_ShaderCache.getShaderBinary(usage, shaderName, m_Engine);
	if (shaderBinary.Binary)
	{
		return shaderBinary;
	}

	auto shaderAssetPtr = m_Bucket.getAsset(shaderName);
	assert(shaderAssetPtr->type() == AssetFile::eShader);

	shaderBinary = assetTool::getShaderBinary(m_Engine, usage, shaderAssetPtr);
	assert(shaderBinary.Binary && shaderBinary.Size > 0ull);

	return shaderBinary;
}

rTextureBinary rAssetBucket::getTextureBinary(const std::string &textureName)
{
	auto textureAssetPtr = m_Bucket.getAsset(textureName);
	assert(textureAssetPtr->type() == AssetFile::eWICTexture ||
		textureAssetPtr->type() == AssetFile::eDDSTexture ||
		textureAssetPtr->type() == AssetFile::eVulkanTexture);

	rTextureBinary textureBinary;
	if (m_Engine == Configurations::eVulkan)
	{
		textureBinary = assetTool::getTextureBinary(m_Engine, textureAssetPtr);
	}
	else
	{
		assert(0);
	}

	return textureBinary;
}

std::string getShaderCode(const std::shared_ptr<byte> &shaderBinary, size_t shaderBinarySize, Configurations::eRenderEngine engine)
{
	std::vector<uint32_t> spirvBinary(shaderBinarySize / sizeof(uint32_t));
	verify(memcpy_s((void *)spirvBinary.data(), shaderBinarySize, (void *)shaderBinary.get(), shaderBinarySize) == 0);

	std::string result;

	if (engine == Configurations::eD3D11)
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
		Logger::instance().log(Logger::eError, "Failed to compile shader, error info: %s", (char8_t *)pErrMsg->GetBufferPointer());
		assert(0);
	}

	size_t bufferSize = pResult->GetBufferSize();
	rAsset::rShaderBinary result;
	result.Size = bufferSize;
	result.Binary.reset(new byte[bufferSize]);
	verify(memcpy_s((void *)result.Binary.get(), bufferSize, (void *)pResult->GetBufferPointer(), bufferSize) == 0);
	
	return result;
}

void buildShaderReflections(Configurations::eRenderEngine engine, rAsset::rShaderBinary &binary)
{
	if (engine == Configurations::eVulkan)
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
				GfxShader::GfxShaderReflection reflection
				{
					///compiler.get_decoration(res.id, spv::DecorationDescriptorSet), uniform (set = 0, binding = 1)
					(uint32_t)type,
					useConstantRange ? std::numeric_limits<uint32_t>().max() : compiler.get_decoration(res.id, spv::DecorationBinding)
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
	else if (engine == Configurations::eD3D11)
	{

	}
	else
	{
		assert(0);
	}
}

rAsset::rShaderBinary getShaderBinary(Configurations::eRenderEngine engine, eRShaderUsage usage, assetFilePtr &shaderAssetPtr)
{
	/// SPIR-V: The Standard, Portable Intermediate Representation
	assert(shaderAssetPtr);

	auto pCode = shaderAssetPtr->data();
	assert(pCode);

	std::string inputFile(shaderAssetPtr->fullPath());
	std::string outputPath = File::directory(getApplicationPath()) + "\\Intermediate\\AssetTool";
	if (!File::isDirectoryExists(outputPath))
	{
		File::createDirectory(outputPath);
	}
	std::string outputFile = outputPath + "\\";
	outputFile += shaderAssetPtr->name() + ".spirv";

	std::string Commandline = getEnvironmentVariable("VK_SDK_PATH") + "\\Bin\\glslangValidator ";

	switch (usage)
	{
	case eVertexShader:
		Commandline += "-S vert ";
		Commandline += "-DVertexShader ";
		break;
	case eFragmentShader:
		Commandline += "-S frag ";
		Commandline += "-DFragmentShader ";
		break;
	default:
		assert(0);
		break;
	}

	Commandline += "-e main ";

#if defined(_DEBUG)
	Commandline += "-g ";
#endif
	Commandline += "-V ";

	Commandline += "-o ";
	Commandline += outputFile;
	Commandline += " ";
	Commandline += inputFile;

	assert(Gear::executeProcess(Commandline, true));

	Gear::File spirvFile(outputFile);
	size_t spirvSize = spirvFile.size();
	assert((spirvSize % sizeof(uint32_t)) == 0u);

	rAsset::rShaderBinary result;

	if (engine == Configurations::eVulkan)
	{
		result.Binary.reset(new byte[spirvSize]());
		result.Size = spirvSize;
		verify(memcpy_s((void *)result.Binary.get(), spirvSize, (void *)spirvFile.data(File::eBinary).get(), spirvSize) == 0);
	}
	else if (engine == Configurations::eD3D11)
	{
		std::string shaderCode = getShaderCode(spirvFile.data(File::eBinary), spirvSize, engine);
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

namespaceEnd(assetTool)