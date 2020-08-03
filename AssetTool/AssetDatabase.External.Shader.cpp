#include "AssetDatabase.h"
#include "ShaderCache.h"
#include <ThirdParty/SPIRV-Cross/spirv_hlsl.hpp>

namespaceStart(AssetTool)

std::string makeHLSLCode(const std::shared_ptr<byte>& spirv, size_t size)
{
	std::vector<uint32_t> spirvBinary(size / sizeof(uint32_t));
	verify(memcpy_s((void *)spirvBinary.data(), size, (void *)spirv.get(), size) == 0);

	std::string hlsl;
	spirv_cross::CompilerHLSL compiler(std::move(spirvBinary));
	spirv_cross::CompilerHLSL::Options options;
	options.shader_model = 50u;
	compiler.set_hlsl_options(options);

	hlsl = compiler.compile();

	return hlsl;
}

/// Macros, Includes???
ShaderBinary compileHLSL(eRShaderUsage usage, const std::string& hlsl)
{
	const char8_t* const shaderModels[eRShaderUsage_MaxEnum] =
	{
		"vs_5_0",
		"hs_5_0",
		"ds_5_0",
		"gs_5_0",
		"ps_5_0",
		"cs_5_0"
	};

	ID3DBlob* result = nullptr;
	ID3DBlob* errMsg = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	auto compileResult = D3DCompile(
		(const void* )hlsl.c_str(),
		hlsl.length(),
		nullptr,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		shaderModels[usage],
		flags,
		0U,
		&result,
		&errMsg);

	if (compileResult != 0)
	{
		Logger::instance().log(Logger::eError, "Failed to compile shader, error info: %s", (char8_t*)errMsg->GetBufferPointer());
		assert(0);
	}

	size_t size = result->GetBufferSize();
	ShaderBinary binary;
	binary.Size = size;
	binary.Binary.reset(new byte[size]);
	verify(memcpy_s((void*)binary.Binary.get(), size, (void*)result->GetBufferPointer(), size) == 0);
	
	return binary;
}

void buildShaderReflections(Configurations::eRenderEngine engine, ShaderBinary& binary)
{
	if (engine == Configurations::eVulkan)
	{
		std::vector<uint32_t> spirv(binary.Size / sizeof(uint32_t));
		verify(memcpy_s((void*)spirv.data(), binary.Size, (void*)binary.Binary.get(), binary.Size) == 0);
		spirv_cross::Compiler compiler(std::move(spirv));
		auto activeVars = compiler.get_active_interface_variables();
		spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources(activeVars);
		compiler.set_enabled_interface_variables(std::move(activeVars));

		auto pushReflection = [&](
			const spirv_cross::SmallVector<spirv_cross::Resource>& resources,
			VkDescriptorType type)
		{
			for each (auto& res in resources)
			{
				GfxShaderReflection reflection
				{
					///compiler.get_decoration(res.id, spv::DecorationDescriptorSet), uniform (set = 0, binding = 1)
					(uint32_t)type,
					compiler.get_decoration(res.id, spv::DecorationBinding)
				};
				binary.Reflections.emplace_back(std::move(reflection));
			}
		};

		pushReflection(shaderResources.separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER);
		pushReflection(shaderResources.sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		pushReflection(shaderResources.separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		pushReflection(shaderResources.storage_images, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		pushReflection(shaderResources.uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		pushReflection(shaderResources.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		pushReflection(shaderResources.push_constant_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC); /// Hack
	}
	else if (engine == Configurations::eD3D11)
	{
		ID3D11ShaderReflection* reflection = nullptr;
		verify(D3DReflect(binary.Binary.get(), binary.Size, IID_ID3D11ShaderReflection, (void**)&reflection) == S_OK);
		
		D3D11_SHADER_DESC shaderDesc{};
		verify(reflection->GetDesc(&shaderDesc) == S_OK);
		for (uint32_t i = 0u; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC inputBindDesc{};
			verify(reflection->GetResourceBindingDesc(i, &inputBindDesc) == S_OK);

			binary.Reflections.emplace_back(GfxShaderReflection
				{
					(uint32_t)inputBindDesc.Type,
					inputBindDesc.BindPoint
				});
		}
	}
	else
	{
		assert(0);
	}
}

AssetTool::ShaderBinary AssetDatabase::tryToGetShaderBinary(
	Configurations::eRenderEngine engine, 
	eRShaderUsage usage, 
	const std::string& shaderName)
{
	/// SPIR-V: The Standard, Portable Intermediate Representation
	auto binary = ShaderCache::instance().tryToGetShaderBinary(engine, usage, shaderName);
	if (binary.Size > 0ull && binary.Binary)
	{
		return binary;
	}

	auto shader = tryToGetAsset(shaderName);
	if (!shader)
	{
		return binary;
	}

	auto code = shader->data();
	assert(code);

	std::string inputFile(shader->fullPath());
	std::string outputPath = File::directory(getApplicationPath()) + "\\Intermediate\\AssetTool";
	if (!File::isDirectoryExists(outputPath))
	{
		File::createDirectory(outputPath);
	}
	std::string outputFile = outputPath + "\\";
	outputFile += shader->name() + ".spirv";

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

	verify_Log(Gear::executeProcess(Commandline, true));

	Gear::File spirvFile(outputFile);
	size_t spirvSize = spirvFile.size();
	assert((spirvSize % sizeof(uint32_t)) == 0u);

	if (engine == Configurations::eVulkan)
	{
		binary.Binary.reset(new byte[spirvSize]());
		binary.Size = spirvSize;
		verify(memcpy_s((void *)binary.Binary.get(), spirvSize, (void *)spirvFile.data(File::eBinary).get(), spirvSize) == 0);
	}
	else if (engine == Configurations::eD3D11)
	{
		std::string hlsl = makeHLSLCode(spirvFile.data(File::eBinary), spirvSize);
		assert(hlsl.length() > 0u);
		binary = compileHLSL(usage, hlsl);
	}
	else
	{
		assert(0);
	}

	buildShaderReflections(engine, binary);

	return binary;
}

namespaceEnd(AssetTool)