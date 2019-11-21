#pragma once

#include "Colorful/Public/RInterface.h"

namespace pugi
{
	class xml_document;
}

namespaceStart(rAsset)

rTexturePtr createTextureFromFile(const std::string &fileName, appConfig::eRenderEngine engine);

struct rShaderBinary
{
	byte *Binary = nullptr;
	size_t Size = 0ull;
};

struct rShaderParser
{
	std::string parse(eRShaderUsage usage, const assetFilePtr &assetPtr, appConfig::eRenderEngine engine);

protected:
	enum eShaderAttribute
	{
		eInputLayout,
		eResourceBinding,
		eOutput,
		eUniformBuffer,
		eTexture,
		eSampler,
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
public:
	rShaderBinary getShaderBinary(eRShaderUsage, const std::string &)
	{
		return rShaderBinary();
	}
protected:
private:
};

class rAssetBucket : public singleton<rAssetBucket>
{
	singletonDeclare(rAssetBucket);
public:
	void initialize(appConfig::eRenderEngine engine);

	rShaderBinary getShaderBinary(eRShaderUsage usage, const std::string &shaderName, const rDevicePtr &devicePtr);

	void finalize() {}
protected:
	inline std::string getShaderCode(eRShaderUsage usage, const std::string &shaderName)
	{
		auto shaderPtr = m_Bucket.getAsset(shaderName);
		assert(shaderPtr);

		return m_ShaderParser.parse(usage, shaderPtr, m_Engine);
	}
private:
	rShaderParser m_ShaderParser;
	rShaderCache m_ShaderCache;
	assetBucket m_Bucket;
	appConfig::eRenderEngine m_Engine = appConfig::eD3D11;
};

namespaceEnd(rAsset)
