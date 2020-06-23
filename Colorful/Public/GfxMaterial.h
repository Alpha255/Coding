#pragma once

#include "Colorful/Public/GfxResource.h"

struct GfxMaterial
{
	std::vector<GfxVertexAttributes> InputLayout;
	GfxShaderPtr Shaders[eRShaderUsage_MaxEnum];
	std::string Name;

};
