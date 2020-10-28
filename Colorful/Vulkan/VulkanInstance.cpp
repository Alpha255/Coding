#include "Colorful/Vulkan/VulkanInstance.h"

NAMESPACE_START(Gfx)

VulkanInstance::VulkanInstance()
{
	std::vector<const char8_t*> layers
	{
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	std::vector<const char8_t*> extensions
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(PLATFORM_WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(PLATFORM_LINUX)
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
#if defined(_DEBUG)
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
	};

	uint32_t count = 0U;
	VERIFY_VK(vkEnumerateInstanceLayerProperties(&count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	VERIFY_VK(vkEnumerateInstanceLayerProperties(&count, supportedLayers.data()));
	layers = getSupportedProperties<VkLayerProperties>(supportedLayers, layers);

	VERIFY_VK(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	VERIFY_VK(vkEnumerateInstanceExtensionProperties(nullptr, &count, supportedExtensions.data()));
	extensions = getSupportedProperties<VkExtensionProperties>(supportedExtensions, extensions);

	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"Vulkan_Application",
		0u,
		"Vulkan_Renderer",
		0u,
		VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0u,
		&appInfo,
		(uint32_t)layers.size(),
		layers.data(),
		(uint32_t)extensions.size(),
		extensions.data()
	};

	VERIFY_VK(vkCreateInstance(&createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageServerityFlagBits,
	VkDebugUtilsMessageTypeFlagsEXT messageTypeFlags,
	const VkDebugUtilsMessengerCallbackDataEXT *pMessengerCallbackData,
	void* userData)
{
	(void)(userData);
	(void)(messageTypeFlags);

	Gear::Log::ELevel level = Gear::Log::ELevel::Error;
	if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT || messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		level = Gear::Log::ELevel::Info;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		level = Gear::Log::ELevel::Warning;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		level = Gear::Log::ELevel::Error;
	}

	std::string message(std::move(Gear::String::format("Vulkan Validation: [%3d][%10s]: %s", 
		pMessengerCallbackData->messageIdNumber, 
		pMessengerCallbackData->pMessageIdName, 
		pMessengerCallbackData->pMessage)));

	Gear::Log::log(level, message);
	return VK_FALSE;
}

VulkanDebugUtilsMessenger::VulkanDebugUtilsMessenger(VkInstance instance, bool8_t verbose)
{
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMesserger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	assert(vkCreateDebugUtilsMesserger);

	VkDebugUtilsMessageSeverityFlagsEXT messageServityFlags = verbose ? (
		  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) :
		(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

	VkDebugUtilsMessengerCreateInfoEXT createInfo
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		nullptr,
		0u,
		messageServityFlags,
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		vkDebugUtilsMessengerFunc,
		nullptr
	};

	VERIFY_VK(vkCreateDebugUtilsMesserger(instance, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

NAMESPACE_END(Gfx)