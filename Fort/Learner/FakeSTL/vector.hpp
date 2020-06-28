#pragma once

#include "Utility.hpp"

///std::vector<int> test;

namespaceStart(FakeSTL)

template<class T>
class VectorValue : public ContainerBase
{
public:
	VectorValue()
		: First()
		, Last()
		, End()
	{
	}

	T* First;
	T* Last;
	T* End;
};

template<class T>
class VectorConstIterator : public IteratorBase
{
public:
	VectorConstIterator()
		: Ptr()
	{
	}

	VectorConstIterator(T* ptr, const ContainerBase* vector)
		: Ptr(ptr)
	{
		this->adopt(vector);
	}

	const T& operator*() const
	{
		return (*Ptr);
	}

	T* operator->() const
	{
		return Ptr;
	}

	VectorConstIterator& operator++()
	{
		++Ptr;
		return (*this);
	}

	VectorConstIterator operator++(int)
	{
		VectorConstIterator tmp = *this;
		++*this;
		return (tmp);
	}

	VectorConstIterator& operator--()
	{
		--Ptr;
		return (*this);
	}

	VectorConstIterator operator--(int)
	{
		VectorConstIterator tmp = *this;
		--*this;
		return (tmp);
	}

	void verifyOffset(const size_t offset) const
	{
		(void)offset;
	}

	VectorConstIterator& operator+=(const size_t off)
	{
		verifyOffset(off);
		Ptr += off;
		return (*this);
	}

	VectorConstIterator operator+(const size_t off)
	{
		VectorConstIterator tmp = *this;
		return (tmp += off);
	}

	VectorConstIterator operator-=(const size_t off)
	{
		return (*this += -off);
	}

	VectorConstIterator operator-(const size_t off)
	{
		VectorConstIterator tmp = *this;
		return (tmp -= off);
	}

	size_t operator-(const VectorConstIterator& right) const
	{
		return Ptr - right.Ptr;
	}

	const T& operator[](const size_t off) const
	{
		return (*(*this + off));
	}

	bool operator==(const VectorConstIterator& right) const
	{
		return (Ptr == right.Ptr);
	}

	bool operator!=(const VectorConstIterator& right) const
	{
		return (!(*this == right));
	}

	bool operator<(const VectorConstIterator& right) const
	{
		return (Ptr < right.Ptr);
	}

	bool operator>(const VectorConstIterator& right) const
	{
		return (right < *this);
	}

	bool operator<=(const VectorConstIterator& right) const
	{
		return (!right < *this);
	}

	bool operator>=(const VectorConstIterator& right) const
	{
		return (!(*this < right));
	}

	T* unwrapped() const
	{
		return (Ptr);
	}

	void seekTo(T* ptr)
	{
		Ptr = std::_Const_cast(ptr);
	}

	T* Ptr;
};

template<class T>
class VectorIterator : public VectorConstIterator<T>
{
public:
	using Base = VectorConstIterator<T>;

	VectorIterator()
	{
	}

	VectorIterator(T* ptr, const ContainerBase* vector)
		: Base(ptr, vector)
	{
	}

	T& operator*() const
	{
		return (const_cast<T&>(Base::operator*()));
	}

	T* operator->() const
	{
		return (std::_Const_cast(Base::operator->()));
	}

	VectorIterator& operator++()
	{
		++*(Base*)this;
		return (*this);
	}

	VectorIterator operator++(int)
	{
		VectorIterator tmp = *this;
		++*this;
		return (tmp);
	}

	VectorIterator& operator--()
	{
		--*(Base*)this;
		return (*this);
	}

	VectorIterator operator--(int)
	{
		VectorIterator tmp = *this;
		--*this;
		return (tmp);
	}

	VectorIterator& operator+=(const size_t off)
	{
		*(Base*)this += off;
		return (*this);
	}

	VectorIterator operator+(const size_t off) const
	{
		VectorIterator tmp = *this;
		return (tmp += off);
	}

	VectorIterator& operator-=(const size_t off)
	{
		return (*this += -off);
	}

	VectorIterator operator-(const size_t off) const
	{
		VectorIterator tmp = *this;
		return (tmp -= off);
	}

	size_t operator-(const Base& right) const
	{
		return (*(Base*)this - right);
	}

	T& operator[](const size_t off) const
	{
		return (*(*this + off));
	}

	T* unwrapped() const
	{
		return (this->Ptr);
	}
};

template<class T1, class T2, 
	bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
	class CompressedPair final : private T1
{
private:
	T2 Val2;

	using Base = T1;
public:
	template<class... Other2> constexpr explicit CompressedPair(Other2&&... val2)
		: T1()
		, Val2(std::forward<Other2>(val2)...)
	{
	}

	template<class Other1, class... Other2>
	CompressedPair(Other1&& val1, Other2&&... val2)
		: T1(std::forward<Other1>(val1))
		, Val2(std::forward<Other2>(val2)...)
	{
	}

	T1& get_first() noexcept
	{
		return (*this);
	}

	const T1& get_first() const noexcept
	{
		return (*this);
	}

	T2& get_second() noexcept
	{
		return (Val2);
	}

	const T2& get_second() const noexcept
	{
		return (Val2);
	}
};

template<class T1, class T2>
class CompressedPair<T1, T2, false> final
{
private:
	T1 Val1;
	T2 Val2;
public:
	template<class... Other2>
	constexpr explicit CompressedPair(Other2&&... val2)
		: Val1()
		, Val2(std::forward<Other2>(val2)...)
	{
	}

	template<class Other1, class... Other2>
	CompressedPair(Other1&& val, Other2&&... val2)
		: Val1(std::forward<Other1>(val1))
		, Val2(std::forward<Other2>(val2)...)
	{
	}

	T1& get_first() noexcept
	{
		return (Val1);
	}

	const T1& get_first() const noexcept
	{
		return (Val1);
	}

	T2& get_second() noexcept
	{
		return (Val2);
	}

	const T2& get_second() const noexcept
	{
		return (Val2);
	}
};

template<class T>
class allocator
{
};

template<class T>
class VectorAlloc
{
public:
	VectorAlloc()
		: Pair()
	{
	}

	VectorValue<T>& get_data() noexcept
	{
		return Pair.get_second();
	}

	const VectorValue<T>& get_data() const noexcept
	{
		return Pair.get_second();
	}

	T*& first() noexcept
	{
		return get_data().First;
	}

	const T*& first() const noexcept
	{
		return get_data().First;
	}

	T*& last() noexcept
	{
		return get_data().Last;
	}

	const T*& last() const noexcept
	{
		return get_data().Last;
	}

	T*& end() noexcept
	{
		return get_data().End;
	}

	const T*& end() const noexcept
	{
		return get_data().End;
	}
private:
	CompressedPair<allocator<T>, VectorValue<T>> Pair;
};

template<class T>
class Vector : public VectorAlloc<T>
{
public:
	Vector()
	{
	}

	explicit Vector(const size_t count)
	{
		if (buy(count))
		{
			initialize(first(), count, std::is_pod<T>());
		}
		else
		{
			assert(0);
		}
	}

	Vector(const size_t count, const T& val)
	{
	}

	template<class Iter>
	Vector(Iter first, Iter last)
	{
	}

	Vector(std::initializer_list<T> list)
	{
	}

	Vector(const Vector& right)
	{
	}

	Vector(Vector&& right) noexcept
	{
	}

	Vector& operator=(Vector&& right)
	{
		return (*this);
	}

	~Vector() noexcept
	{
	}

	template<class... Value>
	decltype(auto) emplace_back(Value&&... val)
	{
	}

	void push_back(const T& val)
	{
	}

	void push_back(T&& val)
	{
	}

	size_t max_size() const noexcept
	{
		return std::min(static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()), static_cast<size_t>(-1) / sizeof(T));
	}

	bool8_t empty() const noexcept
	{
		return first() == last();
	}

	size_t size() const noexcept
	{
		return static_cast<size_t>(last() - first());
	}

	size_t capacity() const noexcept
	{
		return static_cast<size_t>(end() - first());
	}
protected:
private:
	bool8_t buy(const size_t count)
	{
		first() = last() = end() = nullptr;

		if (count == 0)
		{
			return false;
		}
		assert(count <= max_size());

		first() = new T[count]();
		last() = first();
		end() = first() + count;

		return true;
	}

	void initialize(T* first, size_t count, bool8_t useMemset)
	{
		if (useMemset)
		{
			char8_t* const firstCh = reinterpret_cast<char8_t*>(first);
			char8_t* const lastCh = reinterpret_cast<char8_t*>(first + count);
			memset(first, 0, static_cast<size_t>(lastCh - firstCh));
		}
		else
		{
			for (uint32_t i = 0u; i < count; ++i)
			{
				new (first) T;
			}
		}
	}
};

namespaceEnd(FakeSTL)