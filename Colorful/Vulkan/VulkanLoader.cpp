#include "Colorful/Vulkan/VulkanLoader.h"

#if defined(USE_VK_LOADER)

NAMESPACE_START(Gfx)

VK_FUNC_TABLE_DEFINITION

#define VK_LOADER_VERIFY_FUNC(Func)                               \
	if (!Func)                                                    \
	{                                                             \
		LOG_ERROR("Can't get entry point of function %s", #Func); \
		assert(0);                                                \
	}

void VulkanLoader::initialize(VkInstance instance, VkDevice device)
{
	assert(instance && device && vkGetInstanceProcAddr && vkGetDeviceProcAddr);
	assert(!s_DynamicLib);
	s_DynamicLib = std::make_shared<System::DynamicLibrary>("vulkan-1.dll");

#define VK_LOADER_LOAD_FUNCTIONS(Func)                           \
	Func = (PFN_##Func)(s_DynamicLib->getProcAddress(#Func));    \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_GLOBAL_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS

#define VK_LOADER_LOAD_FUNCTIONS(Func)                           \
	Func = (PFN_##Func)(vkGetInstanceProcAddr(instance, #Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_INSTANCE_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS

#define VK_LOADER_LOAD_FUNCTIONS(Func)                           \
	Func = (PFN_##Func)(vkGetDeviceProcAddr(device, #Func));     \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_DEVICE_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

void VulkanLoader::finalize()
{
	VK_FUNC_TABLE_RESET
}

NAMESPACE_END(Gfx)

#endif