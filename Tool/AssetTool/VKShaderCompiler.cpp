#include "AssetTool.h"
#include "Base/AssetFile.h"
#include "Colorful/Vulkan/VulkanTypes.h"

#if defined(max)
	#undef max
#endif

#include <glslang/SPIRV/GlslangToSpv.h>

NamespaceBegin(AssetTool)

const static TBuiltInResource s_DefaultTBuiltInResource =
{
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,

	/* .limits = */
	{
	/* .nonInductiveForLoops = */ 1,
	/* .whileLoops = */ 1,
	/* .doWhileLoops = */ 1,
	/* .generalUniformIndexing = */ 1,
	/* .generalAttributeMatrixVectorIndexing = */ 1,
	/* .generalVaryingIndexing = */ 1,
	/* .generalSamplerIndexing = */ 1,
	/* .generalVariableIndexing = */ 1,
	/* .generalConstantMatrixVectorIndexing = */ 1,
	}
};

bool CompileShader(
	AssetFile &asset,
	const std::string &entryPoint,
	const uint32_t shaderStage,
	__out std::vector<uint32_t> &binary)
{
	EShLanguage language = EShLangCount;
	switch (shaderStage)
	{
	case eVertexShader:
		language = EShLangVertex;
		break;
	case ePixelShader:
		language = EShLangFragment;
		break;
	default:
		assert(0);
		break;
	}

	auto shaderCode = asset.Load();

	/// WTF ???
	glslang::InitializeProcess();
	glslang::InitializeProcess();  /// also test reference counting of users
	glslang::InitializeProcess();  /// also test reference counting of users
	glslang::FinalizeProcess();    /// also test reference counting of users
	glslang::FinalizeProcess();    /// also test reference counting of users

	EShMessages message = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules | EShMsgReadHlsl);
	const char *pShaderCodes[] = { (const char *)(shaderCode.get()) };

	glslang::TShader shader(language);
	shader.setStrings(pShaderCodes, _countof(pShaderCodes));
	shader.setEntryPoint(entryPoint.c_str());

	const int32_t defaultVersion = 100U; /// use 100 for ES environment, 110 for desktop
	if (!shader.parse(&s_DefaultTBuiltInResource, defaultVersion, false, message))
	{
		Base::Log("Compile failed- %s.", asset.GetName().c_str());
		Base::Log(shader.getInfoLog());
		Base::Log(shader.getInfoDebugLog());
		return false;
	}

	glslang::TProgram program;
	program.addShader(&shader);

	if (!program.link(message))
	{
		Base::Log("Link failed- %s.", asset.GetName().c_str());
		Base::Log(shader.getInfoLog());
		Base::Log(shader.getInfoDebugLog());
		return false;
	}

	glslang::GlslangToSpv(*program.getIntermediate(language), binary);
	glslang::FinalizeProcess();

	return binary.size() > 0U;
}

/// Standard, Portable Intermediate Representation - V
std::vector<uint32_t> compileShader(const std::string &fileName, const std::string &entryName, uint32_t shaderStage, bool8_t bUsingParser)
{
	std::vector<uint32_t> spirv;
	auto shaderFile = assetBucket::instance().getAsset(fileName);
	assert(shaderFile);

	if (bUsingParser)
	{
		std::string inputFile(shaderFile->getFullPath());
		std::string outputFile = file::stripExtension(inputFile) + ".spirv";

		char8_t parserPath[MAX_PATH] = {};
		verifyWin(::GetEnvironmentVariableA("VULKAN_SDK", parserPath, MAX_PATH) != 0);
		std::string commandline(parserPath);
		commandline += "\\Bin\\glslangValidator ";

		switch (shaderStage)
		{
		case eVertexShader:
			commandline += "-S vert ";
			break;
		case ePixelShader:
			commandline += "-S frag ";
			break;
		default:
			assert(0);
			break;
		}

		commandline += "-e ";
		commandline += entryName;

#if defined(_DEBUG)
		commandline += "-g ";
#endif
		commandline += "-V ";

		commandline += "-o ";
		commandline += outputFile;
		commandline += " ";
		commandline += inputFile;

		assert(gear::executeProcess(commandline, true));

		gear::fileIO shaderBinary(outputFile);
		size_t shaderBinarySize = shaderBinary.getSize();
		assert((shaderBinarySize % sizeof(uint32_t)) == 0u);
		spirv.resize(shaderBinarySize / sizeof(uint32_t));
		verify(memcpy_s((void *)spirv.data(), shaderBinarySize, (void *)shaderBinary.getData(gear::fileIO::eBinary).get(), shaderBinarySize) == 0);
	}
	else
	{
		EShLanguage language = EShLangCount;
		switch (shaderStage)
		{
		case eVertexShader:
			language = EShLangVertex;
			break;
		case ePixelShader:
			language = EShLangFragment;
			break;
		default:
			assert(0);
			break;
		}

		/// WTF ???
		glslang::InitializeProcess();
		glslang::InitializeProcess();
		glslang::InitializeProcess();
		glslang::FinalizeProcess();
		glslang::FinalizeProcess();

		EShMessages message = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		glslang::TShader shader(language);
		const char *pShaderCodes[] = { (const char8_t *)shaderFile->getData().get() };
		shader.setStrings(pShaderCodes, _countof(pShaderCodes));
		shader.setEntryPoint(entryName.c_str());

		const int32_t defaultVersion = 100u; /// use 100 for ES environment, 110 for desktop
		if (!shader.parse(&s_DefaultTBuiltInResource, defaultVersion, false, message))
		{
			Base::Log("Shader compile failed: %s.", shaderFile->getName().c_str());
			Base::Log(shader.getInfoLog());
			Base::Log(shader.getInfoDebugLog());
			return std::vector<uint32_t>();
		}

		glslang::TProgram program;
		program.addShader(&shader);

		if (!program.link(message))
		{
			Base::Log("Shader link failed: %s.", shaderFile->getName().c_str());
			Base::Log(shader.getInfoLog());
			Base::Log(shader.getInfoDebugLog());
			return std::vector<uint32_t>();
		}

		glslang::GlslangToSpv(*program.getIntermediate(language), spirv);
	}

	return spirv;
}

NamespaceEnd(AssetTool)