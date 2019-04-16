#pragma once

#include <stdint.h>
#include <assert.h>
#include <memory>

template<typename T> struct node
{
	T *Next = nullptr;
	T *Prev = nullptr;

	T Data = 0;
};

template<typename T> inline void freeMemory(T *pMemory)
{
	if (pMemory)
	{
		delete[] pMemory;
		pMemory = nullptr;
	}
}
