#pragma once

#include "Asset.h"

namespaceStart(AssetTool)

class exportAPI ShaderCache : public Singleton<ShaderCache>
{
public:
	void initialize();

	ShaderBinary tryToGetShaderBinary(Configurations::eRenderEngine engine, eRShaderUsage usage, const std::string& shaderName);
protected:
private:
};

namespaceEnd(AssetTool)
