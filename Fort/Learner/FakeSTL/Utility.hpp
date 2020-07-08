#pragma once

#include "Gear/Gear.h"

namespaceStart(FakeSTL)

struct IteratorBase
{
};

template<class T>
class Allocator
{
public:
	typedef T* pointer;

	inline pointer alloc(size_t count)
	{
		return static_cast<pointer>(std::malloc(sizeof(T) * count));
	}

	inline void free(pointer ptr)
	{
		std::free(ptr);
	}
protected:
private:
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

	T1& first() noexcept
	{
		return *this;
	}

	const T1& first() const noexcept
	{
		return *this;
	}

	T2& second() noexcept
	{
		return Val2;
	}

	const T2& second() const noexcept
	{
		return Val2;
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

	T1& first() noexcept
	{
		return Val1;
	}

	const T1& first() const noexcept
	{
		return Val1;
	}

	T2& second() noexcept
	{
		return Val2;
	}

	const T2& second() const noexcept
	{
		return Val2;
	}
};

//template<class T>
//class VectorConstIterator : public IteratorBase
//{
//public:
//	VectorConstIterator()
//		: Ptr()
//	{
//	}
//
//	VectorConstIterator(T* ptr, const ContainerBase* vector)
//		: Ptr(ptr)
//	{
//		this->adopt(vector);
//	}
//
//	const T& operator*() const
//	{
//		return (*Ptr);
//	}
//
//	T* operator->() const
//	{
//		return Ptr;
//	}
//
//	VectorConstIterator& operator++()
//	{
//		++Ptr;
//		return (*this);
//	}
//
//	VectorConstIterator operator++(int)
//	{
//		VectorConstIterator tmp = *this;
//		++*this;
//		return (tmp);
//	}
//
//	VectorConstIterator& operator--()
//	{
//		--Ptr;
//		return (*this);
//	}
//
//	VectorConstIterator operator--(int)
//	{
//		VectorConstIterator tmp = *this;
//		--*this;
//		return (tmp);
//	}
//
//	void verifyOffset(const size_t offset) const
//	{
//		(void)offset;
//	}
//
//	VectorConstIterator& operator+=(const size_t off)
//	{
//		verifyOffset(off);
//		Ptr += off;
//		return (*this);
//	}
//
//	VectorConstIterator operator+(const size_t off)
//	{
//		VectorConstIterator tmp = *this;
//		return (tmp += off);
//	}
//
//	VectorConstIterator operator-=(const size_t off)
//	{
//		return (*this += -off);
//	}
//
//	VectorConstIterator operator-(const size_t off)
//	{
//		VectorConstIterator tmp = *this;
//		return (tmp -= off);
//	}
//
//	size_t operator-(const VectorConstIterator& right) const
//	{
//		return Ptr - right.Ptr;
//	}
//
//	const T& operator[](const size_t off) const
//	{
//		return (*(*this + off));
//	}
//
//	bool operator==(const VectorConstIterator& right) const
//	{
//		return (Ptr == right.Ptr);
//	}
//
//	bool operator!=(const VectorConstIterator& right) const
//	{
//		return (!(*this == right));
//	}
//
//	bool operator<(const VectorConstIterator& right) const
//	{
//		return (Ptr < right.Ptr);
//	}
//
//	bool operator>(const VectorConstIterator& right) const
//	{
//		return (right < *this);
//	}
//
//	bool operator<=(const VectorConstIterator& right) const
//	{
//		return (!right < *this);
//	}
//
//	bool operator>=(const VectorConstIterator& right) const
//	{
//		return (!(*this < right));
//	}
//
//	T* unwrapped() const
//	{
//		return (Ptr);
//	}
//
//	void seekTo(T* ptr)
//	{
//		Ptr = std::_Const_cast(ptr);
//	}
//
//	T* Ptr;
//};
//
//template<class T>
//class VectorIterator : public VectorConstIterator<T>
//{
//public:
//	using Base = VectorConstIterator<T>;
//
//	VectorIterator()
//	{
//	}
//
//	VectorIterator(T* ptr, const ContainerBase* vector)
//		: Base(ptr, vector)
//	{
//	}
//
//	T& operator*() const
//	{
//		return (const_cast<T&>(Base::operator*()));
//	}
//
//	T* operator->() const
//	{
//		return (std::_Const_cast(Base::operator->()));
//	}
//
//	VectorIterator& operator++()
//	{
//		++*(Base*)this;
//		return (*this);
//	}
//
//	VectorIterator operator++(int)
//	{
//		VectorIterator tmp = *this;
//		++*this;
//		return (tmp);
//	}
//
//	VectorIterator& operator--()
//	{
//		--*(Base*)this;
//		return (*this);
//	}
//
//	VectorIterator operator--(int)
//	{
//		VectorIterator tmp = *this;
//		--*this;
//		return (tmp);
//	}
//
//	VectorIterator& operator+=(const size_t off)
//	{
//		*(Base*)this += off;
//		return (*this);
//	}
//
//	VectorIterator operator+(const size_t off) const
//	{
//		VectorIterator tmp = *this;
//		return (tmp += off);
//	}
//
//	VectorIterator& operator-=(const size_t off)
//	{
//		return (*this += -off);
//	}
//
//	VectorIterator operator-(const size_t off) const
//	{
//		VectorIterator tmp = *this;
//		return (tmp -= off);
//	}
//
//	size_t operator-(const Base& right) const
//	{
//		return (*(Base*)this - right);
//	}
//
//	T& operator[](const size_t off) const
//	{
//		return (*(*this + off));
//	}
//
//	T* unwrapped() const
//	{
//		return (this->Ptr);
//	}
//};

namespaceEnd(FakeSTL)
