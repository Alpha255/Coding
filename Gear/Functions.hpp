#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

template<class T>
void safeDelete(T& object)
{
	if (object)
	{
		delete object;
		object = nullptr;
	}
}

template<class T>
void safeDeleteArray(T& object)
{
	if (object)
	{
		delete[] object;
		object = nullptr;
	}
}

template<class T>
void safeRelease(T& object)
{
	if (object)
	{
		object->Release();
	}
}

template <class Left, class Right>
inline bool8_t isEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

/// https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
template<class T> void hash_combine(size_t& seed, const T& v)
{
	seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<class FirstArg, class... RestArgs>
void hash_combine(size_t& seed, const FirstArg& firstArg, const RestArgs&... restArgs)
{
	hash_combine(seed, firstArg);
	hash_combine(seed, restArgs...);
}

template<class... Args>
size_t computeHash(const Args&... args)
{
	size_t seed = 0u;
	hash_combine(seed, args...);
	return seed;
}

NAMESPACE_END(Gear)
