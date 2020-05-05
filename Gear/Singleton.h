#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class NoneCopyable
{
public:
	NoneCopyable() = default;
	~NoneCopyable() = default;

	NoneCopyable(const NoneCopyable &) = delete;
	void operator=(const NoneCopyable &) = delete;
protected:
private:
};

template <typename T> class Singleton : public NoneCopyable
{
public:
	inline static T &instance()
	{
		return s_Instance;
	}

	Singleton(const Singleton &) = delete;
	void operator=(const Singleton &) = delete;
protected:
	Singleton() = default;
	virtual ~Singleton() = default;
private:
	static T s_Instance;
};
template<class T> __declspec(selectany) T Singleton<T>::s_Instance;

#define singletonDeclare(className)          \
private:                                     \
	className() = default;                   \
	~className() = default;                  \
	friend class Gear::Singleton<className>;

template <typename T> class LazySingleton
{
public:
	template <typename... Args> static T *instance(Args &&... args)
	{
		if (nullptr == s_Instance)
		{
			s_Instance = new T(std::forward<Args>(args)...);
			assert(s_Instance);
		}
		return s_Instance;
	}

	static void destroy()
	{
		if (s_Instance)
		{
			delete s_Instance;
			s_Instance = nullptr;
		}
	}
private:
	LazySingleton() = default;
	virtual ~LazySingleton() = default;
	LazySingleton(const LazySingleton &) {}
	void operator=(const LazySingleton &) {}

	static T *s_Instance;
};
template <typename T> T *LazySingleton<T>::s_Instance = nullptr;

namespaceEnd(Gear)


