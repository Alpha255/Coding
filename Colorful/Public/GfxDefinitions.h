#pragma once

#include "Gear/Gear.h"
#include "Colorful/Public/GfxEnumerations.h"

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

template<typename T> class SharedObject
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

	using Type = T;
protected:
	std::shared_ptr<T> m_Object = nullptr;
private:
};

template<typename T> class D3DObject : public SharedObject<T>
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

	virtual ~VulkanObject() = default;
};

#define GfxVerifyD3D(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		D3D11Engine::logError(result);            \
	}                                             \
}

#define rVerifyD3D12(func)                        \
{                                                 \
	::HRESULT result = (func);                    \
	if (FAILED(result))                           \
	{                                             \
		D3D12Engine::logError(result);            \
	}                                             \
}

#define GfxVerifyVk(func)                         \
{                                                 \
	VkResult result = (func);                     \
	if (result != VK_SUCCESS)                     \
	{                                             \
		VulkanEngine::logError(result);           \
	}                                             \
}

template <typename T> class VulkanDeviceObject : public VulkanObject<T>
{
public:
	virtual void destroy(VkDevice device) = 0;
};

#if !defined(_DEBUG)
	#define UsingUnorderedMap
#endif

#define DeclareShared(ClassName)                   \
class ClassName;                                   \
using ClassName##Ptr = std::shared_ptr<ClassName>;