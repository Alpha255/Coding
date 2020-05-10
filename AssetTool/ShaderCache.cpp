#include "ShaderCache.h"

namespaceStart(AssetTool)

void ShaderCache::initialize()
{
}

ShaderBinary ShaderCache::tryToGetShaderBinary(eRShaderUsage, const std::string &, Configurations::eRenderEngine)
{
	return ShaderBinary();
}

namespaceEnd(AssetTool)
