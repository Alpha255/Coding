#pragma once

#include "Gear/Gear.h"
#include "Colorful/Public/REnumerations.h"

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
	inline T operator*() const
	{
		return (T)(m_Object.get());
	}

	inline void reset(T other = NULL)
	{
		if (other)
		{
			m_Object.reset((T *)other, std::function<void(T *&)>([](T *&pObject)
			{
				pObject = NULL;
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

template <typename T> class vkDeviceObject : public vkObject<T>
{
public:
	virtual void destory(const class vkDevice &device) = 0;
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

#define rVerifyD3D11(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		d3d11Engine::instance().logError(result); \
	}                                             \
}

#define rVerifyD3D12(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		d3d12Engine::instance().logError(result); \
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

#include <vulkan/vulkan.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <d3d12.h>
#include <d3d12shader.h>
