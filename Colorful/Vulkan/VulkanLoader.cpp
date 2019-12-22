#include "VulkanEngine.h"

#if defined(UsingVkLoader)

vkFunctionTableDefinition

#define logIfFunctionIsNull(func)                                                              \
	if (!func)                                                                                 \
	{                                                                                          \
		logger::instance().log(logger::eError, "Can't get entry point of function %s", #func); \
		assert(0);                                                                             \
	}

static gear::dynamicLibrary s_VulkanLoader;

void vkLoader::initializeGlobalFunctionTable()
{
	s_VulkanLoader.load("vulkan-1.dll");

#define createFunctionTable(func)                              \
	func = (PFN_##func)(s_VulkanLoader.getProcAddress(#func)); \
	logIfFunctionIsNull(func)

	vkGlobalFunctionTable(createFunctionTable)
#undef createFunctionTable
}

void vkLoader::initializeInstanceFunctionTable(const vkInstance &instance)
{
	assert(instance.isValid() && vkGetInstanceProcAddr);

#define createFunctionTable(func) \
	func = (PFN_##func)(vkGetInstanceProcAddr(*instance, #func)); \
	logIfFunctionIsNull(func)

	vkInstanceFunctionTable(createFunctionTable)
#undef createFunctionTable
}

void vkLoader::initializeDeviceFunctionTable(const vkDevice &device)
{
	assert(device.isValid() && vkGetDeviceProcAddr);

#define createFunctionTable(func) \
	func = (PFN_##func)(vkGetDeviceProcAddr(*device, #func)); \
	logIfFunctionIsNull(func)

	vkDeviceFunctionTable(createFunctionTable)
#undef createFunctionTable
}

void vkLoader::finalize()
{
	vkFunctionTableReset
}

#endif