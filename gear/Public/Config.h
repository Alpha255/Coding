#pragma once

#include "gear/Public/Commandline.h"

namespaceStart(gear)

class appConfig
{
public:
	enum eRenderEngine
	{
		eUnknown,
		eD3D11,
		eVulkan,
		eRenderEngine_MaxEnum
	};

	void load();

	uint32_t getWidth() const
	{
		return m_Width;
	}

	uint32_t getHeight() const
	{
		return m_Height;
	}

	bool8_t isFullScreen() const
	{
		return m_bFullScreen;
	}

	eRenderEngine getRenderEngine() const
	{
		return m_RenderEngine;
	}
protected:
private:
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	bool8_t m_bFullScreen = false;
	eRenderEngine m_RenderEngine = eUnknown;

	commandline m_Commandline;
};

namespaceEnd(gear)
