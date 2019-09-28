#pragma once

#include "vkTest.h"

class launcher
{
public:
	void registerMetaClasses();

	void launch(const std::string &appName);
protected:
private:
	static std::unordered_map<std::string, std::unique_ptr<application>> s_Apps;
};

class appFactory
{
public:
protected:
private:
};

class appConfig
{

};
