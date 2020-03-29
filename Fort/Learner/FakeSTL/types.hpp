#pragma once

#include "Gear/gear.h"

namespaceStart(FakeSTL)

#define DefaultAlignment 16ull

constexpr size_t c_BigAllocationThreshold = 4096;
constexpr size_t c_BigAllocationAlignment = 32;
constexpr size_t c_BigAllocationSentinel = 0xFAFAFAFAFAFAFAFAULL;
constexpr size_t c_NonUserSize = 2 * sizeof(void *) + c_BigAllocationAlignment - 1;

template <class _T, _T _Val> struct intergral_constant
{
	static constexpr _T value = _Val;
	constexpr operator _T() const noexcept
	{
		return (value);
	}
	constexpr _T operator()() const noexcept
	{
		return (value);
	}
};

template<bool _Val> using bool_constant = intergral_constant<bool, _Val>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template<class _T> struct is_const
	: false_type
{
};

template<class _T> struct is_const<const _T>
	: true_type
{
};

template<class _T> constexpr bool is_const_v = is_const<_T>::value;

template<class _T> struct remove_reference
{
	using type = _T;
};
template<class _T> struct remove_reference<_T &>
{
	using type = _T;
};
template<class _T> struct remove_reference<_T &&>
{
	using type = _T;
};
template<class _T> using remove_reference_t = typename remove_reference<_T>::type;

template<class _T> struct is_lvalue_reference
	: false_type
{
};

template<class _T> struct is_lvalue_reference<_T &>
	: true_type
{
};


template<class _T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<_T>::value;

template<class _T> constexpr _T &&forward(remove_reference_t<_T> &arg) noexcept
{
	return (static_cast<_T &&>(arg));
}
template<class _T> constexpr _T &&forward(remove_reference_t<_T> &&arg) noexcept
{
	static_assert(!is_lvalue_reference_v<_T>);
	return (static_cast<_T &&>(arg));
}

template<bool _Test, class _T = void> struct enable_if
{
};

template<class _T> struct enable_if<true, _T>
{
	using type = _T;
};

template<bool _Test, class _T = void> using enable_if_t = typename enable_if<_Test, _T>::type;

template<class _T> constexpr const _T &min_value(const _T &left, const _T &right) noexcept
{
	return (right < left ? right : left);
}

template<class _T> constexpr const _T &max_value(const _T &left, const _T &right) noexcept
{
	return (left < right ? right : left);
}

template<class _T> constexpr size_t align_of = max_value(alignof(_T), static_cast<size_t>(DefaultAlignment));

template<size_t TypeSize> inline size_t get_size_of_n(const size_t count)
{
	constexpr size_t max_possible = static_cast<size_t>(-1) / TypeSize;
	size_t result = count * TypeSize;
	if (max_possible < count)
	{
		result = static_cast<size_t>(-1);
	}

	return result;
}

template<> inline size_t get_size_of_n<1>(const size_t count)
{
	return count;
}

namespaceEnd(FakeSTL)