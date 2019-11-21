#include "RAsset.h"
#include <ThirdParty/pugixml/src/pugixml.hpp>
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(rAsset)

static nlohmann::json s_ShaderConfigJson;

rTexturePtr createTextureFromFile(const std::string &, const appConfig &)
{
	return nullptr;
}

void rAssetBucket::initialize(appConfig::eRenderEngine engine)
{
	m_Engine = engine;
	m_Bucket.initialize();

	std::string configPath(file::getFileDirectory(getApplicationPath()) + "\\shaderConfig.json");
	assert(file::isFileExists(configPath));
	std::ifstream filestream(configPath);
	filestream >> s_ShaderConfigJson;
	filestream.close();
}

rShaderBinary rAssetBucket::getShaderBinary(eRShaderUsage usage, const std::string &shaderName, const rDevicePtr &devicePtr)
{
	assert(usage < eRShaderUsage_MaxEnum && devicePtr);
	rShaderBinary shaderBinary = m_ShaderCache.getShaderBinary(usage, shaderName);
	if (shaderBinary.Binary)
	{
		return shaderBinary;
	}

	auto shaderAssetPtr = m_Bucket.getAsset(shaderName);
	std::string shaderCode = m_ShaderParser.parse(usage, shaderAssetPtr, m_Engine);

	return shaderBinary;
}

std::string rShaderParser::translateType(const std::string &srcType, appConfig::eRenderEngine engine)
{
	std::string result;
	if (appConfig::eD3D11 != engine)
	{
		return result;
	}

	auto shaderParserConfigPtr = s_ShaderConfigJson.find("shaderParser");
	assert(shaderParserConfigPtr != s_ShaderConfigJson.end());

	auto translaterPtr = shaderParserConfigPtr->find("translater");
	assert(translaterPtr != shaderParserConfigPtr->end());

	auto targetPtr = translaterPtr->find(srcType);
	if (targetPtr != translaterPtr->end())
	{
		result = targetPtr.value();
	}

	return result;
}

std::string rShaderParser::parseAttribute(eRShaderUsage, eShaderAttribute attribute, const pugi::xml_document &)
{
	std::string result;
	if (eInputLayout == attribute)
	{

	}
	else if (eUniformBuffer == attribute)
	{

	}
	else if (eTexture == attribute)
	{

	}
	else if (eCode == attribute)
	{

	}
	else
	{
		assert(0);
	}

	return result;
}

std::string rShaderParser::parse(eRShaderUsage usage, const assetFilePtr &assetPtr, appConfig::eRenderEngine engine)
{
	assert(assetPtr);

	pugi::xml_document xmlShaderDoc;
	pugi::xml_parse_result result = xmlShaderDoc.load_file(assetPtr->getFullPath().c_str());
	if (!result)
	{
		logger::instance().log(logger::eError, "Failed to load shader file \"%s\", error info: ",
			assetPtr->getName().c_str(), result.description());
		assert(0);
	}

	std::string path("/XMLShader/");
	std::string shaderUsage = getShaderUsageName(usage);
	path += shaderUsage;
	pugi::xpath_node shaderNode = xmlShaderDoc.select_node(path.c_str());
	if (shaderNode.node().empty())
	{
		logger::instance().log(logger::eError, "Can't find target \"%s\" in shader file: %s",
			shaderUsage.c_str(), assetPtr->getName().c_str());
		assert(0);
	}

	std::string input;
	std::string output;
	std::string resourceBinding;
	std::string codeBody;

	for (auto child = shaderNode.node().first_child(); !child.empty(); child = child.next_sibling())
	{
		eShaderAttribute shaderAttr = eShaderAttribute_MaxEnum;
		if (strcmp(child.name(), "eInput") == 0)
		{
			shaderAttr = eInputLayout;
		}
		else if (strcmp(child.name(), "eOutput") == 0)
		{
			shaderAttr = eOutput;
		}
		else if (strcmp(child.name(), "eResourceBinding") == 0)
		{
			shaderAttr = eResourceBinding;
		}

		for (auto attr = child.first_child(); !attr.empty(); attr = attr.next_sibling())
		{
			if (shaderAttr == eResourceBinding)
			{
				for (auto res = attr.first_child(); !res.empty(); res = res.next_sibling())
				{
					if (strcmp(res.name(), "eUniformBuffer"))
					{
						shaderAttr = eUniformBuffer;
					}
					else if (strcmp(res.name(), "eTexture"))
					{
						shaderAttr = eTexture;
					}
					else if (strcmp(res.name(), "eSampler"))
					{
						shaderAttr = eSampler;
					}
				}
			}
			else
			{
				uint32_t index = 0u;
				const char8_t *pType = nullptr;
				const char8_t *pName = nullptr;
				const char8_t *pUsage = nullptr;

				std::string inputOrOutput;
				for (auto attrValue = attr.attributes_begin(); attrValue != attr.attributes_end(); ++attrValue)
				{
					if (strcmp(attrValue->name(), "type") == 0)
					{
						pType = attrValue->as_string();
					}
					else if (strcmp(attrValue->name(), "name") == 0)
					{
						pName = attrValue->as_string();
					}
					else if (strcmp(attrValue->name(), "usage") == 0)
					{
						pUsage = attrValue->as_string();
					}
				}

				if (engine == appConfig::eVulkan)
				{
					if (strcmp(pUsage, "SV_POSITION") == 0)
					{
						inputOrOutput += format("out gl_PerVertex\n{\n\t%s %s;\n};\n", pType, pName);
					}
					else
					{
						inputOrOutput += format("layout(location = %d) %s %s %s;\n",
							index++,
							shaderAttr == eInputLayout ? "in" : "out",
							pType,
							pName);
					}
				}
				else if (engine == appConfig::eD3D11)
				{
					inputOrOutput += format("struct %s\n{\n", shaderAttr == eInputLayout ? "Input" : "Output");
					std::string type = translateType(pType, engine);
					inputOrOutput += format("\t%s %s : %s;\n", type.c_str(), pName, pUsage);
				}

				if (shaderAttr == eInputLayout)
				{
					input += inputOrOutput;
				}
				else if (shaderAttr == eOutput)
				{
					output += inputOrOutput;
				}
			}
		}
	}

	if (engine == appConfig::eD3D11)
	{
		input += "};\n";
		output += "};\n";
	}

	return std::string("");
}

namespaceEnd(rAsset)
