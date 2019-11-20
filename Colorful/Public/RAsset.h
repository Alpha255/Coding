#pragma once

#include "Colorful/Public/RInterface.h"
#include <ThirdParty/pugixml/src/pugixml.hpp>

namespaceStart(rAsset)

rTexturePtr createTextureFromFile(const std::string &fileName, appConfig::eRenderEngine engine);

struct rShaderParser
{
	std::string ShaderCode[eRShaderUsage::eRShaderUsage_MaxEnum];

	void parse(eRShaderUsage usage, const assetFilePtr &assetPtr, appConfig::eRenderEngine engine);

protected:
	enum eShaderAttribute
	{
		eInputLayout,
		eResourceBinding,
		eOutput,
		eUniformBuffer,
		eTexture,
		eCode,
		eShaderAttribute_MaxEnum
	};

	std::string translateType(const std::string &srcType, appConfig::eRenderEngine engine);
	std::string parseAttribute(eRShaderUsage usage, eShaderAttribute attribute, const pugi::xml_document &shaderDoc);
	
	inline std::string getShaderUsageName(eRShaderUsage usage)
	{
		switch (usage)
		{
		case eRShaderUsage::eVertexShader:
			return enumToString(eVertexShader);
		case eRShaderUsage::eHullShader:
			return enumToString(eHullShader);
		case eRShaderUsage::eDomainShader:
			return enumToString(eDomainShader);
		case eRShaderUsage::eGeometryShader:
			return enumToString(eGeometryShader);
		case eRShaderUsage::eFragmentShader:
			return enumToString(eFragmentShader);
		case eRShaderUsage::eComputeShader:
			return enumToString(eComputeShader);
		}

		assert(0);
		return "";
	}
};

class rShaderCache
{

};

class rAssetBucket : public singleton<rAssetBucket>
{
	singletonDeclare(rAssetBucket);
public:
	void initialize(appConfig::eRenderEngine engine);

	std::vector<byte> getShaderBinary(eRShaderUsage usage, const std::string &shaderName);
protected:
private:
	rShaderCache m_ShaderCache;
	assetBucket m_Bucket;
	appConfig::eRenderEngine m_Engine = appConfig::eD3D11;
};

namespaceEnd(rAsset)
