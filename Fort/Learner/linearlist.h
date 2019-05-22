#pragma once

#include "node.h"

template<typename T> class linearlist
{
public:
	inline linearlist(size_t maxSize)
		: m_MaxSize(maxSize)
	{
		m_Data.reset(new T[maxSize]());
	}

	inline linearlist(const linearlist &other)
		: m_Size(other.m_Size)
		, m_MaxSize(other.m_MaxSize)
	{
		if (other.m_Data != m_Data)
		{
			memcpy(m_Data.get(), other.m_Data.get(), sizeof(T) * other.m_MaxSize);
		}
	}

	inline linearlist(linearlist &&other) noexcept
		: m_Size(other.m_Size)
		, m_MaxSize(other.m_MaxSize)
	{
		if (other.m_Data != m_Data)
		{
			m_Data.reset(other.m_Data.release());
			other.m_Size = 0U;
			other.m_MaxSize = 0U;
		}
	}

	inline void operator= (const linearlist &other)
	{
		if (other.m_Data != m_Data)
		{
			m_Size = other.m_Size;
			m_MaxSize = other.m_MaxSize;
			memcpy(m_Data.get(), other->m_Data.get(), sizeof(T) * other.m_MaxSize);
		}
	}

	inline virtual ~linearlist() = default;

	inline size_t maxSize() const
	{
		return m_MaxSize;
	}

	inline size_t size() const
	{
		return m_Size;
	}

	inline bool empty() const
	{
		return m_Size == 0U;
	}

	inline bool full() const
	{
		return m_Size == m_MaxSize;
	}

	inline void clear()
	{
		m_Size = 0U;
	}

	inline T get(size_t index) const
	{
		assert(index < m_Size);
		return m_Data[index];
	}

	inline void set(size_t index, const T &elem)
	{
		assert(index < m_Size);
		m_Data.get()[index] = elem;
	}

	inline T remove(size_t index)
	{
		assert(index < m_Size);

		T del = m_Data.get()[index];
		memcpy(m_Data.get() + index, m_Data.get() + index + 1U, sizeof(T) * (m_MaxSize - index - 1U));
		--m_Size;

		return del;
	}

	inline bool insert(size_t index, const T &elem)
	{
		assert(index < m_Size + 1U);
		
		if (full())
		{
			return false;
		}

		if (index < m_Size)
		{
			memcpy(m_Data.get() + index + 1U, m_Data.get() + index, sizeof(T) * (m_MaxSize - index - 1U));
		}

		m_Data.get()[index] = elem;
		++m_Size;

		return true;
	}
protected:
private:
	size_t m_Size = 0U;
	size_t m_MaxSize = 0U;
	std::unique_ptr<T> m_Data = nullptr;
};
