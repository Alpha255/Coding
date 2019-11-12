#pragma once

#include "Node.h"

namespaceStart(fort)

template<typename T> class circlelinkedlist
{
public:
	typedef node<T> *iterator;
	typedef const iterator const_iterator;

	circlelinkedlist() = default;
	virtual ~circlelinkedlist()
	{
		clear();
	}

	inline bool empty() const
	{
		return m_Root->Next == m_Root;
	}

	inline size_t size() const
	{
		return m_Size;
	}

	void insert(uint32_t pos, const T &value)
	{
		assert(pos < m_Size && m_Size != 0U);

		uint32_t counter = 0U;
		iterator it = m_Root;
		while (counter < pos)
		{
			it = it->Next;
			++counter;
		}

		node<T> *pNewNode = new node<T>(value);
		pNewNode->Next = it->Next;
		it->Next = pNewNode;

		++m_Size;
	}

	T remove(uint32_t pos)
	{
		assert(pos < m_Size);

		uint32_t counter = 0U;
		iterator it = m_Root;
		iterator itPrev = it;
		while (counter < pos)
		{
			itPrev = it;
			it = it->Next;
			++counter;
		}

		T value = it->Data;

		if (it == m_Root)
		{
			m_Root = m_Root->Next;
			itPrev->Next = m_Root;
		}

		itPrev->Next = it->Next;
		free(it);
		--m_Size;

		return value;
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

		m_Root = nullptr;
		m_Size = 0;
	}
protected:
private:
	iterator m_Root = nullptr;
	size_t m_Size = 0U;
};

namespaceEnd(fort)