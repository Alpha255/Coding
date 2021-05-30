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
#include <forward_list>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>
#include <set>
#include <unordered_set>
#include <random>
#include <condition_variable>
#include <bitset>
#include <tuple>
///#include <span>  /// cpp20

#include "Gear/Definitions.Generated.h"

#pragma warning(disable : 26812)

#if defined(PLATFORM_WIN32)
	#include <Windows.h>
	#include <windowsx.h>
	#include <shlobj.h>
#else
	#error Unknown platform!
#endif

#if defined(max)
	#undef max
#endif

#if defined(min)
	#undef min
#endif

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

#define NAMESPACE_START(Name) namespace Name {
#define NAMESPACE_END(Name) }

#define SAFE_RELEASE(Ptr) \
{                         \
	if((Ptr) != nullptr)  \
	{                     \
		(Ptr)->Release(); \
	}                     \
}

#define SAFE_DELETE(Ptr) \
{                        \
	if((Ptr) != nullptr) \
	{                    \
		delete (Ptr);    \
		(Ptr) = nullptr; \
	}                    \
}

#define SAFE_DELETE_ARRAY(Ptr) \
{                              \
	if((Ptr) != nullptr)       \
	{                          \
		delete[] (Ptr);        \
		(Ptr) = nullptr;       \
	}                          \
}

#if defined(DYNAMIC_LIB)
	#define EXPORT_API __declspec(dllexport)
#else
	#define EXPORT_API __declspec(dllimport)
#endif

#define VERIFY(Condition) \
{                         \
	if (!(Condition))     \
	{                     \
		assert(0);        \
	}                     \
}

#if defined(PLATFORM_WIN32)
#define VERIFY_SYSTEM(Condition)                                                           \
{                                                                                          \
	if (!(Condition))                                                                      \
	{                                                                                      \
		LOG_ERROR("Failed to invoke WINAPI, %s", Gear::System::getErrorMessage().c_str()); \
		assert(0);                                                                         \
	}                                                                                      \
}

#define DLL_POSTFIX ".dll"
#else
	#error Unknown platform!
#endif

#define DECLARE_SHARED_PTR(ClassName) using ClassName##Ptr = std::shared_ptr<ClassName>;
#define DECLARE_UNIQUE_PTR(ClassName) using ClassName##Ptr = std::unique_ptr<ClassName>;
#define DECLARE_PLAIN_PTR(ClassName)  using ClassName##Ptr = ClassName*;

