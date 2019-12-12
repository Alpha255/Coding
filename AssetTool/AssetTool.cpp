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

namespaceEnd(assetTool)