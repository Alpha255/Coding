#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/glslang/glslang/Public/ShaderLang.h>
#include <ThirdParty/glslang/StandAlone/ResourceLimits.h>
#include <ThirdParty/glslang/SPIRV/GlslangToSpv.h>
#include <ThirdParty/SPIRV-Cross/spirv_hlsl.hpp>
#include <ThirdParty/dxc/include/dxc/dxcapi.h>

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
	class DxcLibrary final : public D3DObject<IDxcLibrary> {};
	class DxcCompiler final : public D3DObject<IDxcCompiler> {};
	class DxcBlobEncoding final : public D3DObject<IDxcBlobEncoding> {};
	class DxcOperationResult final : public D3DObject<IDxcOperationResult> {};
	class DxcBlob final : public D3DObject<IDxcBlob> {};

	static std::vector<uint32_t> compileToSPIRV(const char8_t* const source, const char8_t* entry, EShaderStage stage)
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

	static std::vector<uint32_t> compileToSPIRV_DXC(const char8_t* const source, const char8_t* entry, EShaderStage stage)
	{
		assert(source && entry && stage < EShaderStage::ShaderStageCount);

		DxcLibrary library;
		VERIFY(DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary), reinterpret_cast<void**>(library.reference())) == S_OK);

		DxcCompiler compiler;
		VERIFY(DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), reinterpret_cast<void**>(compiler.reference())) == S_OK);

		DxcBlobEncoding blobEncoding;
		VERIFY(library->CreateBlobWithEncodingFromPinned(source, static_cast<uint32_t>(strlen(source)), CP_UTF8, blobEncoding.reference()) == S_OK);

		std::string entryName(entry);
		std::wstring w_entry(entryName.cbegin(), entryName.cend());

		std::string model(shaderModel(stage, true));
		std::wstring w_model(model.cbegin(), model.cend());

		std::vector<const wchar_t*> args
		{
			L"-spirv"
		};

		DxcOperationResult opResult;
		VERIFY(compiler->Compile(
			blobEncoding.get(), 
			nullptr, 
			w_entry.c_str(), 
			w_model.c_str(),
			args.data(),
			static_cast<uint32_t>(args.size()),
			nullptr, 
			0u, 
			nullptr, 
			opResult.reference()) == S_OK);

		::HRESULT hr = E_FAIL;
		VERIFY(opResult->GetStatus(&hr) == S_OK);

		if (FAILED(hr))
		{
			DxcBlobEncoding error;
			VERIFY(opResult->GetErrorBuffer(error.reference()) == S_OK);
			LOG_ERROR("ShaderCompiler:: %s", static_cast<const char8_t*>(error->GetBufferPointer()));
		}

		DxcBlob blob;
		VERIFY(opResult->GetResult(blob.reference()) == S_OK);

		VERIFY(blob->GetBufferSize() % sizeof(uint32_t) == 0);
		std::vector<uint32_t> spirv(blob->GetBufferSize() / sizeof(uint32_t));
		VERIFY(memcpy_s(spirv.data(), blob->GetBufferSize(), blob->GetBufferPointer(), blob->GetBufferSize()) == 0);

#if 1
		std::string hlsl(std::move(compileSPIRVToHLSL(spirv)));
		std::string glsl(std::move(compileSPIRVToGLSL(spirv)));
#endif

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
			shaderModel(stage, false),
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

	static std::string compileSPIRVToHLSL(const std::vector<uint32_t>& spirv)
	{
		assert(spirv.size());

		spirv_cross::CompilerHLSL compiler(spirv.data(), spirv.size());
		spirv_cross::CompilerHLSL::Options options
		{
			50u
		};
		compiler.set_hlsl_options(options);

		std::string hlsl(std::move(compiler.compile()));

		return hlsl;
	}

	static std::string compileSPIRVToGLSL(const std::vector<uint32_t>& spirv)
	{
		assert(spirv.size());

		spirv_cross::CompilerGLSL compiler(spirv.data(), spirv.size());
		spirv_cross::CompilerGLSL::Options options{};
		options.vulkan_semantics = true;
		compiler.set_common_options(options);

		std::string glsl(std::move(compiler.compile()));

		return glsl;
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

	static const char8_t* const shaderModel(EShaderStage stage, bool8_t dxc)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:   return dxc ? "vs_6_0" : "vs_5_0";
		case EShaderStage::Hull:     return dxc ? "hs_6_0" : "hs_5_0";
		case EShaderStage::Domain:   return dxc ? "ds_6_0" : "ds_5_0";
		case EShaderStage::Geometry: return dxc ? "gs_6_0" : "gs_5_0";
		case EShaderStage::Fragment: return dxc ? "ps_6_0" : "ps_5_0";
		case EShaderStage::Compute:  return dxc ? "cs_6_0" : "cs_5_0";
		}

		assert(0);
		return nullptr;
	}
};

NAMESPACE_END(Gfx)
