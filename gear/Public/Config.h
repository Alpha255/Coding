#pragma once

#include "gear/Public/Commandline.h"

namespaceStart(gear)

struct appConfig
{
	enum eRenderEngine
	{
		eUnknown,
		eD3D11,
		eVulkan,
		eRenderEngine_MaxEnum
	};

	uint32_t WindowWidth = 0u;
	uint32_t WindowHeight = 0u;
	bool8_t FullScreen = false;
	eRenderEngine RenderEngine = eUnknown;

	void load();
};

namespaceEnd(gear)
