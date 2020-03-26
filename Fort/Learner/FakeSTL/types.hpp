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

namespaceEnd(FakeSTL)