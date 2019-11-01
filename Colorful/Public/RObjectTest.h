#include "RObject.h"
#include "RInterface.h"

#include <d3d11.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

template <typename T> std::vector<const char8_t *> getSupportedProperties(
	const std::vector<T> &supportedProperties,
	const std::vector<const char8_t *> &targetProperties)
{
	std::vector<const char8_t *> result;

	for each (auto propertyName in targetProperties)
	{
		for (auto it = supportedProperties.begin(); it != supportedProperties.end(); ++it)
		{
			if (_stricmp((const char8_t *)(&(*it)), propertyName) == 0)
			{
				result.emplace_back(propertyName);
				break;
			}
		}
	}

	return result;
}

class vkInstance : public vkObject<VkInstance>
{
public:
};
typedef std::shared_ptr<vkInstance> vkInstancePtr;

class vkEngine
{
public:
	vkInstancePtr createInstance()
	{
		std::vector<const char8_t*> layers =
		{
			"VK_LAYER_LUNARG_standard_validation",
	#if defined(_DEBUG)
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_parameter_validation"
	#endif
		};

		std::vector<const char8_t*> extensions =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		};

		uint32_t count = 0U;
		rVerify(vkEnumerateInstanceLayerProperties(&count, nullptr));
		std::vector<VkLayerProperties> supportedLayers(count);
		rVerify(vkEnumerateInstanceLayerProperties(&count, supportedLayers.data()));
		layers = getSupportedProperties<VkLayerProperties>(supportedLayers, layers);

		rVerify(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
		std::vector<VkExtensionProperties> supportedExtensions(count);
		rVerify(vkEnumerateInstanceExtensionProperties(nullptr, &count, supportedExtensions.data()));
		extensions = getSupportedProperties<VkExtensionProperties>(supportedExtensions, extensions);

		VkApplicationInfo appInfo
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"vkApplication",
			0U,
			"vkEngine",
			0U,
			VK_API_VERSION_1_0
		};

		VkInstanceCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0U,
			&appInfo,
			(uint32_t)layers.size(),
			layers.data(),
			(uint32_t)extensions.size(),
			extensions.data()
		};

		VkInstance handle = VK_NULL_HANDLE;
		rVerify(vkCreateInstance(&createInfo, nullptr, &handle));
		vkInstance instance;
		instance.reset(&handle);

		return std::make_shared<vkInstance>(instance);
	}
protected:
private:
};