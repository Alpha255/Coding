#pragma once

#include "Gear/Public/Definitions.h"

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

#define singletonDeclare(className)          \
private:                                     \
	className() = default;                   \
	~className() = default;                  \
	friend class gear::singleton<className>;

template <typename T> class lazySingleton
{
public:
	template <typename... Args> static T *instance(Args &&... args)
	{
		if (nullptr == s_pInstance)
		{
			s_pInstance = new T(std::forward<Args>(args)...);
			assert(s_pInstance);
		}
		return s_pInstance;
	}

	static void destroy()
	{
		if (s_pInstance)
		{
			delete s_pInstance;
			s_pInstance = nullptr;
		}
	}
private:
	lazySingleton() = default;
	virtual ~lazySingleton() = default;
	lazySingleton(const lazySingleton &) {}
	void operator=(const lazySingleton &) {}

	static T *s_pInstance;
};
template <typename T> T *lazySingleton<T>::s_pInstance = nullptr;

namespaceEnd(gear)


