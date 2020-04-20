#pragma once

#include "memory.hpp"
#include "initializer_list.hpp"
#include <assert.h>

template<class T> 
class Vector_Const_Iterator
{
public:
	T *m_Ptr = nullptr;
};

template<class T>
class Vector_Iterator : public Vector_Const_Iterator<T>
{
};

template<class T, class Alloc = Allocator<T>> 
class Vector
{
public:
	Vector() {}

	explicit Vector(const size_t count)
	{
		if (Buy(count))
		{
		}
	}

	Vector(const size_t count, const T &value)
	{
		if (Buy(count))
		{
		}
	}

	Vector(const Vector &right)
	{

	}

	Vector(InitializerList<T> iList)
	{
	}

	Vector(Vector &&right) noexcept
	{
	}

	template<class Iter>
	Vector(Iter first, Iter last)
	{
	}

	Vector &operator=(Vector &&right)
	{
		return (*this);
	}

	Vector &operator=(const Vector &right)
	{
		return (*this);
	}

	Vector &operator=(InitializerList<T> iList)
	{
		return (*this);
	}

	~Vector() noexcept
	{
	}

	template<class... Args>
	decltype(auto) EmplaceBack(Args &&... args)
	{

	}

	void PushBack(const T &val)
	{
		EmplaceBack(val);
	}

	void PushBack(T &&val)
	{
		EmplaceBack(Move(val));
	}

	template<class... Args>
	Vector_Iterator<T> Emplace(Vector_Const_Iterator<T> where, Args&&... args)
	{
	}

	Vector_Iterator<T> Insert(Vector_Const_Iterator<T> where, const T &val)
	{
		return Emplace(where, val);
	}

	Vector_Iterator<T> Insert(Vector_Const_Iterator<T> where, T &&val)
	{
		return Emplace(where, Move(val));
	}

	Vector_Iterator<T> Insert(Vector_Const_Iterator<T> where, const size_t count, const T& val)
	{
	}

	template<class Iter>
	Vector_Iterator<T> Insert(Vector_Const_Iterator<T> where, Iter first, Iter last)
	{
	}

	Vector_Iterator<T> Insert(Vector_Const_Iterator<T> where, InitializerList<T> iList)
	{
		return Insert(where, iList.Begin(), iList.End());
	}

	void Assign(const size_t size, const T& val)
	{
	}

	template<class Iter>
	void Assign(Iter first, Iter last)
	{
	}

	void Assign(InitializerList<T> iList)
	{
	}

	void Resize(const size_t size)
	{
	}

	void Resize(const size_t size, const T &val)
	{
	}

	size_t MaxSize() const noexcept
	{
		return Min(static_cast<size_t>((std::numeric_limits<ptrdiff_t>::max)()),
			Alloc::MaxSize());
	}
protected:
	bool Buy(const size_t capacity)
	{
		if (capacity == 0)
		{
			return false;
		}

		assert(capacity <= MaxSize());

		m_First = m_Alloc.Allocate(capacity);
		m_Last = m_First;
		m_End = m_First + capacity;

		return true;
	}

	T *Construct_N(T *dst, const size_t count)
	{

	}
private:
	T *m_First = nullptr;
	T *m_Last = nullptr;
	T *m_End = nullptr;
	Alloc m_Alloc;
};
