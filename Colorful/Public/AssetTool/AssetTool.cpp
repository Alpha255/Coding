#include "Colorful/Public/AssetTool/AssetTool.h"
#include "Colorful/Public/AssetTool/ShaderCompiler.hpp"
#include "Colorful/Public/AssetTool/TextureLoader.hpp"
#include "Colorful/Public/AssetTool/ModelLoader.hpp"

NAMESPACE_START(Gfx)

static AssetToolPtr s_Instance = nullptr;

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

	m_AssetDirectory = File::directory(baseDirectory) + "\\Assets";
	assert(File::isExists(m_AssetDirectory, true));
	System::setWorkingDirectory(m_AssetDirectory);
	LOG_INFO("AssetDatabase:: Mount working directory to \"%s\"", System::getCurrentWorkingDirectory().c_str());
}

AssetPtr AssetTool::findAsset(const std::string& name)
{
	std::string path = File::find(m_AssetDirectory, name);
	if (File::isExists(path))
	{
		return std::make_shared<Asset>(path);
	}

	return AssetPtr();
}

ShaderDesc AssetTool::compileShader(EShaderLanguage language, EShaderStage stage, const char8_t* const code)
{
	assert(code && stage < EShaderStage::ShaderStageCount);

	const char8_t* const entry = "main";

#if 0
	if (language == EShaderLanguage::HLSL)
	{
		return ShaderCompiler::compileToSPIRV_DXC(code, entry, stage);
	}
#endif

	ShaderDesc desc{};
	desc.Stage = stage;
	desc.Binary = std::move(ShaderCompiler::compileToSPIRV(code, entry, stage));

	if (language == EShaderLanguage::GLSL)
	{
		desc.Reflection = std::move(ShaderCompiler::getReflection_Vk(desc.Binary));
	}

	if (language == EShaderLanguage::HLSL)
	{
		std::string hlsl(std::move(ShaderCompiler::compileSPIRVToHLSL(desc.Binary)));
		D3DShaderBlob blob = ShaderCompiler::compileToD3D(hlsl.c_str(), entry, stage);
		desc.Reflection = std::move(ShaderCompiler::getReflection_D3D11(blob->GetBufferPointer(), blob->GetBufferSize()));

		assert(blob->GetBufferSize() % sizeof(uint32_t) == 0u);
	}

	return desc;
}

TextureDesc AssetTool::loadTexture(const std::string& name, bool8_t sRGB)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	return TextureLoader::load(asset, sRGB);
}

ShaderDesc AssetTool::loadShader(EShaderLanguage language, EShaderStage stage, const std::string& name)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	auto dataPtr = asset->data();
	return compileShader(language, stage, reinterpret_cast<const char8_t*>(dataPtr.get()));
}

ModelDesc AssetTool::loadModel(const std::string& name)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	auto dataPtr = asset->data();
	return ModelLoader::loadModel(dataPtr.get(), asset->size());
}

void AssetTool::loadMaterial(const std::string& name)
{
	AssetPtr asset = findAsset(name);
}

NAMESPACE_END(Gfx)