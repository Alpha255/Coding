#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class NoneCopyable
{
public:
	NoneCopyable() = default;
	~NoneCopyable() = default;

	NoneCopyable(const NoneCopyable&) = delete;
	void operator=(const NoneCopyable&) = delete;
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

template<class T> class LazySingleton : public NoneCopyable
{
public:
	template <typename... Args> static void initialize(Args&&... args)
	{
		if (!s_Instance)
		{
			s_Instance = new T(std::forward<Args>(args)...);
		}
	}

	static void finalize()
	{
		safeDelete(s_Instance);
	}

	static T* instance()
	{
		assert(s_Instance);
		return s_Instance;
	}
protected:
	LazySingleton() = default;
	virtual ~LazySingleton() = default;
private:
	static T* s_Instance;
};
template <class T> T* LazySingleton<T>::s_Instance = nullptr;

#define lazySingletonDeclare(className)      \
private:                                     \
	className() = default;                   \
	~className() = default;                  \
	friend class Gear::LazySingleton<className>;

namespaceEnd(Gear)


