#include "ShaderCache.h"

namespaceStart(AssetTool)

void ShaderCache::initialize()
{
}

ShaderBinary ShaderCache::tryToGetShaderBinary(Configurations::eRenderEngine, eRShaderUsage, const std::string &)
{
	return ShaderBinary();
}

namespaceEnd(AssetTool)
