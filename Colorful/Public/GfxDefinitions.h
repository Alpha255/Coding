#pragma once

#if 0

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

#if !defined(_DEBUG)
	#define UsingUnorderedMap
#endif

#endif