#pragma once

#include <stdint.h>

template<typename T> struct Node
{
	T *Next = nullptr;
	T *Prev = nullptr;
	T Data;
};
