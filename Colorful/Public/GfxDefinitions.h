#pragma once

#if 0

#include "Gear/Gear.h"
#include "Colorful/Public/GfxEnumerations.h"

#define UsingVkLoader

#if defined(UsingVkLoader)
	#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>

#include <d3d12.h>
#include <d3d12shader.h>

#endif