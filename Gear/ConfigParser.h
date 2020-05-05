#pragma once

#include "Gear/CommandlineParser.h"

namespaceStart(Gear)

struct Configurations
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
	bool8_t VSync = false;
	bool8_t FullScreen = false;
	bool8_t D3DTripleBuffer = true;
	bool8_t EnableVulkanValidation = true;
	bool8_t VulkanValidationVerbose = false;
	eRenderEngine RenderEngine = eD3D11;

	void load();
};

namespaceEnd(Gear)
