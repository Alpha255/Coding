#pragma once

#include "Gear/Public/Extension/Asset/AssetBucket.h"
#include "Gear/Public/Extension/Config.h"
#include "REnumerations.h"
#include "RInterface.h"
#include <ThirdParty/pugixml/src/pugixml.hpp>

namespaceStart(rAsset)

rTexturePtr createTextureFromFile(const std::string &fileName, appConfig::eRenderEngine engine);

struct rShaderParser
{
	std::string ShaderCode[eRShaderUsage::eRShaderUsage_MaxEnum];

	void parse(eRShaderUsage usage, const std::string &fileName, appConfig::eRenderEngine engine);

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
	inline void initialize(appConfig::eRenderEngine engine)
	{
		m_Engine = engine;
		m_Bucket.initialize();
	}
protected:
private:
	rShaderParser m_ShaderParser;
	rShaderCache m_ShaderCache;
	assetBucket m_Bucket;
	appConfig::eRenderEngine m_Engine = appConfig::eD3D11;
};

namespaceEnd(rAsset)
