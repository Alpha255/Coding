#pragma once

#include "Utility.hpp"

namespaceStart(FakeSTL)
// circular queue of pointers to blocks

template<class T>
struct DequeValue
{
	using pointer = typename Allocator<T>::pointer;

	pointer Map = nullptr;
	size_t MapSize = 0u;
	size_t Off = 0u;
	size_t Size = 0u;
};

template<class T>
class DequeAlloc
{
public:
	using pointer = typename DequeValue<T>::pointer;
private:
	CompressedPair<Allocator<T>, DequeAlloc<T>> Pair;
};

template<class T> 
class Deque : public DequeAlloc<T>
{
public:
	void push_front(T&& val)
	{
	}

	void push_back(T&& val)
	{
	}
};

namespaceEnd(FakeSTL)
