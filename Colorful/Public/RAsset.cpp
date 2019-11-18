#include "RAsset.h"

namespaceStart(rAsset)

rTexturePtr createTextureFromFile(const std::string &, const appConfig &)
{
	return nullptr;
}

std::string rShaderParser::translateType(const std::string &srcType, appConfig::eRenderEngine engine)
{
	std::string result;
	if (appConfig::eD3D11 != engine)
	{
		return result;
	}

	auto shaderParserConfigPtr = config.ConfigJson.find("shaderParser");
	assert(shaderParserConfigPtr != config.ConfigJson.end());

	auto translaterPtr = shaderParserConfigPtr->find("translater");
	assert(translaterPtr != shaderParserConfigPtr->end());

	auto targetPtr = translaterPtr->find(srcType);
	if (targetPtr != translaterPtr->end())
	{
		result = targetPtr.value();
	}

	return result;
}

std::string rShaderParser::parseAttribute(eRShaderUsage usage, eShaderAttribute attribute, const pugi::xml_document &shaderDoc)
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

void rShaderParser::parse(eRShaderUsage usage, const std::string &fileName, appConfig::eRenderEngine)
{
	auto shaderAssetPtr = assetBucket::instance().getAsset(fileName);
	assert(shaderAssetPtr);

	pugi::xml_document xmlShaderDoc;
	pugi::xml_parse_result result = xmlShaderDoc.load_file(shaderAssetPtr->getFullPath().c_str());
	if (!result)
	{
		logger::instance().log(logger::eError, "Failed to load shader file \"%s\", error info: ",
			fileName.c_str(), result.description());
		assert(0);
	}

	std::string path("/XMLShader/");
	std::string shaderUsage = getShaderUsageName(usage);
	path += shaderUsage;
	pugi::xpath_node shaderNode = xmlShaderDoc.select_node(path.c_str());
	if (shaderNode.node().empty())
	{
		logger::instance().log(logger::eError, "Can't find target \"%s\" in shader file: %s",
			shaderUsage.c_str(), fileName.c_str());
		assert(0);
	}
}

namespaceEnd(rAsset)
