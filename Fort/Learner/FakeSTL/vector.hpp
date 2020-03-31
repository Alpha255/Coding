#pragma once

#include "memory.hpp"

namespaceStart(FakeSTL)

template<class _T1, class _T2, bool = is_empty_v<_T1> && !is_final_v<_T1>> class compressed_pair final : private _T1
{
private:
	_T2 _val2;
	using _base = _T1;
public:
	template<class... _Other> constexpr explicit compressed_pair(_Other&& ...vals)
		: _T1(),
		_val2(forward<_Other>(vals)...)
	{
	}

	template<class... _Other1, class... _Other2> compressed_pair(_Other1 &&val1, _Other2&&... val2)
		: _T1(forward<_Other1>(val1)),
		_val2(forward<_Other2>(val2)...)
	{
	}

	_T1 &get_first() noexcept
	{
		return *this;
	}
	const _T1 &get_first() const noexcept
	{
		return *this;
	}

	_T2 &get_second() noexcept
	{
		return _val2;
	}
	const _T2 &get_second() const noexcept
	{
		return _val2;
	}
};

namespaceEnd(FakeSTL)
