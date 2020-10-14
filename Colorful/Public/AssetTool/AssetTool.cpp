#include "Colorful/Public/AssetTool/AssetTool.h"
#include "Colorful/Public/AssetTool/ShaderCompiler.hpp"

NAMESPACE_START(Gfx)

static AssetToolPtr s_Instance = nullptr;
static std::string s_AssetDirectory;

AssetTool& AssetTool::instance()
{
	if (!s_Instance)
	{
		s_Instance = std::make_unique<AssetTool>();
	}

	return *s_Instance;
}

void AssetTool::initialize(const std::string& baseDirectory)
{
	assert(File::isExists(baseDirectory, true));

	s_AssetDirectory = File::directory(baseDirectory) + "\\Assets";
	assert(File::isExists(s_AssetDirectory, true));
	System::setWorkingDirectory(s_AssetDirectory);
	LOG_INFO("AssetDatabase:: Mount working directory to \"%s\"", System::getCurrentWorkingDirectory().c_str());
}

AssetPtr AssetTool::findAsset(const std::string& name)
{
	std::string path = File::find(s_AssetDirectory, name);
	if (File::isExists(path))
	{
		return std::make_shared<Asset>(path);
	}

	return AssetPtr();
}

std::vector<uint32_t> AssetTool::loadShader(EShaderLanguage language, EShaderStage stage, const std::string& name)
{
	AssetPtr asset = findAsset(name);
	if (!asset)
	{
		return std::vector<uint32_t>();
	}

	const char8_t* const entry = "main";

	auto code = asset->data();

#if 0
	if (language == EShaderLanguage::HLSL)
	{
		return ShaderCompiler::compileToSPIRV_DXC(reinterpret_cast<const char8_t* const>(code.get()), entry, stage);
	}
#endif

	std::vector<uint32_t> binary(std::move(ShaderCompiler::compileToSPIRV(reinterpret_cast<const char8_t* const>(code.get()), entry, stage)));

	if (language == EShaderLanguage::HLSL)
	{
		std::string hlsl(std::move(ShaderCompiler::compileSPIRVToHLSL(binary)));
		D3DShaderBlob blob = ShaderCompiler::compileToD3D(hlsl.c_str(), entry, stage);

		D3D11ShaderReflection reflection;
		VERIFY(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflection.reference())) == S_OK);
		
		D3D11_SHADER_DESC shaderDesc{};
		VERIFY(reflection->GetDesc(&shaderDesc));
		for (uint32_t i = 0u; i < shaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc{};
			VERIFY(reflection->GetInputParameterDesc(i, &inputParamDesc) == S_OK);
		}
		for (uint32_t i = 0u; i < shaderDesc.ConstantBuffers; ++i)
		{
			D3D11_SHADER_BUFFER_DESC bufferDesc{};
			VERIFY(reflection->GetConstantBufferByIndex(i)->GetDesc(&bufferDesc) == S_OK);
		}
		for (uint32_t i = 0u; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc{};
			VERIFY(reflection->GetResourceBindingDesc(i, &bindDesc) == S_OK);
		}
	}

	return binary;
}

void AssetTool::loadMaterial(const std::string& name)
{
	AssetPtr asset = findAsset(name);
}

NAMESPACE_END(Gfx)