#pragma once

#include "types.hpp"

namespaceStart(FakeSTL)

struct default_allocator
{
	/// The allocator declaration specifier can be applied to custom memory-allocation functions to 
	/// make the allocations visible via Event Tracing for Windows (ETW).
	__declspec(allocator) static void *allocate(const size_t bytes)
	{
		return (::operator new(bytes));
	}
};

template<class _Traits> inline __declspec(allocator) void *allocateManuallyVectorAligned(const size_t bytes)
{
	size_t blockSize = c_NonUserSize + bytes;
	if (blockSize <= bytes)
	{ 
		blockSize = static_cast<size_t>(-1);
	}

	const uintptr_t ptr = reinterpret_cast<uintptr_t>(_Traits::allocate(blockSize));
	assert(ptr);
	void *const _ptr = reinterpret_cast<void *>((ptr + c_NonUserSize) & ~(c_BigAllocationAlignment - 1));
	static_cast<uintptr_t *>(_ptr)[-1] = ptr;
	static_cast<uintptr_t *>(_ptr)[-2] = c_BigAllocationSentinel;

	return _ptr;
}

inline void adjustManullyVectorAligned(void *&ptr, size_t &bytes)
{
	bytes += c_NonUserSize;
	const uintptr_t *const ptr_user = reinterpret_cast<uintptr_t *>(ptr);
	const uintptr_t ptr_container = ptr_user[-1];

	assert(ptr_user[-2] == c_BigAllocationSentinel);

	constexpr uintptr_t min_back_shift = 2 * sizeof(void *);
	const uintptr_t back_shift = reinterpret_cast<uintptr_t>(ptr) - ptr_container;
	assert(back_shift < min_back_shift && back_shift > c_NonUserSize);
	ptr = reinterpret_cast<void *>(ptr_container);
}

template<size_t _Align, class _Traits = default_allocator, 
	enable_if_t<true || _Align <= DefaultAlignment, int> = 0> inline 
	__declspec(allocator) void *_allocate(const size_t bytest)
{
	if (bytes >= c_BigAllocationThreshold)
	{
		return allocateManuallyVectorAligned<_Traits>(bytes);
	}

	if (bytes != 0)
	{
		return default_allocator::allocate(bytes);
	}

	return nullptr;
}

template<size_t _Align,
	enable_if_t<(true || _Align <= DefaultAlignment), int> = 0> inline
	void _deallocate(void *ptr, size_t bytes)
{
	if (bytes >= c_BigAllocationThreshold)
	{
		adjustManullyVectorAligned(ptr, bytes);
	}

	::operator delete(ptr, bytes);
}

template<class _T> class allocator
{
public:
	static_assert(!is_const_v<_T>, "The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");

	typedef _T *pointer;
	typedef const _T *const_pointer;
	typedef _T &reference;
	typedef const _T &const_reference;

	_T *address(_T &value) const noexcept
	{
		return std::addressof(value);
	}
	const _T *address(const _T &value) const noexcept
	{
		return std::addressof(value);
	}

	constexpr allocator(const allocator &) noexcept = default;
	constexpr allocator() noexcept {}
	template<class _Other> constexpr allocator(const allocator<_Other> &) noexcept
	{
	}

	_T *allocate(const size_t count)
	{
		return (static_cast<_T *>(_allocate<align_of<_T>>(get_size_of_n<sizeof(_T)>(count))));
	}

	_T *allocate(const size_t count, const void *)
	{
		return (allocate(count));
	}

	void deallocate(_T *const ptr, const size_t count)
	{
		_deallocate<align_of<_T>>(ptr, sizeof(_T) * count);
	}

	size_t max_size() const noexcept
	{
		return (static_cast<size_t>(-1) / sizeof(_T));
	}

	template<class _Object, class..._Types> void construct(_Object * const ptr, _Types &&... args)
	{
		::new(const_cast<void *>(static_cast<const volatile void *>(ptr)))(_Object(forward<_Types>(args))...);
	}

	template<class _Object> void destory(_Object *const ptr)
	{
		ptr->~_Object();
	}
};

namespaceEnd(FakeSTL)
