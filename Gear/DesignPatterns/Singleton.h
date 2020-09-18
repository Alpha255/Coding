#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

class NoneCopyable
{
public:
	NoneCopyable() = default;
	~NoneCopyable() = default;

	NoneCopyable(const NoneCopyable&) = delete;
	NoneCopyable& operator=(const NoneCopyable&) = delete;
protected:
private:
};

template<class T> class TSingleton : public NoneCopyable
{
public:
	inline static T& instance()
	{
		return s_Instance;
	}

	TSingleton(const TSingleton&) = delete;
	void operator=(const TSingleton&) = delete;
protected:
	TSingleton() = default;
	virtual ~TSingleton() = default;
private:
	static T s_Instance;
};
template<class T> __declspec(selectany) T TSingleton<T>::s_Instance;

#define DECLARE_SINGLETON(ClassName)         \
private:                                     \
	ClassName() = default;                   \
	~ClassName() = default;                  \
	friend class Gear::TSingleton<ClassName>;

template<class T> class TLazySingleton : public NoneCopyable
{
public:
	template <typename... Args> static void initialize(Args&&... args)
	{
		if (!s_Instance)
		{
			s_Instance = new T(std::forward<Args>(args)...);
		}
	}

	void finalize()
	{
		cleanup();
		SAFE_DELETE(s_Instance);
	}

	static T* instance()
	{
		assert(s_Instance);
		return s_Instance;
	}

	virtual void cleanup()
	{
	}
protected:
	TLazySingleton() = default;
	virtual ~TLazySingleton() = default;
private:
	static T* s_Instance;
};
template <class T> T* TLazySingleton<T>::s_Instance = nullptr;

#define DECLARE_LAZY_SINGLETON(ClassName)    \
private:                                     \
	~ClassName() = default;                  \
	friend class Gear::TLazySingleton<ClassName>;

NAMESPACE_END(Gear)


