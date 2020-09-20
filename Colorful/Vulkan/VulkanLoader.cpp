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

System::DynamicLibraryPtr VulkanLoader::s_DynamicLib = nullptr;

void VulkanLoader::loadGlobalFuncs()
{
	assert(!s_DynamicLib);
	s_DynamicLib = std::make_shared<System::DynamicLibrary>("vulkan-1.dll");

#define VK_LOADER_LOAD_FUNCTIONS(Func)                           \
	Func = (PFN_##Func)(s_DynamicLib->getProcAddress(#Func));    \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_GLOBAL_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

void VulkanLoader::loadInstanceFuncs(VkInstance instance)
{
	assert(instance && vkGetInstanceProcAddr);

#define VK_LOADER_LOAD_FUNCTIONS(Func)                           \
	Func = (PFN_##Func)(vkGetInstanceProcAddr(instance, #Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_INSTANCE_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

void VulkanLoader::loadDeviceFuncs(VkDevice device)
{
	assert(device && vkGetDeviceProcAddr);

#define VK_LOADER_LOAD_FUNCTIONS(Func)                           \
	Func = (PFN_##Func)(vkGetDeviceProcAddr(device, #Func));     \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_DEVICE_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

void VulkanLoader::free()
{
	VK_FUNC_TABLE_RESET
}

const char8_t* const VulkanErrorLog::getErrorMessage(VkResult result)
{
	switch (result)
	{
	case VK_SUCCESS:                                            return "Success";
	case VK_NOT_READY:                                          return "Not Ready";
	case VK_TIMEOUT:                                            return "Time Out";
	case VK_EVENT_SET:                                          return "Set Event";
	case VK_EVENT_RESET:                                        return "Reset Event";
	case VK_INCOMPLETE:                                         return "Incomplete";
	case VK_ERROR_OUT_OF_HOST_MEMORY:                           return "Out Of Host Memory";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:                         return "Out Of Device Memory";
	case VK_ERROR_INITIALIZATION_FAILED:                        return "Failed To Initialization";
	case VK_ERROR_DEVICE_LOST:                                  return "Lost Device";
	case VK_ERROR_MEMORY_MAP_FAILED:                            return "Failed To Map Memory";
	case VK_ERROR_LAYER_NOT_PRESENT:                            return "Layer Not Present";
	case VK_ERROR_EXTENSION_NOT_PRESENT:                        return "Extension Not Present";
	case VK_ERROR_FEATURE_NOT_PRESENT:                          return "Feature Not Present";
	case VK_ERROR_INCOMPATIBLE_DRIVER:                          return "Incompatible Driver";
	case VK_ERROR_TOO_MANY_OBJECTS:                             return "Too Many Objects";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:                         return "Format Not Supported";
	case VK_ERROR_FRAGMENTED_POOL:                              return "Fragmented Pool";
	case VK_ERROR_OUT_OF_POOL_MEMORY:                           return "Out Of Pool Memory";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:                      return "Invalid External Handle";
	case VK_ERROR_SURFACE_LOST_KHR:                             return "Lost Surface";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:                     return "Window Is In Use";
	case VK_SUBOPTIMAL_KHR:                                     return "Suboptimal";
	case VK_ERROR_OUT_OF_DATE_KHR:                              return "Out Of Date";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:                     return "Incompatible Display";
	case VK_ERROR_VALIDATION_FAILED_EXT:                        return "Failed To Validation";
	case VK_ERROR_INVALID_SHADER_NV:                            return "Invalid Shader-Nvidia";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "Invalid DRM Format Modifier Plane Layout";
	case VK_ERROR_FRAGMENTATION_EXT:                            return "Fragmentation";
	case VK_ERROR_NOT_PERMITTED_EXT:                            return "Not Premited";
	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:                   return "Invalid Device Address";
	}

	return "Unknown error";
}

NAMESPACE_END(Gfx)

#endif
