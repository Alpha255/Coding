#include "VulkanShader.h"
#include "VulkanEngine.h"
#include "Base/AssetFile.h"

#if defined max
	#undef max
#endif

#include <glslang/SPIRV/GlslangToSpv.h>

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

void VulkanInputLayout::Create(const void *, const std::vector<Geometry::VertexLayout> &layouts)
{
	assert(!m_bValid && layouts.size() > 0U);

	m_VertexInputAttrs.clear();

	size_t stride = 0U;
	for (uint32_t i = 0U; i < layouts.size(); ++i)
	{
		VkVertexInputAttributeDescription inputAttrDesc
		{
			i,
			0U,
			(VkFormat)layouts[i].Format,
			layouts[i].Offset
		};
		m_VertexInputAttrs.emplace_back(inputAttrDesc);

		stride += layouts[i].Stride;
	}

	m_InputBinding = VkVertexInputBindingDescription
	{
		0U,
		(uint32_t)stride,
		VK_VERTEX_INPUT_RATE_VERTEX
	};

	m_CreateInfo = VkPipelineVertexInputStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&m_InputBinding,
		(uint32_t)m_VertexInputAttrs.size(),
		m_VertexInputAttrs.data()
	};

	m_bValid = true;
}

std::vector<uint32_t> VulkanShader::Compile(const std::string &fileName, const std::string &entryPoint)
{
	AssetFile shaderFile(fileName);
	auto shaderCode = shaderFile.Load();

	EShLanguage language = EShLangCount;
	switch (m_Type)
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
	glslang::InitializeProcess();  // also test reference counting of users
	glslang::InitializeProcess();  // also test reference counting of users
	glslang::FinalizeProcess();    // also test reference counting of users
	glslang::FinalizeProcess();    // also test reference counting of users
	
	EShMessages message = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules/* | EShMsgReadHlsl*/);
	const char *pShaderCodes[] = { (const char *)(shaderCode.get()) };

	glslang::TShader shader(language);
	shader.setStrings(pShaderCodes, _countof(pShaderCodes));
	shader.setEntryPoint(entryPoint.c_str());

	const int32_t defaultVersion = 100U; /// use 100 for ES environment, 110 for desktop
	if (!shader.parse(&s_DefaultTBuiltInResource, defaultVersion, false, message))
	{
		Base::Log("Compile failed- %s.", fileName.c_str());
		Base::Log(shader.getInfoLog());
		Base::Log(shader.getInfoDebugLog());
		assert(0);
	}

	glslang::TProgram program;
	program.addShader(&shader);

	if (!program.link(message))
	{
		Base::Log("Link failed- %s.", fileName.c_str());
		Base::Log(shader.getInfoLog());
		Base::Log(shader.getInfoDebugLog());
		assert(0);
	}

	std::vector<uint32_t> spv;
	glslang::GlslangToSpv(*program.getIntermediate(language), spv);
	glslang::FinalizeProcess();

	assert(spv.size() > 0U);

	return spv;
}

void VulkanShader::Create(const std::string &fileName, const std::string &entryPoint)
{
	assert(!IsValid());

	m_EntryName = entryPoint;

	auto shaderBinary = Compile(fileName, entryPoint);

	VkShaderModuleCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0U,
		shaderBinary.size() * sizeof(int32_t),
		shaderBinary.data()
	};
	Check(vkCreateShaderModule(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	m_ShaderStage = VkPipelineShaderStageCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0U,
		m_Stage,
		m_Handle,
		m_EntryName.c_str(),
		nullptr
	};
}

void VulkanShader::Destory()
{
	assert(IsValid());

	vkDestroyShaderModule(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}
