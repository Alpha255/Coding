#pragma once

#include "node.h"

template<typename T> class linearlist
{
public:
	inline linearlist(const size_t maxSize)
		: m_MaxSize(maxSize)
	{
		m_Elems = new T[maxSize]();
	}

	inline linearlist(const linearlist &other)
		: m_Size(other.m_Size)
		, m_MaxSize(other.m_MaxSize)
	{
		if (other.m_Elems != m_Elems)
		{
			memcpy(m_Elems, other->m_Elems, sizeof(T) * other.m_MaxSize);
		}
	}

	inline linearlist(linearlist &&other) noexcept
		: m_Size(other.m_Size)
		, m_MaxSize(other.m_MaxSize)
	{
		if (other.m_Elems != m_Elems)
		{
			m_Elems = other.m_Elems;
			other.m_Elems = nullptr;
			other.m_Size = 0U;
			other.m_MaxSize = 0U;
		}
	}

	inline void operator= (const linearlist &other)
	{
		if (other.m_Elems != this->m_Elems)
		{
			m_Size = other.m_Size;
			m_MaxSize = other.m_MaxSize;
			memcpy(m_Elems, other->m_Elems, sizeof(T) * other.m_MaxSize);
		}
	}

	inline virtual ~linearlist()
	{
		freeMemory(m_Elems);
	}

	inline size_t MaxSize() const
	{
		return m_MaxSize;
	}

	inline size_t Size() const
	{
		return m_Size;
	}

	inline bool Empty() const
	{
		return m_Size == 0U;
	}

	inline bool Full() const
	{
		return m_Size == m_MaxSize;
	}

	inline void Clear()
	{
		m_Size = 0U;
	}

	inline T Get(size_t index) const
	{
		assert(index < m_Size);
		return m_Elems[index];
	}

	inline void Set(size_t index, const T &elem)
	{
		assert(index < m_MaxSize);
		m_Elems[index] = elem;
		m_Size = index >= m_Size ? 1U + m_Size : m_Size;
	}

	T Delete(size_t index);
	void Insert(size_t index, const T &elem);
protected:
private:
	size_t m_Size = 0U;
	size_t m_MaxSize = 0U;
	T* m_Elems;
};

template<typename T> T linearlist<T>::Delete(size_t index)
{
	assert(index < m_Size);

	T del = m_Elems[index];
	memcpy(m_Elems + index, m_Elems + index + 1, sizeof(T) * (m_MaxSize - index - 1U));
	--m_Size;

	return del;
}

template<typename T> void linearlist<T>::Insert(size_t index, const T &elem)
{
	assert(index < m_MaxSize && m_Size < m_MaxSize);
	memcpy(m_Elems + index + 1U, m_Elems + index, sizeof(T) * (m_MaxSize - index - 1U));
	m_Elems[index] = elem;
	++m_Size;
}
