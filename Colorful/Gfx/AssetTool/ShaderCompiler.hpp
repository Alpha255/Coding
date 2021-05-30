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

	static void compileToSPIRV(const char8_t* const source, const char8_t* entry, EShaderStage stage, ShaderDesc& desc)
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

		spv::SpvBuildLogger logger;
		glslang::SpvOptions options;

		glslang::GlslangToSpv(*intermediate, desc.Binary, &logger, &options);
		if (desc.Binary.size() == 0u)
		{
			LOG_ERROR("ShaderCompiler:: %s", logger.getAllMessages().c_str());
			assert(0);
		}
		desc.BinarySize = desc.Binary.size() * sizeof(uint32_t);

		glslang::FinalizeProcess();
	}

	static void compileToSPIRV_DXC(const char8_t* const source, const char8_t* entry, EShaderStage stage, ShaderDesc& desc)
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
		std::vector<uint32_t> spirv(static_cast<size_t>(std::ceil(blob->GetBufferSize() / sizeof(uint32_t))));
		VERIFY(memcpy_s(spirv.data(), blob->GetBufferSize(), blob->GetBufferPointer(), blob->GetBufferSize()) == 0);

#if 0
		std::string hlsl(std::move(compileSPIRVToHLSL(spirv)));
		std::string glsl(std::move(compileSPIRVToGLSL(spirv)));
#endif
		desc.Binary = std::move(spirv);
		desc.BinarySize = blob->GetBufferSize();
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

		if (FAILED(D3DCompile2(
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
			error.reference())))
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

	static void getReflection_Vk(ShaderDesc& desc)
	{
		assert(desc.BinarySize);

		spirv_cross::Compiler compiler(desc.Binary.data(), desc.Binary.size());
		auto activeVars = compiler.get_active_interface_variables();
		spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources(activeVars);
		compiler.set_enabled_interface_variables(std::move(activeVars));

		for each (auto& uniformBuffer in shaderResources.uniform_buffers)
		{
			auto type = compiler.get_type(uniformBuffer.type_id);
			auto binding = compiler.get_decoration(uniformBuffer.id, spv::Decoration::DecorationBinding);
			auto size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));

			desc.Reflections.ResourceBindings.push_back({
				EDescriptorType::UniformBuffer,
					binding,
					size,
					uniformBuffer.name
				});
			++desc.Reflections.UniformBufferCount;
		}
		for each (auto& sampledImage in shaderResources.sampled_images)
		{
			auto binding = compiler.get_decoration(sampledImage.id, spv::Decoration::DecorationBinding);
			desc.Reflections.ResourceBindings.push_back({
				EDescriptorType::CombinedImageSampler,
					binding,
					~0u,
					sampledImage.name
				});
			++desc.Reflections.SampledTextureCount;
		}
		for each (auto& image in shaderResources.separate_images)
		{
			auto binding = compiler.get_decoration(image.id, spv::Decoration::DecorationBinding);
			desc.Reflections.ResourceBindings.push_back({
				EDescriptorType::SampledImage,
					binding,
					~0u,
					image.name
				});
			++desc.Reflections.TextureCount;
		}
		for each (auto& sampler in shaderResources.separate_samplers)
		{
			auto binding = compiler.get_decoration(sampler.id, spv::Decoration::DecorationBinding);
			desc.Reflections.ResourceBindings.push_back({
				EDescriptorType::Sampler,
					binding,
					~0u,
					sampler.name
				});
			++desc.Reflections.SamplerCount;
		}
	}

	static void getReflection_D3D11(ShaderDesc& desc)
	{
		assert(desc.BinarySize);

		D3D11ShaderReflection d3dReflection;
		VERIFY(D3DReflect(desc.Binary.data(), desc.BinarySize, __uuidof(ID3D11ShaderReflection), reinterpret_cast<void**>(d3dReflection.reference())) == S_OK);

		D3D11_SHADER_DESC shaderDesc;
		VERIFY(d3dReflection->GetDesc(&shaderDesc) == S_OK);
		for (uint32_t i = 0u; i < shaderDesc.ConstantBuffers; ++i)
		{
			D3D11_SHADER_BUFFER_DESC bufferDesc;
			VERIFY(d3dReflection->GetConstantBufferByIndex(i)->GetDesc(&bufferDesc) == S_OK);

			if (bufferDesc.Type == D3D_CT_CBUFFER) /// D3D_CT_TBUFFER: A buffer containing texture data
			{
				D3D11_SHADER_INPUT_BIND_DESC bindDesc;
				VERIFY(d3dReflection->GetResourceBindingDesc(i, &bindDesc) == S_OK);

				desc.Reflections.ResourceBindings.push_back({
					EDescriptorType::UniformBuffer,
						bindDesc.BindPoint,
						bufferDesc.Size
					});
				++desc.Reflections.UniformBufferCount;
			}
		}
		for (uint32_t i = 0u; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			VERIFY(d3dReflection->GetResourceBindingDesc(i, &bindDesc) == S_OK);

			switch (bindDesc.Type)
			{
			case D3D_SIT_CBUFFER:
			case D3D_SIT_TBUFFER:
				break;
			case D3D_SIT_TEXTURE:
				desc.Reflections.ResourceBindings.push_back({
					EDescriptorType::SampledImage,
						bindDesc.BindPoint
					});
				++desc.Reflections.TextureCount;
				break;
			case D3D_SIT_SAMPLER:
				desc.Reflections.ResourceBindings.push_back({
					EDescriptorType::Sampler,
						bindDesc.BindPoint
					});
				++desc.Reflections.SamplerCount;
				break;
			case D3D_SIT_UAV_RWTYPED:
				break;
			}
		}
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
