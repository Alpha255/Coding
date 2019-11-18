#pragma once

#include "Gear/Public/Extension/Commandline.h"
#include <ThirdParty/json/single_include/nlohmann/json.hpp>

namespaceStart(gear)

struct appConfig
{
	enum eRenderEngine
	{
		eUnknown,
		eOpenGL,
		eD3D11,
		eD3D12,
		eVulkan,
		eRenderEngine_MaxEnum
	};

	uint32_t WindowWidth = 1280u;
	uint32_t WindowHeight = 720u;
	bool8_t FullScreen = false;
	eRenderEngine RenderEngine = eD3D11;
	nlohmann::json ConfigJson;

	void load();
};

namespaceEnd(gear)
