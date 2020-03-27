#pragma once

#include "types.hpp"

namespaceStart(FakeSTL)

#define DefaultAlignment 16ull

constexpr size_t c_BigAllocationThreshold = 4096;
constexpr size_t c_BigAllocationAlignment = 32;
constexpr size_t c_BigAllocationSentinel = 0xFAFAFAFAFAFAFAFAULL;
constexpr size_t c_NonUserSize = 2 * sizeof(void *) + c_BigAllocationAlignment - 1;

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

template<class _T> class allocator
{
public:
	static_assert(!is_const_v<_T>);

	typedef _T *pointer;
	typedef const _T *const_pointer;
	typedef _T &reference;
	typedef const _T &reference;

	constexpr allocator(const allocator &) noexcept = default;
	template<class _Other> constexpr allocator(const allocator<_Other> &) noexcept
	{
	}


};

namespaceEnd(FakeSTL)
