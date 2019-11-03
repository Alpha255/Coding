#pragma once

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>
#include <condition_variable>
#include <Windows.h>

#define safeRelease(ptr)  \
{                         \
	if((ptr) != nullptr)  \
	{                     \
		(ptr)->Release(); \
		(ptr) = nullptr;  \
	}                     \
}

#define safeDelete(ptr)  \
{                        \
	if((ptr) != nullptr) \
	{                    \
		delete (ptr);    \
		(ptr) = nullptr; \
	}                    \
}

#define safeDeleteArray(ptr) \
{                            \
	if((ptr) != nullptr)     \
	{                        \
		delete[](ptr);       \
		(ptr) = nullptr;     \
	}                        \
}

#define namespaceStart(name) namespace name {
#define namespaceEnd(name) }

#define verify(condition) \
{                         \
	if (!(condition))     \
	{                     \
		assert(0);        \
	}                     \
}


#if defined(DEBUG) || defined(_DEBUG)
#define verifyWin(condition)                                                                                                   \
{                                                                                                                              \
	if (!(condition))                                                                                                          \
	{                                                                                                                          \
		uint32_t errorCode = (uint32_t)::GetLastError();                                                                       \
		std::string errorMsg = gear::getErrorMessage(errorCode);                                                               \
		gear::log(gear::eError, "Failed to invoke WINAPI, error code = %u, error info = \"%s\"", errorCode, errorMsg.c_str()); \
		assert(0);                                                                                                             \
	}                                                                                                                          \
}
#else
#define verifyWin(condition) verify(condition)
#endif

#define winMainEntry(appName)                                  \
int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t) \
{                                                              \
	appName app_##appName;                                     \
	app_##appName.initialize(#appName, 0);                     \
	app_##appName.loop();                                      \
	app_##appName.finalize();                                  \
}

#define enumToString(enumValue) #enumValue

#if defined(UsingAsDynamicLib)
	#define exportAPI __declspec(dllexport)
#else
	#define exportAPI __declspec(dllimport)
#endif

typedef char char8_t;
typedef unsigned char uchar8_t;
typedef float float32_t;
typedef double double64_t;
typedef bool bool8_t;
typedef long long32_t;

