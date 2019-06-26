#pragma once

#include "Node.h"

template<typename T> class linkedlist
{
public:
	class iterator
	{
	public:
	protected:
	private:
	};

	linkedlist() = default;
	virtual ~linkedlist()
	{
		clear();
	}

	inline bool empty() const
	{
		return m_Root == nullptr;
	}

	inline size_t size() const
	{
		return m_Size;
	}

	inline bool insert();

	inline node<T> remove();

	inline void push_back(T value)
	{
		if (!m_Root)
		{
			m_Root = new node<T>(value);
		}
		else
		{
			node<T> *pNode = m_Root;
			while (pNode)
			{
				pNode = pNode->Next;
			}

			node<T> *pNewNode = new node<T>(value);
			pNode->Next = pNewNode;
		}

		++m_Size;
	}

	void clear()
	{
		node<T> *pNode = m_Root;
		while (pNode)
		{
			node<T> *pNextNode = pNode->Next;
			free(pNode);
			pNode = pNextNode;
		}

		m_Size = 0;
	}
protected:
private:
	node<T> *m_Root = nullptr;
	size_t m_Size = 0U;
};