#pragma once

#include <stdint.h>
#include <assert.h>
#include <memory>

template<typename T> struct node
{
	inline node() = default;
	inline node(const T &value)
		: Data(value)
	{
	}

	node *Prev = nullptr;
	node *Next = nullptr;

	T Data = 0;
};

template<typename T> inline void free(T *&pMemory)
{
	if (pMemory)
	{
		delete pMemory;
		pMemory = nullptr;
	}
}

template<typename T> inline void freeArray(T *&pMemory)
{
	if (pMemory)
	{
		delete[] pMemory;
		pMemory = nullptr;
	}
}
