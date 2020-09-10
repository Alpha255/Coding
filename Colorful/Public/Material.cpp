#include "Colorful/Public/Material.h"
#include "AssetTool/AssetDatabase.h"
#include <iomanip>

Material::Material(const std::string& name)
{
	auto asset = AssetTool::AssetDatabase::instance().tryToGetAsset(name);
	assert(asset);

	auto file = std::static_pointer_cast<File>(asset);
	new (this)File(std::move(*file));
}

void Material::reload()
{
}

void Material::test()
{	
	auto inputPtr = std::make_shared<InputLayout>();
	inputPtr->Attributes.push_back(std::make_pair(ePosition, eRGB32_Float));
	inputPtr->Attributes.push_back(std::make_pair(eNormal, eRGB32_UInt));
	inputPtr->serialize(m_Json);

	Shader shaders;
	shaders.Target = "ImGui.shader";
	shaders.Usage = eVertexShader;
	auto asset = AssetTool::AssetDatabase::instance().tryToGetAsset(shaders.Target);
	shaders.Source = std::string(reinterpret_cast<char8_t*>(asset->data().get()));
	auto binary = AssetTool::AssetDatabase::instance().tryToGetShaderBinary(Configurations::eRenderEngine::eVulkan, shaders.Usage, shaders.Target);
	shaders.Binary = binary.Binary;
	shaders.serialize(m_Json);

	std::ofstream os("test.json", std::ios::out);
	os << std::setw(4) << m_Json << std::endl;
	os.close();
}
