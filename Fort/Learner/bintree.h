#pragma once

#include "node.h"
#include <vector>

namespaceStart(fort)

std::vector<int> test;

template <typename T> class BinTree
{
public:
	virtual ~BinTree()
	{

	}

	virtual void Insert(const T &value)
	{
		assert(0);

		if (!m_Root)
		{
			m_Root = new treenode<T>(value, nullptr, nullptr);
			m_Root->Level = 0U;
			return;
		}
	}

	inline void TraverseByPreOrder()
	{
		std::cout << "Traverse by preOrder: ";

		TraverseByPreOrderImpl(m_Root);

		std::cout << std::endl;
	}

	inline void TraverseByPreOrder_No()
	{
		treenode<T> *pCurrent = m_Root;
		std::stack<treenode<T> *> nodeStack;

		while (pCurrent)
		{
			PrintImpl(pCurrent);

			nodeStack.push(pCurrent);

			if (pCurrent->LeftChild)
			{
				pCurrent = pCurrent->LeftChild;
			}
			else if (!nodeStack.empty())
			{
				while (!nodeStack.empty())
				{
					pCurrent = nodeStack.top();
					nodeStack.pop();
					pCurrent = pCurrent->RightChild;
					if (pCurrent)
					{
						break;
					}
				}
			}
			else
			{
				pCurrent = nullptr;
			}
		}
	}

	inline void TraverseByInOrder()
	{
		std::cout << "Traverse by inOrder: ";

		TraverseByInOrderImpl(m_Root);

		std::cout << std::endl;
	}

	inline void TraverseByPostOrder()
	{
		std::cout << "Traverse by postOrder: ";

		TraverseByPostOrderImpl(m_Root);

		std::cout << std::endl;
	}
protected:
	void TraverseByPreOrderImpl(const treenode<T> *pNode)
	{
		if (pNode)
		{
			PrintImpl(pNode);
			TraverseByPreOrderImpl(pNode->LeftChild);
			TraverseByPreOrderImpl(pNode->RightChild);
		}
	}

	void TraverseByInOrderImpl(const treenode<T> *pNode)
	{
		if (pNode)
		{
			TraverseByInOrderImpl(pNode->LeftChild);
			PrintImpl(pNode);
			TraverseByInOrderImpl(pNode->RightChild);
		}
	}

	void TraverseByPostOrderImpl(const treenode<T> *pNode)
	{
		if (pNode)
		{
			TraverseByPostOrderImpl(pNode->LeftChild);
			TraverseByPostOrderImpl(pNode->RightChild);
			PrintImpl(pNode);
		}
	}

	void PrintImpl(const treenode<T> *pNode)
	{
		if (pNode)
		{
			std::cout << pNode->Data << "=>";
		}
	}
private:
	treenode<T> *m_Root = nullptr;
};

namespaceEnd(fort)
