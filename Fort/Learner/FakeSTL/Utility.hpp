#pragma once

#include "Gear/Gear.h"

namespaceStart(FakeSTL)

struct IteratorBase
{
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
