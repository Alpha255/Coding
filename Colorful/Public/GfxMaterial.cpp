#include "Colorful/Public/GfxMaterial.h"
#include "Colorful/Public/AssetTool/AssetTool.h"
#include <ThirdParty/tinyxml2/tinyxml2.h>

NAMESPACE_START(Gfx)

Material::Material(const std::string& name)
{
	////auto shader = AssetTool::instance().loadShader(EShaderLanguage::HLSL, EShaderStage::Vertex, "BasicHLSL11_VS.hlsl");

	auto asset = AssetTool::instance().findAsset(name);
	assert(asset);

	tinyxml2::XMLDocument doc;
	VERIFY(doc.LoadFile(asset->fullPath().c_str()) == tinyxml2::XMLError::XML_SUCCESS);

	m_Name = doc.FirstChild()->Value();
	assert(m_Name.length());

	for (auto node = doc.FirstChild()->FirstChild(); node; node = node->NextSibling())
	{
		if (_stricmp(node->Value(), "InputLayout") == 0)
		{
			loadInputLayouts(node);
		}
		else if (_stricmp(node->Value(), "Shader") == 0)
		{
			loadShaders(node);
		}
		else if (_stricmp(node->Value(), "ShaderResource") == 0)
		{
			loadShaderResources(node);
		}
	}
}

void Material::loadInputLayouts(const tinyxml2::XMLNode* root)
{
	for (auto node = root->FirstChild(); node; node = node->NextSibling())
	{
		assert(_stricmp(node->Value(), "Location") == 0);
		auto element = node->ToElement();
		assert(element);

		auto usage = element->Attribute("Usage");
		auto format = element->Attribute("Format");
	}
}

void Material::loadShaders(const tinyxml2::XMLNode* root)
{
	for (auto node = root->FirstChild(); node; node = node->NextSibling())
	{
		auto text = node->FirstChild()->ToText();
		assert(text && text->CData());

		auto code = text->Value();
		assert(code);

		if (_stricmp(node->Value(), "VertexShader") == 0)
		{
			
		}
		else if (_stricmp(node->Value(), "HullShader") == 0)
		{

		}
		else if (_stricmp(node->Value(), "DomainShader") == 0)
		{

		}
		else if (_stricmp(node->Value(), "GeometryShader") == 0)
		{

		}
		else if (_stricmp(node->Value(), "FragmentShader") == 0)
		{

		}
		else if (_stricmp(node->Value(), "ComputeShader") == 0)
		{

		}
	}
}

void Material::loadShaderResources(const tinyxml2::XMLNode* root)
{
	for (auto node = root->FirstChild(); node; node = node->NextSibling())
	{

	}
}

NAMESPACE_END(Gfx)