#pragma once

#include "gear/Public/Commandline.h"

namespaceStart(gear)

class appConfig
{
public:
	void load();

	uint32_t getWindowWidth() const
	{
		return m_WindowWidth;
	}

	uint32_t getWindowHeight() const
	{
		return m_WindowHeight;
	}

	bool8_t getFullScreen() const
	{
		return m_bFullScreen;
	}
protected:
private:
	uint32_t m_WindowWidth = 0u;
	uint32_t m_WindowHeight = 0u;
	bool8_t m_bFullScreen = false;

	commandline m_Commandline;
};

namespaceEnd(gear)
