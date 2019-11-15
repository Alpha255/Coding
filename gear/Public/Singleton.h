#pragma once

#include "Gear/Public/Common.h"

namespaceStart(gear)

class noneCopyable
{
public:
	noneCopyable() = default;
	~noneCopyable() = default;

	noneCopyable(const noneCopyable &) = delete;
	void operator=(const noneCopyable &) = delete;
protected:
private:
};

template <typename T> class singleton : public noneCopyable
{
public:
	inline static T &instance()
	{
		return s_Instance;
	}

	singleton(const singleton &) = delete;
	void operator=(const singleton &) = delete;
protected:
	singleton() = default;
	virtual ~singleton() = default;
private:
	static T s_Instance;
};
template<class T> __declspec(selectany) T singleton<T>::s_Instance;

#define singletonDeclare(ClassName)          \
private:                                     \
	ClassName() = default;                   \
	~ClassName() = default;                  \
	friend class gear::singleton<ClassName>;

namespaceEnd(gear)


