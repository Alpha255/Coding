#pragma once

#include "Node.h"

template<typename T> class linkedlist
{
public:
	linkedlist() = default;
	virtual ~linkedlist() = default;

	inline bool empty() const
	{
		return m_Root == nullptr;
	}

	inline size_t size() const
	{
		return m_Size;
	}

	inline bool insert();

	inline node remove();
protected:
private:
	node *m_Root = nullptr;
	size_t m_Size = 0U;
};