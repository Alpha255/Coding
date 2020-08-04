#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <stack>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>
#include <set>
#include <random>
#include <condition_variable>
#include <bitset>

#include "Gear/Definitions.Generated.h"

using char8_t = char;
using uchar8_t = unsigned char;
using float32_t = float;
using double64_t = double;
using bool8_t = bool;
using long32_t = long;
using ulong32_t = unsigned long;
using long64_t = long long;
using ulong64_t = unsigned long long;
using byte8_t = unsigned char;

static_assert(sizeof(char8_t) == 1ull, "Size of char miss match.");
static_assert(sizeof(uchar8_t) == 1ull, "Size of unsigned char miss match.");
static_assert(sizeof(float32_t) == 4ull, "Size of float miss match.");
static_assert(sizeof(double64_t) == 8ull, "Size of double miss match.");
static_assert(sizeof(bool8_t) == 1ull, "Size of bool miss match.");
static_assert(sizeof(long32_t) == 4ull, "Size of long miss match.");
static_assert(sizeof(long64_t) == 8ull, "Size of long long miss match.");
static_assert(sizeof(ulong32_t) == 4ull, "Size of long miss match.");
static_assert(sizeof(ulong64_t) == 8ull, "Size of long long miss match.");
static_assert(sizeof(byte8_t) == 1ull, "Size of byte miss match.");

#define namespaceStart(name) namespace name {
#define namespaceEnd(name) }

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

#define enumToString(enumValue) #enumValue

#if defined(UsingAsDynamicLib)
	#define exportAPI __declspec(dllexport)
#else
	#define exportAPI __declspec(dllimport)
#endif

#define verify(condition) \
{                         \
	if (!(condition))     \
	{                     \
		assert(0);        \
	}                     \
}

#if defined(Platform_Win32)

#include <Windows.h>
#include <shlobj.h>

#define verify_Log(condition)                                                                          \
{                                                                                                      \
	if (!(condition))                                                                                  \
	{                                                                                                  \
		uint32_t errorCode = (uint32_t)::GetLastError();                                               \
		std::string errorMsg = Gear::getErrorMessage(errorCode);                                       \
		Gear::Logger::instance().log(Gear::Logger::eError,                                             \
			"Failed to invoke WINAPI, error code = %u, error info = %s", errorCode, errorMsg.c_str()); \
		assert(0);                                                                                     \
	}                                                                                                  \
}

#if defined(max)
	#undef max
#endif

#if defined(min)
	#undef min
#endif

#define appMainEntry(appName)                                  \
int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t) \
{                                                              \
	appName app_##appName;                                     \
	app_##appName.initialize(#appName);                        \
	app_##appName.loop();                                      \
	app_##appName.finalize();                                  \
}

#elif defined(Platform_Linux)

#define verify_Log(condition)

#define appMainEntry(appName) 

#endif

