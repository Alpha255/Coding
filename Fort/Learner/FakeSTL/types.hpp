#pragma once

#include "Gear/gear.h"

namespaceStart(FakeSTL)

template<class... _Types> using void_t = void;

template <class _T, _T _Val> struct integral_constant
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
template<bool _Val> using bool_constant = integral_constant<bool, _Val>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template<class _T> struct is_const : false_type {};
template<class _T> struct is_const<const _T> : true_type {};
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

template<class _T> struct remove_const
{
	using type = _T;
};
template<class _T> struct remove_const<const _T>
{
	using type = _T;
};
template<class _T> using remove_const_t = typename remove_const<_T>::type;

template<class _T> struct remove_volatile
{
	using type = _T;
};
template<class _T> struct remove_const<volatile _T>
{
	using type = _T;
};
template<class _T> using remove_volatile_t = typename remove_volatile<_T>::type;

template<class _T> struct remove_cv
{
	using type = _T;
};
template<class _T> struct remove_cv<const _T>
{
	using type = _T;
};
template<class _T> struct remove_cv<volatile _T>
{
	using type = _T;
};
template<class _T> struct remove_cv<const volatile _T>
{
	using type = _T;
};
template<class _T> using remove_cv_t = typename remove_cv<_T>::type;

template<class _T> struct _is_integral : false_type {};
template<> struct _is_integral<bool> : true_type {};
template<> struct _is_integral<char> : true_type {};
template<> struct _is_integral<unsigned char> : true_type {};
template<> struct _is_integral<signed char> : true_type {};
template<> struct _is_integral<wchar_t> : true_type {};
template<> struct _is_integral<char16_t> : true_type {};
template<> struct _is_integral<char32_t> : true_type {};
template<> struct _is_integral<unsigned short> : true_type {};
template<> struct _is_integral<short> : true_type {};
template<> struct _is_integral<unsigned int> : true_type {};
template<> struct _is_integral<int> : true_type {};
template<> struct _is_integral<unsigned long> : true_type {};
template<> struct _is_integral<long> : true_type {};
template<> struct _is_integral<unsigned long long> : true_type {};
template<> struct _is_integral<long long> : true_type {};
template<class _T> struct is_integral : _is_integral<remove_cv_t<_T>>::type {};
template<class _T> constexpr bool is_integral_v = is_integral<_T>::value;

template<class _T> struct _is_floating_point : false_type {};
template<> struct _is_floating_point<float> : true_type {};
template<> struct _is_floating_point<double> : true_type {};
template<> struct _is_floating_point<long double> : true_type {};
template<class _T> struct is_floating_point : _is_floating_point<remove_cv_t<_T>>::type {};
template<class _T> constexpr bool is_floating_point_v = is_floating_point<_T>::value;

template<class _T> struct is_lvalue_reference : false_type {};
template<class _T> struct is_lvalue_reference<_T &> : true_type {};
template<class _T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<_T>::value;

template<class _T1, class _T2> struct is_same : false_type { };
template<class _T1> struct is_same<_T1, _T1> : true_type {};
template<class _T1, class _T2> constexpr bool is_same_v = is_same<_T1, _T2>::value;

template<class _Ty> constexpr bool is_pod_v = __is_pod(_Ty);
template<class _Ty> constexpr bool is_empty_v = __is_empty(_Ty);
template<class _Ty> constexpr bool is_final_v = __is_final(_Ty);

template<bool _Test, class _T = void> struct enable_if {};
template<class _T> struct enable_if<true, _T>
{
	using type = _T;
};
template<bool _Test, class _T = void> using enable_if_t = typename enable_if<_Test, _T>::type;

template<bool _Test, class _T1, class _T2> struct conditional
{
	using type = _T2;
};

template<class _T1, class _T2> struct conditional<true, _T1, _T2>
{
	using type = _T1;
};
template<bool _Test, class _T1, class _T2> using conditional_t = typename conditional<_Test, _T1, _T2>::type;

template<class _T> constexpr _T &&forward(remove_reference_t<_T> &arg) noexcept
{
	return (static_cast<_T &&>(arg));
}
template<class _T> constexpr _T &&forward(remove_reference_t<_T> &&arg) noexcept
{
	static_assert(!is_lvalue_reference_v<_T>, "bad forward call");
	return (static_cast<_T &&>(arg));
}

template<class _T> struct get_first_parameter;
template<template<class, class...> class _T, class _First, class... _Rest> struct get_first_parameter<_T<_First, _Rest...>>
{
	using type = _First;
};

template<class _T, class = void> struct get_ptr_difference_type
{
	using type = ptrdiff_t;
};
template<class _T> struct get_ptr_difference_type<_T, void_t<typename _T::difference_type>>
{
	using type = typename _T::difference_type;
};

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

template<class _Newfirst, class _T> struct replace_first_parameter;
template<class _Newfirst, template<class, class...> class _T, class _First, class... _Rest> struct replace_first_parameter<_Newfirst, _T<_First, _Rest...>>
{
	using type = _T<_Newfirst, _Rest...>;
};

template<class _T, class _Other, class = void> struct get_rebind_type
{
	using type = typename replace_first_parameter<_Other, _T>::type;
};
template<class _T, class _Other> struct get_rebind_type<_T, _Other, void_t<typename _T::template rebind<_Other>::other>>
{
	using type = typename _T::template rebind<_Other>::other;
};

template<class _T, class _Other, class = void> struct get_rebind_alias
{
	using type = typename replace_first_parameter<_Other, _T>::type;
};
template<class _T, class _Other> struct get_rebind_alias<_T, _Other, void_t<typename _T::template rebind<_Other>>>
{
	using type = typename _T::template rebind<_Other>;
};

template<class _T, class = void> struct get_element_type
{
	using type = typename get_first_parameter<_T>::type;
};
template<class _T> struct get_element_type<_T, void_t<typename _T::element_type>>
{
	using type = typename _T::element_type;
};

template<class _T> struct pointer_traits
{
	using element_type = typename  get_element_type<_T>::type;
	using pointer = _T;
	using difference_type = typename get_ptr_difference_type<_T>::type;

	template<class _Other> using rebind = typename get_rebind_alias<_T, _Other>::type;
};

template<class _T, class = void> struct get_pointer_type
{
	using type = typename _T::value_type *;
};
template<class _T> struct get_pointer_type<_T, void_t<typename _T::pointer>>
{
	using type = typename _T::pointer;
};
template<class _T, class = void> struct get_const_pointer_type
{
	using pointer_type = typename get_pointer_type<_T>::type;
	using value_type = typename _T::value_type;
	using type = typename pointer_traits<pointer_type>::template rebind<const value_type>;
};
template<class _T, class = void> struct get_void_pointer_type
{
	using pointer_type = typename get_pointer_type<_T>::type;
	using type = typename pointer_traits<pointer_type>::template rebind<void>;
};
template<class _T> struct get_void_pointer_type<_T, void_t<typename _T::void_pointer>>
{
	using type = typename _T::void_pointer;
};
template<class _T, class = void> struct get_const_void_pointer_type
{
	using pointer_type = typename get_pointer_type<_T>::type;
	using type = typename pointer_traits<pointer_type>::template rebind<const void>;
};
template<class _T> struct get_const_void_pointer_type<_T, void_t<typename _T::const_void_pointer>>
{
	using type = typename _T::const_void_pointer;
};

template<class _T, class = void> struct get_difference_type
{
	using pointer_type = typename get_pointer_type<_T>::type;
	using type = typename pointer_traits<pointer_type>::difference_type;
};
template<class _T> struct get_difference_type<_T, void_t<typename _T::difference_type>>
{
	using type = typename _T::difference_type;
};

template<class _T, bool = is_integral_v<_T>> struct _sign_base
{
	using _type = remove_cv_t<_T>;
	using _signed = bool_constant<_type(-1) < _type(0)>;
	using _unsigned = bool_constant<_type(0) < _type(-1)>;
};
template<class _T> struct _sign_base<_T, false>
{
	using _signed = typename is_floating_point<_T>::type;
	using _unsigned = false_type;
};
template<class _T> struct is_signed : _sign_base<_T>::_signed
{
};
template<class _T> constexpr bool is_signed_v = is_signed<_T>::value;
template<class _T> struct is_unsigned : _sign_base<_T>::_unsigned
{
};
template<class _T> constexpr bool is_unsigned_v = is_unsigned<_T>::value;

template<class _T> using is_nonbool_integral = bool_constant<is_integral_v<_T> && !is_same_v<remove_cv_t<_T>, bool>>;

template<class _T> struct is_enum : bool_constant<__is_enum(_T)>
{
};
template<class _T> constexpr bool is_enum_v = __is_enum(_T);

template<bool _First_value, class _First, class..._Rest> struct _conjunction
{
	using type = _First;
};
template<class _True, class _Next, class... _Rest> struct _conjunction<true, _True, _Next, _Rest...>
{
	using type = typename _conjunction<_Next::value, _Next, _Rest...>::type;
};
template<class... _Traits> struct conjunction : true_type
{
};
template<class _First, class... _Rest> struct conjunction<_First, _Rest...>
	: _conjunction<_First::value, _First, _Rest...>::type
{
};
template<class... _Traits> constexpr bool conjunction_v = conjunction<_Traits...>::value;

template<bool _First_value, class _First, class... _Rest> struct _disjunction
{
	using type = _First;
};
template<class _False, class _Next, class... _Rest> struct _disjunction<false, _False, _Next, _Rest...>
{
	using type = typename _disjunction<_Next::value, _Next, _Rest...>::type;
};
template<class... _Traits> struct disjunction : false_type
{
};
template<class _First, class... _Rest> struct disjunction<_First, _Rest...>
	: _disjunction<_First::value, _First, _Rest...>::type
{
};
template<class... _Traits> constexpr bool disjunction_v = disjunction<_Traits...>::value;
template<class _T, class... _Types> constexpr bool _is_any_of_v = disjunction_v<is_same<_T, _Types>...>;


template<class _T> struct change_sign
{
	static_assert(is_nonbool_integral<_T>::value || is_enum_v<_T>, "make_signed<T>/make_unsigned<T> require that T shall be a (possibly "
		"cv-qualified) integral type or enumeration but not a bool type.");

	using _signed =
		conditional_t<_is_any_of_v<_T, long, unsigned long>, long,
		conditional_t<sizeof(_T) == 1, signed char,
		conditional_t<sizeof(_T) == 2, short,
		conditional_t<sizeof(_T) == 4, int, long long>>>>;

	using _unsigned =
		conditional_t<_is_any_of_v<_T, long, unsigned long>, unsigned long,
		conditional_t<sizeof(_T) == 1, unsigned char,
		conditional_t<sizeof(_T) == 2, unsigned short,
		conditional_t<sizeof(_T) == 4, unsigned int, unsigned long long>>>>;
};
template<class _T> struct change_sign<const _T>
{
	using _signed = const typename change_sign<_T>::_signed;
	using _unsigned = const typename change_sign<_T>::_unsigned;
};
template<class _T> struct change_sign<volatile _T>
{
	using _signed = const typename change_sign<_T>::_signed;
	using _unsigned = const typename change_sign<_T>::_unsigned;
};
template<class _T> struct change_sign<const volatile _T>
{
	using _signed = const typename change_sign<_T>::_signed;
	using _unsigned = const typename change_sign<_T>::_unsigned;
};

template<class _T> struct make_signed
{
	using type = typename change_sign<_T>::_signed;
};
template<class _T> using make_signed_t = typename make_signed<_T>::type;

template<class _T> struct make_unsigned
{
	using type = typename change_sign<_T>::_unsigned;
};
template<class _T> using make_unsigned_t = typename make_unsigned<_T>::type;
template<class _T> constexpr make_unsigned_t<_T> _unsigned_value(_T value)
{
	return (static_cast<make_unsigned_t<_T>>(value));
}

template<class _T, class = void> struct get_size_type
{
	using type = make_unsigned_t<typename get_difference_type<_T>::type>;
};
template<class _T> struct get_size_type<_T, void_t<typename _T::size_type>>
{
	using type = typename _T::size_type;
};

namespaceEnd(FakeSTL)