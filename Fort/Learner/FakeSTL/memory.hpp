#pragma once

#define DefaultAlignment 16ull

template<class T> struct RemoveReference
{
	using Type = T;
};
template<class T> struct RemoveReference<T &>
{
	using Type = T;
};
template<class T> struct RemoveReference<T &&>
{
	using Type = T;
};

template<class T> constexpr T &&Forward(RemoveReference<T> &arg) noexcept
{
	return static_cast<T &&>(arg);
}

template<class T> constexpr T &&Forward(RemoveReference<T> &&arg) noexcept
{
	return static_cast<T &&>(arg);
}

template<class T> constexpr const T &Min(const T &left, const T &right)
{
	return right < left ? right : left;
}

template<class T> constexpr const T &Max(const T &left, const T &right)
{
	return left < right ? right : left;
}

template<class T> constexpr size_t Align = Max(alignof(T), static_cast<size_t>(DefaultAlignment));

template<class T> class Allocator
{
public:
	const_cast Allocator() noexcept {}
	constexpr Allocator(const Allocator &) noexcept = default;

	inline void Deallocate(T *const ptr, const size_t count)
	{
		/// Adjust???
		::operator delete(ptr, sizeof(T) * count;
	}

	T *Allocate(const size_t count)
	{
		constexpr size_t maxCount = static_cast<size_t>(-1) / sizeof(T);
		if (count < maxCount)
		{
			return ::operator new(sizeof(T) * count);
		}
		return nullptr;
	}

	template<class T, class... Args>
	void construct(T *const ptr, Args &&... args)
	{
		::new(const_cast<void *>(static_cast<const volatile void *>(ptr)))
			T(Forward<Args>(args)...);
	}

	template<class T>
	void destory(T *const ptr)
	{
		ptr->~T();
	}
protected:
private:
};