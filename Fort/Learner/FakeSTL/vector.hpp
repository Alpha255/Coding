#pragma once

#include "Utility.hpp"

///std::vector<int> test;

namespaceStart(FakeSTL)

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

template<class T>
struct VectorValue
{
	using pointer = typename Allocator<T>::pointer;

	pointer First = nullptr;
	pointer Last = nullptr;
	pointer End = nullptr;
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

template<class T>
class VectorAlloc
{
public:
	using pointer = typename VectorValue<T>::pointer;

	VectorAlloc()
		: Pair()
	{
	}

	Allocator<T>& allocator() noexcept
	{
		return Pair.first();
	}

	const Allocator<T>& allocator() const noexcept
	{
		return Pair.first();
	}

	VectorValue<T>& data() noexcept
	{
		return Pair.second();
	}

	const VectorValue<T>& data() const noexcept
	{
		return Pair.second();
	}

	pointer& first() noexcept
	{
		return data().First;
	}

	const pointer& first() const noexcept
	{
		return data().First;
	}

	pointer& last() noexcept
	{
		return data().Last;
	}

	const pointer& last() const noexcept
	{
		return data().Last;
	}

	pointer& end() noexcept
	{
		return data().End;
	}

	const pointer& end() const noexcept
	{
		return data().End;
	}
private:
	CompressedPair<Allocator<T>, VectorValue<T>> Pair;
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
		if (buy(count))
		{
			fill(first(), count, val);
		}
		else
		{
			assert(0);
		}
	}

	template<class Iter>
	Vector(Iter first, Iter last)
	{
		assert(last >= first);
		const size_t count = static_cast<size_t>(std::distance(first, last));
		if (buy(count))
		{
			copy(first, last, this->first());
		}
		else
		{
			assert(0);
		}
	}

	Vector(std::initializer_list<T> list)
	{
		if (buy(list.size()))
		{
			copy(list.begin(), list.end(), first());
		}
		else
		{
			assert(0);
		}
	}

	Vector(const Vector& right)
	{
		if (buy(right.size()))
		{
			copy(right.first(), right.last(), first());
		}
		else
		{
			assert(0);
		}
	}

	Vector(Vector&& right) noexcept
	{
		move_from(right);
	}

	Vector& operator=(Vector&& right)
	{
		return (*this);
	}

	~Vector() noexcept
	{
		if (first() != pointer())
		{
			if (!std::is_pod<T>())
			{
				destroy(first(), last());
			}

			allocator().free(first());

			first() = pointer();
			last() = pointer();
			end() = pointer();
		}
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

	T& operator[](const size_t pos)
	{
		assert(pos < size());
		return first()[pos];
	}

	const T& operator[](const size_t pos) const
	{
		assert(pos < size());
		return first()[pos];
	}

	T& at(const size_t pos)
	{
		assert(pos < size());
		return first()[pos];
	}

	const T& at(const size_t pos) const
	{
		assert(pos < size());
		return first()[pos];
	}

	T& front()
	{
		assert(!empty());
		return *first();
	}

	const T& front() const
	{
		assert(!empty());
		return *first();
	}

	T& back()
	{
		assert(!empty());
		return last()[-1];
	}

	const T& back() const
	{
		assert(!empty());
		return last()[-1];
	}

	T* data() noexcept
	{
		return first();
	}

	const T* data() const noexcept
	{
		return first();
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

		first() = allocator().alloc(count);
		end() = first() + count;
		last() = end();

		return true;
	}

	void initialize(pointer first, size_t count, bool8_t useMemset)
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
				new (first + i) T;
			}
		}
	}

	void fill(pointer first, size_t count, const T& value)
	{
		for (uint32_t i = 0u; i < count; ++i)
		{
			new (first) T(value);
		}
	}

	template<class Iter>
	void copy(Iter first, Iter last, pointer dst)
	{
		const char8_t* const firstCh = const_cast<const char8_t*>(reinterpret_cast<const volatile char8_t*>(first));
		const char8_t* const lastCh = const_cast<const char8_t*>(reinterpret_cast<const volatile char8_t*>(last));
		char8_t* const dstCh = const_cast<char8_t*>(reinterpret_cast<volatile char8_t*>(dst));
		const auto count = static_cast<size_t>(lastCh - firstCh);
		memmove(dstCh, firstCh, count);
	}

	void move_from(Vector&& right) noexcept
	{
		this->first() = right.first();
		this->last() = right.last();
		this->end() = right.end();

		right.first() = pointer();
		right.last() = pointer();
		right.end() = pointer();
	}

	inline void destruct(T& obj) noexcept
	{
		obj.~T();
	}

	template<class Iter>
	void destroy(Iter first, Iter last)
	{
		for (; first != last; ++first)
		{
			destruct(*first);
		}
	}
};

namespaceEnd(FakeSTL)