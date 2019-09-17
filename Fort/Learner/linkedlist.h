#pragma once

#include "Node.h"

template<typename T> class linkedlist
{
public:
	typedef node<T> *iterator;
	typedef const iterator const_iterator;

	linkedlist() = default;
	virtual ~linkedlist()
	{
		clear();
	}

	inline iterator begin()
	{
		return m_Begin;
	}

	inline const_iterator begin() const
	{
		return m_Begin;
	}

	inline iterator end()
	{
		return m_End;
	}

	inline const_iterator end() const
	{
		return m_End;
	}

	inline bool empty() const
	{
		return m_Begin == nullptr;
	}

	inline size_t size() const
	{
		return m_Size;
	}

	void insert(uint32_t pos, const T &value)
	{
		assert(pos < m_Size && m_Size != 0U);

		uint32_t counter = 0U;
		iterator it = m_Begin;
		while (counter < pos)
		{
			it = it->Next;
			++counter;
		}

		node<T> *pNewNode = new node<T>(value);
		pNewNode->Next = it->Next;
		it->Next = pNewNode;

		if (it == m_End)
		{
			m_End = pNewNode;
		}

		++m_Size;
	}

	T remove(uint32_t pos)
	{
		assert(pos < m_Size);

		uint32_t counter = 0U;
		iterator it = m_Begin;
		iterator itPrev = it;
		while (counter < pos)
		{
			itPrev = it;
			it = it->Next;
			++counter;
		}

		T value = it->Data;

		if (it == m_Begin)
		{
			m_Begin = m_Begin->Next;
		}
		if (it == m_End)
		{
			m_End = itPrev;
		}

		itPrev->Next = it->Next;
		free(it);
		--m_Size;

		return value;
	}

	inline void push_back(const T &value)
	{
		if (!m_Begin)
		{
			m_Begin = new node<T>(value);
			m_End = m_Begin;
		}
		else
		{
			node<T> *pNewNode = new node<T>(value);
			m_End->Next = pNewNode;
			m_End = pNewNode;
		}

		++m_Size;
	}

	void clear()
	{
		node<T> *pNode = m_Begin;
		while (pNode)
		{
			node<T> *pNextNode = pNode->Next;
			free(pNode);
			pNode = pNextNode;
		}

		m_Begin = nullptr;
		m_End = nullptr;
		m_Size = 0;
	}
protected:
private:
	iterator m_Begin = nullptr;
	iterator m_End = nullptr;
	size_t m_Size = 0U;
};