#pragma once

template<class T>
class InitializerList
{
public:
	typedef T value_type;
	typedef const T &reference;
	typedef const T &const_reference;
	typedef const T *iterator;
	typedef const T *const_iterator;

	constexpr InitializerList() noexcept
	{
	}

	constexpr InitializerList(const T *first, const T *last) noexcept
		: m_First(first)
		, m_Last(last)
	{
	}

	constexpr const T *Begin() const noexcept
	{
		return m_First;
	}

	constexpr const T *End() const noexcept
	{
		return m_Last;
	}

	constexpr size_t Size() const noexcept
	{
		return (static_cast<size_t>(m_Last - m_First));
	}
private:
	const T *m_First = nullptr;
	const T *m_Last = nullptr;
};