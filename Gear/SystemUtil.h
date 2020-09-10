#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

template <typename Left, typename Right>
inline bool8_t isEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

/// https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
template<class T> void hash_combine(size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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
