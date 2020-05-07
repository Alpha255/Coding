#pragma once

#include "Gear/Gear.h"
#include "Colorful/Public/Enumerations.h"

template <typename T> class SharedObject
{
public:
	inline SharedObject() = default;
	virtual ~SharedObject() = default;

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

	inline uint32_t refCount() const
	{
		return m_Object.use_count();
	}
protected:
	std::shared_ptr<T> m_Object = nullptr;
private:
};

template <typename T> class D3DObject : public SharedObject<T>
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

template<typename T> struct VulkanObject
{
	T Handle = nullptr;

	inline bool8_t isValid() const
	{
		return Handle != nullptr;
	}
};

template <typename T> class VulkanDeviceObject : public VulkanObject<T>
{
public:
	virtual void destroy(const class VulkanDevice &device) = 0;
};

#define rVerifyD3D11(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		d3d11Engine::logError(result);            \
	}                                             \
}

#define rVerifyD3D12(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		d3d12Engine::logError(result);            \
	}                                             \
}

#define rVerifyVk(func)                        \
{                                              \
	VkResult result = (func);                  \
	if (result != VK_SUCCESS)                  \
	{                                          \
		vkEngine::logError(result);            \
	}                                          \
}

#define UsingVkLoader

#if defined(UsingVkLoader)
	#define VK_NO_PROTOTYPES
#endif

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

#define vkMemoryAllocator nullptr /// For future use
