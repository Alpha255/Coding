#pragma once

#include "bintree.h"

namespaceStart(fort)

template <typename T> struct rbTreeNode : public treenode<T>
{
	enum eColor
	{
		eRed,
		eBlack
	};

	eColor Color = eBlack;
};

template <typename T> class rbTree : public BinTree<T>
{
public:
	void Insert(const T &value) override
	{

	}
protected:
	void RotateRight(rbTreeNode<T> *pPivotNode)
	{
	}

	void RotateLeft(rbTreeNode<T> *pPivotNode)
	{
		rbTreeNode<T> *pNode = pPivotNode->RightChild;
		pPivotNode->RightChild = pNode->LeftChild;

		if (pNode->LeftChild)
		{
			pNode->LeftChild->Parent = pPivotNode;
		}

		/// Root Node
		if (!pPivotNode->Parent)
		{
			m_Root = pNode;
		}
		else if (pPivotNode == pPivotNode->Parent->LeftChild)
		{
			pPivotNode->Parent->LeftChild = pNode;
		}
		else
		{
			pPivotNode->Parent->RightChild = pNode;
		}

		pNode->LeftChild = pPivotNode;
		pPivotNode->Parent = pNode;
	}
private:
};

namespaceEnd(fort)