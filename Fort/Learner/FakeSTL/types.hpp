#pragma once

#include "Gear/gear.h"

namespaceStart(FakeSTL)

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

template<class T> struct is_const
	: false_type
{
};

template<class T> struct is_const
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

namespaceEnd(FakeSTL)