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
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

/// https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
template<class T> void hash_combine(size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

NAMESPACE_END(Gear)
