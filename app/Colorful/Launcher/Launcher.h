#pragma once

#include "app/Colorful/vkTest/vkTest.h"
#include "app/Colorful/vkMultithread/vkMultiThread.h"
#include "gear/Public/Config.h"

class launcher
{
public:
	inline void loadConfig()
	{
		m_Config.load();
	}

	void registerMetaClasses();

	void launch();
protected:
private:
	static std::unordered_map<std::string, std::unique_ptr<application>> s_Apps;
	gear::appConfig m_Config;
};

class appFactory
{
public:
protected:
private:
};
