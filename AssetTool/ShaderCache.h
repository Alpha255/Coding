#pragma once

#include "Asset.h"

namespaceStart(AssetTool)

exportAPI class ShaderCache : public Singleton<ShaderCache>
{
public:
	void initialize();

	ShaderBinary tryToGetShaderBinary(eRShaderUsage usage, const std::string& shaderName, Configurations::eRenderEngine engine);
protected:
private:
};

namespaceEnd(AssetTool)
