#include <ThirdParty/tinyxml2/tinyxml2.h>

NAMESPACE_START(Gfx)

class MaterialLoader
{
public:
	static MaterialDesc loadMaterial(const char8_t* name)
	{
		assert(name);

		auto asset = AssetTool::instance().findAsset(name);
		assert(asset);

		return loadMaterial(reinterpret_cast<const char8_t*>(asset->data().get()), asset->size());
	}

	static MaterialDesc loadMaterial(const char8_t* data, size_t size)
	{
		assert(data && size);

		MaterialDesc desc;

		tinyxml2::XMLDocument doc;
		VERIFY(doc.Parse(data, size) == tinyxml2::XMLError::XML_SUCCESS);

		desc.Name.assign(doc.RootElement()->Name());
		for (auto node = doc.FirstChild()->FirstChild(); node; node = node->NextSibling())
		{
			if (_stricmp(node->Value(), "InputLayout") == 0)
			{
				loadInputLayouts(node, desc);
			}
			else if (_stricmp(node->Value(), "Shader") == 0)
			{
				loadShaders(node, desc);
			}
			else if (_stricmp(node->Value(), "Textures") == 0)
			{
				assert(0);
			}
		}

		return desc;
	}

protected:
	static void loadInputLayouts(const tinyxml2::XMLNode* root, MaterialDesc& desc)
	{
		assert(root);

		for (auto node = root->FirstChild(); node; node = node->NextSibling())
		{
			if (_stricmp(node->Value(), "Stream") == 0)
			{
				VertexInputLayoutDesc::VertexInput vertexInput;
				for (auto childNode = node->FirstChild(); childNode; childNode = childNode->NextSibling())
				{
					auto element = childNode->ToElement();
					assert(element);

					if (_stricmp(element->Name(), "Attribute") == 0)
					{
						vertexInput.Binding = std::atoi(element->Attribute("Binding"));
						auto inputRate = element->Attribute("InputRate");
						if (_stricmp(inputRate, "Vertex") == 0)
						{
							vertexInput.InputRate = VertexInputLayoutDesc::EVertexInputRate::Vertex;
						}
						else if (_stricmp(inputRate, "Instance") == 0)
						{
							vertexInput.InputRate = VertexInputLayoutDesc::EVertexInputRate::Instance;
						}
						else
						{
							assert(0);
						}
					}
					else if (_stricmp(element->Name(), "Layout") == 0)
					{
						auto formatAttr = FormatAttribute::attribute(element->Attribute("Format"));
						VertexInputLayoutDesc::VertexLayout layout
						{
							static_cast<uint32_t>(std::atoi(element->Attribute("Location"))),
							static_cast<uint32_t>(formatAttr.Stride / BITS_IN_BYTES()),
							formatAttr.Format,
							element->Attribute("Usage")
						};
						vertexInput.appendVertexLayout(layout);
					}
				}

				desc.InputLayoutDesc.VertexInputs.emplace_back(std::move(vertexInput));
			}
		}

		assert(desc.InputLayoutDesc.VertexInputs.size());
	}

	static void loadShaders(const tinyxml2::XMLNode* root, MaterialDesc& desc)
	{
		assert(root);

		for (auto node = root->FirstChild(); node; node = node->NextSibling())
		{			
			auto language = _stricmp(node->ToElement()->Attribute("Language"), "GLSL") == 0 ? EShaderLanguage::GLSL : EShaderLanguage::HLSL;
			auto file = node->ToElement()->Attribute("File");

			EShaderStage shaderStage = EShaderStage::ShaderStageCount;
			if (_stricmp(node->Value(), "VertexShader") == 0)
			{
				shaderStage = EShaderStage::Vertex;
			}
			else if (_stricmp(node->Value(), "HullShader") == 0)
			{
				shaderStage = EShaderStage::Hull;
			}
			else if (_stricmp(node->Value(), "DomainShader") == 0)
			{
				shaderStage = EShaderStage::Domain;
			}
			else if (_stricmp(node->Value(), "GeometryShader") == 0)
			{
				shaderStage = EShaderStage::Geometry;
			}
			else if (_stricmp(node->Value(), "FragmentShader") == 0)
			{
				shaderStage = EShaderStage::Fragment;
			}
			else if (_stricmp(node->Value(), "ComputeShader") == 0)
			{
				shaderStage = EShaderStage::Compute;
			}
			assert(shaderStage != EShaderStage::ShaderStageCount);

			desc.ShaderDescs[shaderStage].Language = language;
			if (node->FirstChild())
			{
				auto text = node->FirstChild()->ToText();
				if (text->CData())
				{
					desc.ShaderDescs[shaderStage] = std::move(AssetTool::instance().compileShader(language, shaderStage, text->Value()));
				}
			}
			else if (file)
			{
				auto shaderFile = AssetTool::instance().findAsset(file);
				assert(shaderFile);
				desc.ShaderDescs[shaderStage] = std::move(AssetTool::instance().compileShader(language, shaderStage,
					reinterpret_cast<const char8_t*>(shaderFile->data().get())));
			}
			else
			{
				assert(0);
			}
		}
	}
};

NAMESPACE_END(Gfx)