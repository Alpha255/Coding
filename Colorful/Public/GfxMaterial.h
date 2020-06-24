#pragma once

#include "Colorful/Public/GfxResource.h"

struct GfxMaterial
{
	std::vector<GfxVertexAttributes> InputLayout;
	std::string ShaderNames[eRShaderUsage_MaxEnum];
	GfxShaderPtr Shaders[eRShaderUsage_MaxEnum];
	std::string Name;

	void serialize();
	void deserialize();
};
