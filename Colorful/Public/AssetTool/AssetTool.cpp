#include "Colorful/Public/AssetTool/AssetTool.h"
#include "Colorful/Public/AssetTool/ShaderCompiler.hpp"
#include "Colorful/Public/AssetTool/TextureLoader.hpp"

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

std::vector<uint32_t> AssetTool::compileShader(EShaderLanguage language, EShaderStage stage, const char8_t* const code)
{
	assert(code && stage < EShaderStage::ShaderStageCount);

	const char8_t* const entry = "main";

#if 0
	if (language == EShaderLanguage::HLSL)
	{
		return ShaderCompiler::compileToSPIRV_DXC(code, entry, stage);
	}
#endif

	std::vector<uint32_t> binary(std::move(ShaderCompiler::compileToSPIRV(code, entry, stage)));
	if (language == EShaderLanguage::GLSL)
	{
		auto reflection = ShaderCompiler::getReflection_Vk(binary);
	}

	if (language == EShaderLanguage::HLSL)
	{
		std::string hlsl(std::move(ShaderCompiler::compileSPIRVToHLSL(binary)));
		D3DShaderBlob blob = ShaderCompiler::compileToD3D(hlsl.c_str(), entry, stage);
		auto reflection = ShaderCompiler::getReflection_D3D11(blob->GetBufferPointer(), blob->GetBufferSize());

		assert(blob->GetBufferSize() % sizeof(uint32_t) == 0u);
	}

	return binary;
}

TextureDesc AssetTool::loadTexture(const std::string& name)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	return TextureLoader::load(asset);
}

std::vector<uint32_t> AssetTool::loadShader(EShaderLanguage language, EShaderStage stage, const std::string& name)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	return compileShader(language, stage, reinterpret_cast<const char8_t*>(asset->data().get()));
}

void AssetTool::loadMaterial(const std::string& name)
{
	AssetPtr asset = findAsset(name);
}

NAMESPACE_END(Gfx)