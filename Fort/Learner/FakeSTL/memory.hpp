#pragma once

#include "types.hpp"

namespaceStart(FakeSTL)

#define defaultAlignment 16ull

constexpr size_t c_BigAllocationThreshold = 4096;
constexpr size_t c_BigAllocationAlignment = 32;
constexpr size_t c_BigAllocationSentinel = 0xFAFAFAFAFAFAFAFAULL;
constexpr size_t c_NonUserSize = 2 * sizeof(void *) + c_BigAllocationAlignment - 1;

template<class _T> constexpr const _T &min_value(const _T &left, const _T &right) noexcept
{
	return (right < left ? right : left);
}
template<class _T> constexpr const _T &max_value(const _T &left, const _T &right) noexcept
{
	return (left < right ? right : left);
}
template<class _T> constexpr size_t align_of = max_value(alignof(_T), static_cast<size_t>(defaultAlignment));

template<class _T> class allocator;
template<class _Alloc, class = void> struct is_default_allocator : false_type {};
template<class _T> struct is_default_allocator<allocator<_T>, void> : true_type {};

template<class _Alloc, class _Size_type, class _Const_void_pointer, class = void> struct has_allocate_hint : false_type {};

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
	enable_if_t<true || _Align <= defaultAlignment, int> = 0> inline
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
	enable_if_t<(true || _Align <= defaultAlignment), int> = 0> inline
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

template<class _Alloc> struct allocator_traits;

template<class _Alloc> struct normal_allocator_traits
{
	using allocator_type = _Alloc;
	using value_type = typename _Alloc::value_type;
	using pointer = typename get_pointer_type<_Alloc>::type;
	using const_pointer = typename get_const_pointer_type<_Alloc>::type;
	using void_pointer = typename get_void_pointer_type<_Alloc>::type;
	using const_void_pointer = typename get_const_void_pointer_type<_Alloc>::type;

	using size_type = typename get_size_type<_Alloc>::type;
	using difference_type = typename get_difference_type<_Alloc>::type;

	template<class _Other> using rebind_alloc = typename get_rebind_type<_Alloc, _Other>::type;
	template<class _Other> using rebind_traits = allocator_traits<rebind_alloc<_Other>>;

	static __declspec(allocator) pointer allocate(_Alloc &alloc, const size_type count)
	{
		return alloc.allocate(count);
	}

	static __declspec(allocator) pointer allocate(_Alloc &alloc, const size_type count, const const_void_pointer hint)
	{
		return allocate1(alloc, count, hint, has_allocate_hint<_Alloc, size_type, const_void_pointer>{}));
	}

	static __declspec(allocator) pointer allocate1(_Alloc &alloc, const size_type count, const_void_pointer hint, true_type)
	{
		return alloc.allocate(count, hint);
	}

	static __declspec(allocator) pointer allocate1(_Alloc &alloc, const size_type count, const_void_pointer, false_type)
	{
		return alloc.allocate(count);
	}
};

template<class _Alloc> struct default_allocator_traits
{
	using allocator_type = _Alloc;
	using value_type = typename _Alloc::value_type;
	using pointer = value_type *;
	using const_pointer = const value_type *;
	using void_pointer = void *;
	using const_void_pointer = const void *;

	template<class _Other> using rebind_alloc = allocator<_Alloc>;

	template<class _Other> using rebind_traits = allocator_traits<allocator<_Other>>;

	static __declspec(allocator) pointer allocate(_Alloc &, const size_t count)
	{
		return (static_cast<pointer>(_allocate<align_of<value_type>>(get_size_of_n<sizeof(value_type)>(count))));
	}

	static __declspec(allocator) pointer allocate(_Alloc &, const size_t count, const_void_pointer)
	{
		return (static_cast<pointer>(_allocate<align_of<value_type>>(get_size_of_n<sizeof(value_type)>(count))));
	}

	static void deallocate(_Alloc &, const pointer ptr, const size_t count)
	{
		_deallocate<align_of<value_type>>(ptr, sizeof(value_type) * count);
	}

	template<class _Object, class... _Types> static void construct(_Alloc &, _Object * const ptr, _Types &&... args)
	{
		::new(const_cast<void *>(static_cast<const volatile void *>(ptr)))_Object(forward<_Types>(args)...);
	}

	template<class _Object> static void destroy(_Alloc &, _Object * const ptr)
	{
		ptr->~_Object();
	}

	static size_t max_size(const _Alloc &) noexcept
	{
		return (static_cast<size_t>(-1) / sizeof(value_type));
	}

	static _Alloc select_on_container_copy_construction(const _Alloc & alloc)
	{
		return (alloc);
	}
};

template<class _Alloc> struct allocator_traits
	: conditional_t<is_default_allocator<_Alloc>::value,
	default_allocator_traits<_Alloc>, normal_allocator_traits<_Alloc>>
{
};

namespaceEnd(FakeSTL)
