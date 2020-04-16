#pragma once

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

template<class T> class Allocator
{
public:
protected:
private:
};