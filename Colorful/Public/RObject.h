#pragma once

#include "gear/gear.h"

template <typename T> class rObject
{
public:
	inline rObject() = default;
	virtual ~rObject() = default;

	inline bool8_t isValid() const
	{
		return m_Object != nullptr;
	}

	inline T *get() const
	{
		return m_Object.get();
	}

	inline T *operator->() const
	{
		assert(isValid());
		return get();
	}

	inline uint32_t getRefCount() const
	{
		return m_Object.use_count();
	}
protected:
	std::shared_ptr<T> m_Object = nullptr;
private:
};

template <typename T> class vkObject : public rObject<T>
{
public:
	inline T operator&() const
	{
		return (T)(m_Object.get());
	}

	inline void reset(T other = VK_NULL_HANDLE)
	{
		if (other)
		{
			m_Object.reset((T *)other, std::function<void(T *&)>([](T *&pObject)
			{
				pObject = VK_NULL_HANDLE;
			}));
		}
		else
		{
			if (m_Object)
			{
				m_Object.reset();
				m_Object = nullptr;
			}
		}
	}
};

template <typename T> class d3dObject : public rObject<T>
{
public:
	inline void reset(T *other = nullptr)
	{
		if (other)
		{
			m_Object.reset(other, std::function<void(T *&)>([](T *&pObject) 
			{ 
				pObject->Release(); 
				pObject = nullptr;
			}));
		}
		else
		{
			if (m_Object)
			{
				m_Object.reset();
				m_Object = nullptr;
			}
		}
	}
};

#if 0
#define rVerify(func)                                 \
{                                                     \
	auto result = (func);                             \
	if (result != 0)                                  \
	{                                                 \
		if (g_rEnginePtr)                             \
		{                                             \
			g_rEnginePtr->logError((uint32_t)result); \
		}                                             \
	}                                                 \
}
#endif

#define rVerifyD3D11(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		d3d11Engine::instance().logError(result); \
	}                                             \
}

#define rVerifyVk(func)                        \
{                                              \
	VkResult result = (func);                  \
	if (result != VK_SUCCESS)                  \
	{                                          \
		vkEngine::instance().logError(result); \
	}                                          \
}
