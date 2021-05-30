#include "Colorful/Gfx/AssetTool/AssetTool.h"
#include "Colorful/Gfx/AssetTool/ShaderCompiler.hpp"
#include "Colorful/Gfx/AssetTool/TextureLoader.hpp"
#include "Colorful/Gfx/AssetTool/ModelLoader.hpp"
#include "Colorful/Gfx/AssetTool/MaterialLoader.hpp"
#include "Colorful/Gfx/GfxDevice.h"

NAMESPACE_START(Gfx)

static std::string s_AssetDirectory;
GfxDevice* s_GfxDevice = nullptr;

AssetTool& AssetTool::instance()
{
	static AssetTool s_Instance;
	return s_Instance;
}

void AssetTool::initialize(const std::string& baseDirectory, GfxDevice* gfxDevice)
{
	assert(gfxDevice && File::isExists(baseDirectory.c_str(), true));

	s_GfxDevice = gfxDevice;
	s_AssetDirectory = File::directory(baseDirectory) + "\\Assets";
	assert(File::isExists(s_AssetDirectory.c_str(), true));
	System::setWorkingDirectory(s_AssetDirectory.c_str());
	LOG_INFO("AssetDatabase:: Mount working directory to \"%s\"", System::getCurrentWorkingDirectory().c_str());
}

AssetPtr AssetTool::findAsset(const char8_t* name)
{
	std::string path = File::find(s_AssetDirectory.c_str(), name);
	if (File::isExists(path.c_str()))
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

	ShaderDesc desc;
	desc.Stage = stage;
	ShaderCompiler::compileToSPIRV(code, entry, stage, desc);

	if (language == EShaderLanguage::GLSL)
	{
		ShaderCompiler::getReflection_Vk(desc);
	}

	if (language == EShaderLanguage::HLSL)
	{
		std::string hlsl(std::move(ShaderCompiler::compileSPIRVToHLSL(desc.Binary)));
		D3DShaderBlob blob = ShaderCompiler::compileToD3D(hlsl.c_str(), entry, stage);
		ShaderCompiler::getReflection_D3D11(desc);

		assert(blob->GetBufferSize() % sizeof(uint32_t) == 0u);
	}

	return desc;
}

TexturePtr AssetTool::loadTexture(const char8_t* name, bool8_t sRGB)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	auto data = asset->data(File::EMode::Binary); /// Holding data
	TextureDesc desc(std::move(TextureLoader::load(asset->extension(), data.get(), asset->size(), sRGB)));
	desc.Name = asset->name();

	switch (desc.Type)
	{
	case ETextureType::T_1D:
	case ETextureType::T_1D_Array:
		return s_GfxDevice->create1DTexture(desc);
	case ETextureType::T_2D:
	case ETextureType::T_2D_Array:
	case ETextureType::T_Cube:
	case ETextureType::T_Cube_Array:
		return s_GfxDevice->create2DTexture(desc);
	case ETextureType::T_3D:
		return s_GfxDevice->create3DTexture(desc);
	default:
		assert(0);
		return nullptr;
	}
}

ShaderPtr AssetTool::loadShader(EShaderLanguage language, EShaderStage stage, const char8_t* name)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	ShaderDesc desc(std::move(compileShader(language, stage, reinterpret_cast<const char8_t*>(asset->data().get()))));

	switch (stage)
	{
	case EShaderStage::Vertex:
		return s_GfxDevice->createVertexShader(desc);
	case EShaderStage::Hull:
		return s_GfxDevice->createHullShader(desc);
	case EShaderStage::Domain:
		return s_GfxDevice->createDomainShader(desc);
	case EShaderStage::Geometry:
		return s_GfxDevice->createGeometryShader(desc);
	case EShaderStage::Compute:
		return s_GfxDevice->createComputeShader(desc);
	case EShaderStage::Fragment:
		return s_GfxDevice->createFragmentShader(desc);
	default:
		assert(0);
		return nullptr;
	}
}

ModelPtr AssetTool::loadModel(const char8_t* name, const Model::LoadingSettings& options)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	auto desc = std::move(ModelLoader::loadModel(asset->fullPath().c_str(), options));
	return std::make_shared<Model>(desc);
}

MaterialPtr AssetTool::loadMaterial(const char8_t* name)
{
	AssetPtr asset = findAsset(name);
	assert(asset);

	auto desc = std::move(MaterialLoader::loadMaterial(
		reinterpret_cast<const char8_t*>(asset->data().get()), 
		asset->size()));

	return std::make_shared<Material>(desc);
}

void AssetTool::finalize()
{
}

NAMESPACE_END(Gfx)