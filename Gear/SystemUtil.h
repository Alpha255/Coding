#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

std::string getErrorMessage(uint32_t errorCode);

std::string getApplicationPath();

bool8_t executeProcess(const std::string &Commandline, bool8_t bWaitUntilDone = true);

void sleep(uint32_t microseconds);

Math::Rect getWindowRect(uint64_t windowHandle);

std::string getEnvironmentVariable(const std::string &envVarName);

uint64_t getAppInstance();

Math::Vec2 getDesktopWindowSize();

struct DynamicLibrary
{
	uint64_t Handle = 0u;

	void load(const std::string &libraryName);

	void *getProcAddress(const std::string &procName);

	void free();

	DynamicLibrary() = default;
	DynamicLibrary(const std::string &libraryName)
	{
		load(libraryName);
	}

	~DynamicLibrary()
	{
		free();
	}
};

NAMESPACE_END(Gear)
