#include "Colorful/Public/GfxMaterial.h"
#include "Colorful/Public/AssetTool/AssetTool.h"
#include <ThirdParty/tinyxml2/tinyxml2.h>

NAMESPACE_START(Gfx)

Material::Material(const std::string& name)
{
	auto asset = AssetTool::instance().findAsset(name);
	assert(asset);

	auto dataPtr = asset->data();

	tinyxml2::XMLDocument doc;
	VERIFY(doc.Parse(reinterpret_cast<const char8_t*>(dataPtr.get()), asset->size()) == tinyxml2::XMLError::XML_SUCCESS);

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
	}
}

void Material::loadInputLayouts(const tinyxml2::XMLNode* root)
{
	assert(root);
	std::vector<VertexInputDesc> descs{};
	for (auto node = root->FirstChild(); node; node = node->NextSibling())
	{
		if (_stricmp(node->Value(), "Stream") == 0)
		{
			VertexInputDesc desc{};
			for (auto childNode = node->FirstChild(); childNode; childNode = childNode->NextSibling())
			{
				auto element = childNode->ToElement();
				assert(element);

				if (_stricmp(element->Name(), "Attribute") == 0)
				{
					desc.Slot = std::atoi(element->Attribute("Slot"));
					auto inputRate = element->Attribute("InputRate");
					if (_stricmp(inputRate, "Vertex") == 0)
					{
						desc.InputRate = VertexInputDesc::EVertexInputRate::Vertex;
					}
					else if (_stricmp(inputRate, "Instance") == 0)
					{
						desc.InputRate = VertexInputDesc::EVertexInputRate::Instance;
					}
					else
					{
						assert(0);
					}
				}
				else if (_stricmp(element->Name(), "Layout") == 0)
				{
					auto formatAttr = FormatAttribute::attribute(element->Attribute("Format"));
					VertexInputDesc::VertexLayout layout
					{
						std::atoi(element->Attribute("Location")),
						static_cast<uint32_t>(formatAttr.Stride / BITS_IN_BYTES()),
						formatAttr.Format,
						element->Attribute("Usage")
					};
					desc.Layouts.emplace_back(std::move(layout));
				}
			}

			descs.emplace_back(std::move(desc));
		}
	}

	assert(descs.size());
}

void Material::loadShaders(const tinyxml2::XMLNode* root)
{
	assert(root);
	for (auto node = root->FirstChild(); node; node = node->NextSibling())
	{
		auto text = node->FirstChild()->ToText();
		assert(text && text->CData());

		auto code = text->Value();
		assert(code);

		auto stage = EShaderStage::ShaderStageCount;
		if (_stricmp(node->Value(), "VertexShader") == 0)
		{
			stage = EShaderStage::Vertex;
		}
		else if (_stricmp(node->Value(), "HullShader") == 0)
		{
			stage = EShaderStage::Hull;
		}
		else if (_stricmp(node->Value(), "DomainShader") == 0)
		{
			stage = EShaderStage::Domain;
		}
		else if (_stricmp(node->Value(), "GeometryShader") == 0)
		{
			stage = EShaderStage::Geometry;
		}
		else if (_stricmp(node->Value(), "FragmentShader") == 0)
		{
			stage = EShaderStage::Fragment;
		}
		else if (_stricmp(node->Value(), "ComputeShader") == 0)
		{
			stage = EShaderStage::Compute;
		}

		assert(stage != EShaderStage::ShaderStageCount);
		auto desc = AssetTool::instance().compileShader(EShaderLanguage::GLSL, stage, code);
	}
}

NAMESPACE_END(Gfx)