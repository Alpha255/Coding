#include "RAsset.h"
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

std::vector<byte> rAssetBucket::getShaderBinary(eRShaderUsage usage, const std::string &shaderName)
{
	auto shaderPtr = m_Bucket.getAsset(shaderName);
	assert(shaderPtr);

	rShaderParser shaderParser;
	shaderParser.parse(usage, shaderPtr, m_Engine);

	std::string shaderCode = shaderParser.ShaderCode[usage];

	return std::vector<byte>();
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

void rShaderParser::parse(eRShaderUsage usage, const assetFilePtr &assetPtr, appConfig::eRenderEngine)
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
}

namespaceEnd(rAsset)
