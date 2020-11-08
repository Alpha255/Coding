#pragma once

#include "Gear/Gear.h"

NAMESPACE_START(FORT)

template<class T>
struct TreeNode
{
	T* Prev = nullptr;
	T* Next = nullptr;
};

template<class T>
class Tree
{

};

template<class T>
class BinaryTree : public Tree<T>
{

};

template<class T>
class RedBlackTree : public BinaryTree<T>
{
};

NAMESPACE_END(FORT)