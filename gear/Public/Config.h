#pragma once

#include "gear/Public/Commandline.h"

namespaceStart(gear)

class appConfig
{
public:
	void load();

	std::string getAppName() const
	{
		return m_AppName;
	}

	uint32_t getWindowWidth() const
	{
		return m_WindowWidth;
	}

	uint32_t getWindowHeight() const
	{
		return m_WindowHeight;
	}
protected:
private:
	std::string m_AppName;
	uint32_t m_WindowWidth = 0u;
	uint32_t m_WindowHeight = 0u;

	commandline m_Commandline;
};

namespaceEnd(gear)
