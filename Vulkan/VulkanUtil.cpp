#include "VulkanUtil.h"

NamespaceBegin(VulkanUtil)

std::vector<const char *> FilterLayers(const std::vector<VkLayerProperties> &supportedLayers)
{
	std::vector<std::string> layers;
#if defined(_DEBUG)
	/// https://vulkan.lunarg.com/doc/view/1.0.13.0/windows/layers.html
	layers.emplace_back("VK_LAYER_LUNARG_standard_validation");
	///layers.emplace_back("VK_LAYER_LUNARG_api_dump");
	///layers.emplace_back("VK_LAYER_GOOGLE_threading");
	///layers.emplace_back("VK_LAYER_GOOGLE_unique_objects");
	///layers.emplace_back("VK_LAYER_LUNARG_parameter_validation");
	///layers.emplace_back("VK_LAYER_LUNARG_object_tracker");
	///layers.emplace_back("VK_LAYER_LUNARG_image");
	///layers.emplace_back("VK_LAYER_LUNARG_device_limits");
	///layers.emplace_back("VK_LAYER_LUNARG_core_validation");
	///layers.emplace_back("VK_LAYER_LUNARG_swapchain");
#endif
	layers.emplace_back("VK_LAYER_RENDERDOC_Capture");

	std::vector<const char *> enabledLayers;
	for each (auto &layer in layers)
	{
		for each (auto &supportedLayer in supportedLayers)
		{
			if (layer == supportedLayer.layerName)
			{
				enabledLayers.emplace_back(supportedLayer.layerName);
			}
		}
	}

	return enabledLayers;
}

std::vector<const char *> FilterExtensions(const std::vector<VkExtensionProperties> &supportedExtensions)
{
	/// https://vulkan.lunarg.com/doc/view/1.0.39.1/windows/LoaderAndLayerInterface.html
	std::vector<std::string> extensions;
	extensions.emplace_back("VK_KHR_xcb_surface");  /// Linux
	extensions.emplace_back("VK_KHR_xlib_surface"); /// Linux
	extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);;
	extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if VK_EXT_debug_report
	extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
#if VK_EXT_debug_marker
	extensions.emplace_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
#endif

	std::vector<const char *> enabledExtensions;
	for each (auto &extension in extensions)
	{
		for each (auto &supportedExtension in supportedExtensions)
		{
			if (extension == supportedExtension.extensionName)
			{
				enabledExtensions.emplace_back(supportedExtension.extensionName);
			}
		}
	}

	return enabledExtensions;
}

NamespaceEnd(VulkanUtil)