#pragma once

#include "gear/gear.h"

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

template <typename T> struct treenode
{
	inline treenode() = default;
	inline treenode(const T &value, treenode * pParent = nullptr, treenode *leftChild = nullptr, treenode *rightChild = nullptr)
		: Data(value)
		, Parent(pParent)
		, LeftChild(leftChild)
		, RightChild(rightChild)
	{
	}

	treenode *Parent = nullptr;
	treenode *LeftChild = nullptr;
	treenode *RightChild = nullptr;
	uint32_t Level = UINT32_MAX;

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
