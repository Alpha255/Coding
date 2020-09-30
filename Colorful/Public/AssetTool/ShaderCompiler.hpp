#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/glslang/glslang/Public/ShaderLang.h>
#include <ThirdParty/glslang/StandAlone/ResourceLimits.h>
#include <ThirdParty/glslang/SPIRV/GlslangToSpv.h>

NAMESPACE_START(Gfx)

/// Support Macro and Include ??? 
struct ShaderMacro
{
	std::string Name;
	std::string Definition;
};

class ShaderCompiler
{
public:
	static std::vector<uint32_t> compileToSpirv(const char8_t* const source, const char8_t* entry, EShaderStage stage)
	{
		assert(source && entry && stage < EShaderStage::ShaderStageCount);

		EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);
		const char8_t* const sources[] = { source };
		glslang::TShader::ForbidIncluder noneIncluder;

		glslang::InitializeProcess();

		glslang::TShader shader(language(stage));
		shader.setStrings(sources, _countof(sources));
		shader.setEntryPoint(entry);
		shader.setSourceEntryPoint(entry);  /// ???
		shader.setPreamble(nullptr);        /// ???
		shader.addProcesses({});            /// ???

		if (!shader.parse(&glslang::DefaultTBuiltInResource, 
			EVersion::OGL_Desktop, 
			EProfile::ENoProfile, 
			false, 
			false, 
			messages, 
			noneIncluder))
		{
			LOG_ERROR("ShaderCompiler:: %s. %s", shader.getInfoLog(), shader.getInfoDebugLog());
			assert(0);
		}

		glslang::TProgram program;
		program.addShader(&shader);

		if (!program.link(messages))
		{
			LOG_ERROR("ShaderCompiler:: %s. %s", program.getInfoLog(), program.getInfoDebugLog());
			assert(0);
		}

		glslang::TIntermediate* intermediate = program.getIntermediate(shader.getStage()); /// buildReflection
		assert(intermediate);

		std::vector<uint32_t> spirv{};
		spv::SpvBuildLogger logger;
		glslang::SpvOptions options;

		glslang::GlslangToSpv(*intermediate, spirv, &logger, &options);
		if (spirv.size() == 0u)
		{
			LOG_ERROR("ShaderCompiler:: %s", logger.getAllMessages().c_str());
			assert(0);
		}

		glslang::FinalizeProcess();

		return spirv;
	}

	static D3DShaderBlob compileToD3D(const char8_t* const source, const char8_t* entry, EShaderStage stage)
	{
		assert(source && entry && stage < EShaderStage::ShaderStageCount);

		D3DShaderBlob binary;
		D3DShaderBlob error;

#if defined(DEBUG) || defined(_DEBUG)
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS; /// Forces strict compile, which might not allow for legacy syntax.
#endif

		if (!D3DCompile2(
			source, 
			strlen(source), 
			nullptr,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry,
			shaderModel(stage),
			flags,
			0u,
			0u,
			nullptr,
			0u,
			binary.reference(),
			error.reference()))
		{
			LOG_ERROR("ShaderCompiler:: %s", reinterpret_cast<const char8_t* const>(error->GetBufferPointer()));
			assert(0);
		}

		return binary;
	}
protected:
	enum EVersion
	{
		OGL_ES = 100,
		OGL_Desktop = 110
	};

	static EShLanguage language(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:   return EShLangVertex;
		case EShaderStage::Hull:     return EShLangTessControl;
		case EShaderStage::Domain:   return EShLangTessEvaluation;
		case EShaderStage::Geometry: return EShLangGeometry;
		case EShaderStage::Fragment: return EShLangFragment;
		case EShaderStage::Compute:  return EShLangCompute;
		}

		assert(0);
		return EShLangCount;
	}

	static const char8_t* const shaderModel(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:   return "vs_5_0";
		case EShaderStage::Hull:     return "hs_5_0";
		case EShaderStage::Domain:   return "ds_5_0";
		case EShaderStage::Geometry: return "gs_5_0";
		case EShaderStage::Fragment: return "ps_5_0";
		case EShaderStage::Compute:  return "cs_5_0";
		}

		assert(0);
		return nullptr;
	}
};

NAMESPACE_END(Gfx)
