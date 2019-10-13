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

#define verifyWin(condition)                                  \
{                                                             \
	if (!(condition))                                         \
	{                                                         \
		gear::log("gear: Error code = %u", ::GetLastError()); \
		assert(0);                                            \
	}                                                         \
}

typedef char char8_t;
typedef unsigned char uchar8_t;
typedef float float32_t;
typedef double double64_t;
typedef bool bool8_t;
typedef long long32_t;

