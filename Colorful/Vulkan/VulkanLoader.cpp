#include "VulkanEngine.h"

#if defined(UsingVkLoader)

vkFunctionTableDefinition

#define logIfFunctionIsNull(func)                                                              \
	if (!func)                                                                                 \
	{                                                                                          \
		Logger::instance().log(Logger::eError, "Can't get entry point of function %s", #func); \
		assert(0);                                                                             \
	}

static Gear::DynamicLibrary s_VulkanLoader;

void VulkanLoader::initializeGlobalFunctionTable()
{
	s_VulkanLoader.load("vulkan-1.dll");

#define createFunctionTable(func)                              \
	func = (PFN_##func)(s_VulkanLoader.getProcAddress(#func)); \
	logIfFunctionIsNull(func)

	vkGlobalFunctionTable(createFunctionTable)
#undef createFunctionTable
}

void VulkanLoader::initializeInstanceFunctionTable(VkInstance instance)
{
	assert(instance != VK_NULL_HANDLE && vkGetInstanceProcAddr);

#define createFunctionTable(func) \
	func = (PFN_##func)(vkGetInstanceProcAddr(instance, #func)); \
	logIfFunctionIsNull(func)

	vkInstanceFunctionTable(createFunctionTable)
#undef createFunctionTable
}

void VulkanLoader::initializeDeviceFunctionTable(VkDevice device)
{
	assert(device != VK_NULL_HANDLE && vkGetDeviceProcAddr);

#define createFunctionTable(func) \
	func = (PFN_##func)(vkGetDeviceProcAddr(device, #func)); \
	logIfFunctionIsNull(func)

	vkDeviceFunctionTable(createFunctionTable)
#undef createFunctionTable
}

void VulkanLoader::finalize()
{
	vkFunctionTableReset
}

#endif