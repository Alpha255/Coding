#include "Colorful/Vulkan/VulkanEngine.h"

void VulkanInstance::create(const Gear::Configurations& config)
{
	assert(!isValid());

	std::vector<const char8_t*> layers =
	{
		"VK_LAYER_LUNARG_standard_validation",
#if defined(_DEBUG)
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	std::vector<const char8_t*> extensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(Platform_Win32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(Platform_Linux)
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	uint32_t count = 0U;
	rVerifyVk(vkEnumerateInstanceLayerProperties(&count, nullptr));
	std::vector<VkLayerProperties> supportedLayers(count);
	rVerifyVk(vkEnumerateInstanceLayerProperties(&count, supportedLayers.data()));
	layers = VulkanEngine::getSupportedProperties<VkLayerProperties>(supportedLayers, layers);

	rVerifyVk(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> supportedExtensions(count);
	rVerifyVk(vkEnumerateInstanceExtensionProperties(nullptr, &count, supportedExtensions.data()));
	extensions = VulkanEngine::getSupportedProperties<VkExtensionProperties>(supportedExtensions, extensions);

	VkApplicationInfo appInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"vkApplication",
		0U,
		"VulkanEngine",
		0U,
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

	rVerifyVk(vkCreateInstance(&createInfo, vkMemoryAllocator, &Handle));

#if defined(_DEBUG)
	m_DebugUtilsMessenger.create(Handle, config.VulkanValidationVerbose);
#endif

#if defined(UsingVkLoader)
	VulkanLoader::initializeInstanceFunctionTable(Handle);
#endif
}

void VulkanInstance::destroy()
{
	if (isValid())
	{
#if defined(_DEBUG)
		m_DebugUtilsMessenger.destroy(Handle);
#endif

		vkDestroyInstance(Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageServerityFlagBits,
	VkDebugUtilsMessageTypeFlagsEXT messageTypeFlags,
	const VkDebugUtilsMessengerCallbackDataEXT *pMessengerCallbackData,
	void* pUserData)
{
	(void)(pUserData);
	(void)(messageTypeFlags);

	Logger::eLogLevel logLevel = Logger::eError;
	if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		logLevel = Logger::eInfo;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		logLevel = Logger::eInfo;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		logLevel = Logger::eWarning;
	}
	else if (messageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		logLevel = Logger::eError;
	}

	std::string message = format("[%3d][%10s]: %s",
		pMessengerCallbackData->messageIdNumber,
		pMessengerCallbackData->pMessageIdName,
		pMessengerCallbackData->pMessage);

	Logger::instance().log(logLevel, "Vulkan Validation: %s.", message.c_str());
	return VK_FALSE;
}

void VulkanInstance::VulkanDebugUtilsMessenger::create(VkInstance instance, bool8_t verbose)
{
	assert(!isValid());

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMesserger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	assert(vkCreateDebugUtilsMesserger);

	VkDebugUtilsMessageSeverityFlagsEXT messageServityFlags = verbose ? (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
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

	rVerifyVk(vkCreateDebugUtilsMesserger(instance, &createInfo, vkMemoryAllocator, &Handle));
}

void VulkanInstance::VulkanDebugUtilsMessenger::destroy(VkInstance instance)
{
	if (isValid())
	{
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		assert(vkDestroyDebugUtilsMessenger);
		vkDestroyDebugUtilsMessenger(instance, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}