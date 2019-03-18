#include "VulkanTest.h"
#include "Base/AssetFile.h"
#include <ImGUI/imgui.h>

VkInstance vkInstance = VK_NULL_HANDLE;
std::vector<VkPhysicalDevice> vkPhysicalDevices;
VkDevice vkDevice = VK_NULL_HANDLE;
VkPhysicalDeviceProperties vkDeviceProperties = {};
VkPhysicalDeviceFeatures vkDeviceFeatures = {};
VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperties = {};
VkPhysicalDeviceFeatures vkEnabledFeatures = {};
std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties;
VkQueueFamilyIndex vkQueueFamilyIndex = {};
std::vector<const char*> vkEnabledDeviceExtensions;
std::vector<const char*> vkEnabledInstanceExtensions;
std::vector<std::string> vkSupportedExtensions;
VkCommandPool vkCommandPool = VK_NULL_HANDLE;
VkCommandPool vkCommandPool_S = VK_NULL_HANDLE;
VkQueue vkQueue = VK_NULL_HANDLE;
VkSemaphore vkSemaphoreComplete = VK_NULL_HANDLE, vkSemaphorePresent = VK_NULL_HANDLE;
VkSubmitInfo vkSubmitInfo = {};
VkPipelineStageFlags vkSubmitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
::HWND vkHwnd = nullptr;
uint32_t vkWidth = 0U, vkHeight = 0U;
uint32_t vkQueueNodeIndex = 0U;
uint32_t vkImageCount = 0U;
uint32_t vkImageIndex = 0U;
uint32_t vkVertexCount = 0U;
uint32_t vkIndexCount = 0U;
bool vkPrepared = false;
bool vkVSync = false;
VkFormat vkColorFormat;
VkFormat vkDepthFormat;
VkColorSpaceKHR vkColorSpace;
VkSwapchainKHR vkSwapchain = VK_NULL_HANDLE;
std::vector<VkBackBuffer> vkBackBuffers;
std::vector<VkCommandBuffer> vkDrawCmdBuffers;
std::vector<VkFence> vkWaitFences;
std::vector<VkFramebuffer> vkFrameBuffers;
std::vector<VkPipelineShaderStageCreateInfo> vkShaderStates;
VkDepthStencilView vkDepthStencilView;
VkRenderPass vkRenderpass = VK_NULL_HANDLE;
VkPipelineCache vkPipelineCache = VK_NULL_HANDLE;
VkPipeline vkPipeline = VK_NULL_HANDLE;
VkImage vkImageFont = VK_NULL_HANDLE;
VkImageView vkImageViewFont = VK_NULL_HANDLE;
VkDeviceMemory vkMemoryFontImage = VK_NULL_HANDLE;
VkSampler vkSampler = VK_NULL_HANDLE;
VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
VkMemoryBuffer vkVertexBuffer = {};
VkMemoryBuffer vkIndexBuffer = {};

bool ExtensionSupported(std::string extension)
{
	return (std::find(vkSupportedExtensions.begin(), vkSupportedExtensions.end(), extension) != vkSupportedExtensions.end());
}

uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(vkQueueFamilyProperties.size()); i++)
		{
			if ((vkQueueFamilyProperties[i].queueFlags & queueFlags) && 
				((vkQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	if (queueFlags & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(vkQueueFamilyProperties.size()); i++)
		{
			if ((vkQueueFamilyProperties[i].queueFlags & queueFlags) && 
				((vkQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && 
				((vkQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < vkQueueFamilyProperties.size(); i++)
	{
		if (vkQueueFamilyProperties[i].queueFlags & queueFlags)
		{
			return i;
			break;
		}
	}

	return UINT32_MAX;
}

void CreateInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "VulkanTest";
	appInfo.pEngineName = "VulkanTest";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions = 
	{ 
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
	};

	const char *validationLayerNames[] = 
	{
		"VK_LAYER_LUNARG_standard_validation"
	};

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	instanceCreateInfo.enabledLayerCount = 1U;
	instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;

	check(vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance));
}

void CreateDevice()
{
	uint32_t gpuCount = 0;
	check(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, nullptr));
	assert(gpuCount > 0);

	vkPhysicalDevices.resize(gpuCount);
	check(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, vkPhysicalDevices.data()));

	vkGetPhysicalDeviceProperties(vkPhysicalDevices[0], &vkDeviceProperties);
	vkGetPhysicalDeviceFeatures(vkPhysicalDevices[0], &vkDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevices[0], &vkDeviceMemoryProperties);

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, nullptr);
	assert(queueFamilyCount > 0);
	vkQueueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, vkQueueFamilyProperties.data());

	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extCount, nullptr);
	if (extCount > 0)
	{
		std::vector<VkExtensionProperties> extensions(extCount);
		if (vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
		{
			for (auto ext : extensions)
			{
				vkSupportedExtensions.emplace_back(ext.extensionName);
			}
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	const float defaultQueuePriority(0.0f);
	VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
	{
		vkQueueFamilyIndex.Graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = vkQueueFamilyIndex.Graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.emplace_back(queueInfo);
	}
	else
	{
		vkQueueFamilyIndex.Graphics = 0U;
	}

	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
	{
		vkQueueFamilyIndex.Compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
		if (vkQueueFamilyIndex.Compute != vkQueueFamilyIndex.Graphics)
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = vkQueueFamilyIndex.Compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.emplace_back(queueInfo);
		}
	}
	else
	{
		vkQueueFamilyIndex.Compute = vkQueueFamilyIndex.Graphics;
	}

	if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
	{
		vkQueueFamilyIndex.Transfer = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
		if ((vkQueueFamilyIndex.Transfer != vkQueueFamilyIndex.Graphics) &&
			(vkQueueFamilyIndex.Transfer != vkQueueFamilyIndex.Compute))
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = vkQueueFamilyIndex.Transfer;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.emplace_back(queueInfo);
		}
	}
	else
	{
		vkQueueFamilyIndex.Transfer = vkQueueFamilyIndex.Graphics;
	}

	std::vector<const char*> deviceExtensions(vkEnabledDeviceExtensions);
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	vkEnabledFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &vkEnabledFeatures;

	if (ExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	}

	if (deviceExtensions.size() > 0)
	{
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	check(vkCreateDevice(vkPhysicalDevices[0], &deviceCreateInfo, nullptr, &vkDevice));
}

VkCommandPool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
	cmdPoolInfo.flags = createFlags;
	VkCommandPool cmdPool;
	check(vkCreateCommandPool(vkDevice, &cmdPoolInfo, nullptr, &cmdPool));

	return cmdPool;
}

VkFormat GetSupportedDepthFormat()
{
	std::vector<VkFormat> depthFormats = 
	{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevices[0], format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return format;
		}
	}

	return VK_FORMAT_UNDEFINED;
}

bool IsMouseBtnDown()
{
	ImGuiIO &io = ImGui::GetIO();
	for (uint32_t i = 0U; i < ARRAYSIZE(io.MouseDown); ++i)
	{
		if (io.MouseDown[i])
		{
			return true;
		}
	}

	return false;
}

LRESULT CALLBACK vkWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		::DestroyWindow(hWnd);
		::PostQuitMessage(0);
		break;
	case WM_SIZE:
		if ((vkPrepared) && (wParam != SIZE_MINIMIZED))
		{
			if ((vkPrepared) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
			{
				Resize(LOWORD(lParam), HIWORD(lParam));
			}
		}
		break;
	}

	if (vkPrepared)
	{
		ImGuiIO &io = ImGui::GetIO();

		int mouseBtn = -1;
		if (WM_LBUTTONDOWN == uMsg || WM_LBUTTONUP == uMsg)
		{
			mouseBtn = 0;
		}
		if (WM_RBUTTONDOWN == uMsg || WM_RBUTTONUP == uMsg)
		{
			mouseBtn = 1;
		}
		if (WM_MBUTTONDOWN == uMsg || WM_MBUTTONUP == uMsg)
		{
			mouseBtn = 2;
		}

		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			if (!IsMouseBtnDown() && ::GetCapture() == nullptr)
			{
				::SetCapture(hWnd);
			}
			io.MouseDown[mouseBtn] = true;
			return 0LL;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			io.MouseDown[mouseBtn] = false;
			if (!IsMouseBtnDown() && ::GetCapture() == hWnd)
			{
				::ReleaseCapture();
			}
			return 0LL;
		case WM_MOUSEWHEEL:
			io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
			return 0LL;
		case WM_MOUSEMOVE:
			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);
			return 0LL;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wParam < 256)
			{
				io.KeysDown[wParam] = 1;
			}
			return 0LL;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (wParam < 256)
			{
				io.KeysDown[wParam] = 0;
			}
			return 0LL;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if (wParam > 0 && wParam < 0x10000)
			{
				io.AddInputCharacter((unsigned short)wParam);
			}
			return 0LL;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void MakeWindow(const wchar_t *pTitle, uint32_t width, uint32_t height, uint32_t windowStyle)
{
	::HINSTANCE hInst = ::GetModuleHandle(nullptr);
	assert(hInst && pTitle);

	::WNDCLASSEX wndClassEx = {};
	memset(&wndClassEx, 0, sizeof(::WNDCLASSEX));
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(::WNDCLASSEX);
	wndClassEx.hbrBackground = (::HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.hCursor = ::LoadCursor(0, IDC_ARROW);
	wndClassEx.hIcon = nullptr;
	wndClassEx.hIconSm = wndClassEx.hIcon;
	wndClassEx.hInstance = hInst;
	wndClassEx.lpfnWndProc = vkWindowProc;
	wndClassEx.lpszClassName = pTitle;
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;

	if (::RegisterClassEx(&wndClassEx))
	{
		::RECT rect{ 0, 0, (long)width, (long)height };
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		vkHwnd = ::CreateWindow(pTitle, pTitle, WS_OVERLAPPEDWINDOW ^ windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInst, nullptr);
		assert(vkHwnd);

		vkWidth = width;
		vkHeight = height;

		::ShowWindow(vkHwnd, SW_SHOWDEFAULT);
		::UpdateWindow(vkHwnd);
	}
}

void Resize(uint32_t, uint32_t)
{

}

void InitSwapchain(::HINSTANCE hInst)
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = hInst;
	surfaceCreateInfo.hwnd = vkHwnd;
	check(vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &vkSurface));

	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueCount, NULL);
	assert(queueCount >= 1);

	std::vector<VkQueueFamilyProperties> queueProps(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueCount, queueProps.data());

	std::vector<VkBool32> supportsPresent(queueCount);
	for (uint32_t i = 0; i < queueCount; i++)
	{
		check(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevices[0], i, vkSurface, &supportsPresent[i]));
	}

	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < queueCount; i++)
	{
		if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			if (graphicsQueueNodeIndex == UINT32_MAX)
			{
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE)
			{
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX)
	{
		for (uint32_t i = 0; i < queueCount; ++i)
		{
			if (supportsPresent[i] == VK_TRUE)
			{
				presentQueueNodeIndex = i;
				break;
			}
		}
	}

	assert(graphicsQueueNodeIndex != UINT32_MAX);
	assert(presentQueueNodeIndex != UINT32_MAX);
	assert(graphicsQueueNodeIndex == presentQueueNodeIndex);

	vkQueueNodeIndex = graphicsQueueNodeIndex;

	uint32_t formatCount;
	check(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevices[0], vkSurface, &formatCount, nullptr));
	assert(formatCount > 0);

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	check(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevices[0], vkSurface, &formatCount, surfaceFormats.data()));

	if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		vkColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		vkColorSpace = surfaceFormats[0].colorSpace;
	}
	else
	{
		bool found_B8G8R8A8_UNORM = false;
		for (auto&& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				vkColorFormat = surfaceFormat.format;
				vkColorSpace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}

		if (!found_B8G8R8A8_UNORM)
		{
			vkColorFormat = surfaceFormats[0].format;
			vkColorSpace = surfaceFormats[0].colorSpace;
		}
	}
}

void CreateSwapchain()
{
	VkSwapchainKHR vkOldSwapchain = vkSwapchain;

	VkSurfaceCapabilitiesKHR surfCaps;
	check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevices[0], vkSurface, &surfCaps));

	uint32_t presentModeCount;
	check(vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevices[0], vkSurface, &presentModeCount, NULL));
	assert(presentModeCount > 0);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	check(vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevices[0], vkSurface, &presentModeCount, presentModes.data()));

	VkExtent2D swapchainExtent = {};
	if (surfCaps.currentExtent.width == (uint32_t)-1)
	{
		swapchainExtent.width = vkWidth;
		swapchainExtent.height = vkHeight;
	}
	else
	{
		swapchainExtent = surfCaps.currentExtent;
		vkWidth = surfCaps.currentExtent.width;
		vkHeight = surfCaps.currentExtent.height;
	}

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	if (!vkVSync)
	{
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
	if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
	{
		desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
	}

	VkSurfaceTransformFlagsKHR preTransform;
	if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		preTransform = surfCaps.currentTransform;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = 
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (auto& compositeAlphaFlag : compositeAlphaFlags) 
	{
		if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) 
		{
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = NULL;
	swapchainCI.surface = vkSurface;
	swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
	swapchainCI.imageFormat = vkColorFormat;
	swapchainCI.imageColorSpace = vkColorSpace;
	swapchainCI.imageExtent = 
	{ 
		swapchainExtent.width, 
		swapchainExtent.height 
	};
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = nullptr;
	swapchainCI.presentMode = swapchainPresentMode;
	swapchainCI.oldSwapchain = vkOldSwapchain;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.compositeAlpha = compositeAlpha;

	if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) 
	{
		swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) 
	{
		swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	check(vkCreateSwapchainKHR(vkDevice, &swapchainCI, nullptr, &vkSwapchain));

	if (vkOldSwapchain != VK_NULL_HANDLE)
	{
		for (uint32_t i = 0; i < vkImageCount; i++)
		{
			vkDestroyImageView(vkDevice, vkBackBuffers[i].vkImageView, nullptr);
		}
		vkDestroySwapchainKHR(vkDevice, vkOldSwapchain, nullptr);

		vkBackBuffers.clear();
	}
	check(vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &vkImageCount, NULL));

	std::vector<VkImage> images(vkImageCount);
	vkBackBuffers.resize(vkImageCount);
	check(vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &vkImageCount, images.data()));
	for (uint32_t i = 0; i < vkImageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {};
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = NULL;
		colorAttachmentView.format = vkColorFormat;
		colorAttachmentView.components = 
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentView.subresourceRange.baseMipLevel = 0;
		colorAttachmentView.subresourceRange.levelCount = 1;
		colorAttachmentView.subresourceRange.baseArrayLayer = 0;
		colorAttachmentView.subresourceRange.layerCount = 1;
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.flags = 0;

		vkBackBuffers[i].vkImage = images[i];

		colorAttachmentView.image = vkBackBuffers[i].vkImage;

		check(vkCreateImageView(vkDevice, &colorAttachmentView, nullptr, &vkBackBuffers[i].vkImageView));
	}
}

void CreateCommandBuffers()
{
	vkDrawCmdBuffers.resize(vkImageCount);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = vkCommandPool_S;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = vkImageCount;

	check(vkAllocateCommandBuffers(vkDevice, &commandBufferAllocateInfo, vkDrawCmdBuffers.data()));
}

void CreateFence()
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkWaitFences.resize(vkImageCount);

	for (auto& fence : vkWaitFences)
	{
		check(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &fence));
	}
}

uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < vkDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((vkDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}

	assert(0);

	return 0U;
}

void CreateDepthStencilView()
{
	vkDepthFormat = GetSupportedDepthFormat();
	VkImageCreateInfo image = {};
	image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image.pNext = NULL;
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = vkDepthFormat;
	image.extent = 
	{ 
		vkWidth, 
		vkHeight, 
		1 
	};
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	image.flags = 0;

	VkMemoryAllocateInfo mem_alloc = {};
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc.pNext = nullptr;
	mem_alloc.allocationSize = 0;
	mem_alloc.memoryTypeIndex = 0;

	VkImageViewCreateInfo depthStencilView = {};
	depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthStencilView.pNext = nullptr;
	depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilView.format = vkDepthFormat;
	depthStencilView.flags = 0;
	depthStencilView.subresourceRange = {};
	depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depthStencilView.subresourceRange.baseMipLevel = 0;
	depthStencilView.subresourceRange.levelCount = 1;
	depthStencilView.subresourceRange.baseArrayLayer = 0;
	depthStencilView.subresourceRange.layerCount = 1;

	VkMemoryRequirements memReqs;

	check(vkCreateImage(vkDevice, &image, nullptr, &vkDepthStencilView.vkImage));
	vkGetImageMemoryRequirements(vkDevice, vkDepthStencilView.vkImage, &memReqs);
	mem_alloc.allocationSize = memReqs.size;
	mem_alloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	check(vkAllocateMemory(vkDevice, &mem_alloc, nullptr, &vkDepthStencilView.vkMemory));
	check(vkBindImageMemory(vkDevice, vkDepthStencilView.vkImage, vkDepthStencilView.vkMemory, 0));

	depthStencilView.image = vkDepthStencilView.vkImage;
	check(vkCreateImageView(vkDevice, &depthStencilView, nullptr, &vkDepthStencilView.vkImageView));
}

void CreateRenderpass()
{
	std::array<VkAttachmentDescription, 2> attachments = {};
	attachments[0].format = vkColorFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Depth attachment
	attachments[1].format = vkDepthFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.pDepthStencilAttachment = &depthReference;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	subpassDescription.pResolveAttachments = nullptr;

	// Subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	check(vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &vkRenderpass));
}

void CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	check(vkCreatePipelineCache(vkDevice, &pipelineCacheCreateInfo, nullptr, &vkPipelineCache));
}

void CreateFrameBuffers()
{
	VkImageView attachments[2];
	attachments[1] = vkDepthStencilView.vkImageView;

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.renderPass = vkRenderpass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = vkWidth;
	frameBufferCreateInfo.height = vkHeight;
	frameBufferCreateInfo.layers = 1;
	vkFrameBuffers.resize(vkImageCount);
	for (uint32_t i = 0; i < vkFrameBuffers.size(); i++)
	{
		attachments[0] = vkBackBuffers[i].vkImageView;
		check(vkCreateFramebuffer(vkDevice, &frameBufferCreateInfo, nullptr, &vkFrameBuffers[i]));
	}
}

VkShaderModule CreateShader(const char *fileName)
{
	AssetFile shader(fileName);
	auto pCode = shader.Load();

	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo{};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.codeSize = shader.GetSize();
	moduleCreateInfo.pCode = (uint32_t *)&pCode;

	check(vkCreateShaderModule(vkDevice, &moduleCreateInfo, NULL, &shaderModule));

	return shaderModule;
}

void CreateShaders()
{
	VkPipelineShaderStageCreateInfo shaderStage = {};
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStage.module = CreateShader("UIVertexShader.bin");
	shaderStage.pName = "main"; 

	vkShaderStates.emplace_back(shaderStage);

	shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStage.module = CreateShader("UIFragmentShader.bin");
	vkShaderStates.emplace_back(shaderStage);
}

VkMemoryBuffer CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, void *data)
{
	VkMemoryBuffer buffer;

	VkBufferCreateInfo bufCreateInfo{};
	bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufCreateInfo.usage = usageFlags;
	bufCreateInfo.size = size;
	check(vkCreateBuffer(vkDevice, &bufCreateInfo, nullptr, &buffer.Buffer));

	VkMemoryRequirements memReqs;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkGetBufferMemoryRequirements(vkDevice, buffer.Buffer, &memReqs);
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
	check(vkAllocateMemory(vkDevice, &memAllocInfo, nullptr, &buffer.Memory));

	buffer.Alignment = memReqs.alignment;
	buffer.Size = memAllocInfo.allocationSize;
	buffer.Usage = usageFlags;
	buffer.Property = memoryPropertyFlags;

	if (data != nullptr)
	{
		buffer.Map();
		buffer.Copy(data, size);
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			buffer.Flush();
		}

		buffer.UnMap();
	}

	buffer.SetDescriptor();

	buffer.Bind();

	return buffer;
}

VkCommandBuffer AllocCommandBuffer(VkCommandBufferLevel level, uint32_t bufferCount, bool begin)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = vkCommandPool;
	commandBufferAllocateInfo.level = level;
	commandBufferAllocateInfo.commandBufferCount = bufferCount;

	VkCommandBuffer cmdBuffer;
	check(vkAllocateCommandBuffers(vkDevice, &commandBufferAllocateInfo, &cmdBuffer));

	if (begin)
	{
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		check(vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo));
	}

	return cmdBuffer;
}

void SetImageLayout(
	VkCommandBuffer cmdbuffer,
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		imageMemoryBarrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		break;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		break;
	}

	vkCmdPipelineBarrier(
		cmdbuffer,
		srcStageMask,
		dstStageMask,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}

#define DEFAULT_FENCE_TIMEOUT 100000000000
void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free)
{
	if (commandBuffer == VK_NULL_HANDLE)
	{
		return;
	}

	check(vkEndCommandBuffer(commandBuffer));

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0U;
	VkFence fence;
	check(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &fence));

	check(vkQueueSubmit(queue, 1, &submitInfo, fence));
	check(vkWaitForFences(vkDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

	vkDestroyFence(vkDevice, fence, nullptr);

	if (free)
	{
		vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, &commandBuffer);
	}
}

void InitImGUI()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';
	io.RenderDrawListsFn = nullptr;
	io.ImeWindowHandle = vkHwnd;

	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.extent.width = texWidth;
	imageInfo.extent.height = texHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	check(vkCreateImage(vkDevice, &imageInfo, nullptr, &vkImageFont));
	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(vkDevice, vkImageFont, &memReqs);
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	check(vkAllocateMemory(vkDevice, &memAllocInfo, nullptr, &vkMemoryFontImage));
	check(vkBindImageMemory(vkDevice, vkImageFont, vkMemoryFontImage, 0));

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = vkImageFont;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.layerCount = 1;
	check(vkCreateImageView(vkDevice, &viewInfo, nullptr, &vkImageViewFont));

	VkMemoryBuffer stagingBuffer = CreateBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		uploadSize);

	stagingBuffer.Map();
	stagingBuffer.Copy(fontData, uploadSize);
	stagingBuffer.UnMap();

	VkCommandBuffer copyCmd = AllocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1U, true);

	SetImageLayout(
		copyCmd,
		vkImageFont,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT);

	VkBufferImageCopy bufferCopyRegion = {};
	bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = texWidth;
	bufferCopyRegion.imageExtent.height = texHeight;
	bufferCopyRegion.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(
		copyCmd,
		stagingBuffer.Buffer,
		vkImageFont,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&bufferCopyRegion
	);

	SetImageLayout(
		copyCmd,
		vkImageFont,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

	FlushCommandBuffer(copyCmd, vkQueue, true);

	stagingBuffer.Destroy();

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	check(vkCreateSampler(vkDevice, &samplerInfo, nullptr, &vkSampler));

	VkDescriptorPoolSize descriptorPoolSize;
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize.descriptorCount = 1U;

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = 1U;
	descriptorPoolInfo.pPoolSizes = &descriptorPoolSize;
	descriptorPoolInfo.maxSets = 2U;
	check(vkCreateDescriptorPool(vkDevice, &descriptorPoolInfo, nullptr, &vkDescriptorPool));

	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	setLayoutBinding.binding = 0U;
	setLayoutBinding.descriptorCount = 1U;
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = &setLayoutBinding;
	descriptorSetLayoutCreateInfo.bindingCount = 1U;
	check(vkCreateDescriptorSetLayout(vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout));

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
	descriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayout;
	descriptorSetAllocateInfo.descriptorSetCount = 1U;
	check(vkAllocateDescriptorSets(vkDevice, &descriptorSetAllocateInfo, &vkDescriptorSet));
	VkDescriptorImageInfo descriptorImageInfo{};
	descriptorImageInfo.sampler = vkSampler;
	descriptorImageInfo.imageView = vkImageViewFont;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = vkDescriptorSet;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.dstBinding = 0U;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.descriptorCount = 1U;
	vkUpdateDescriptorSets(vkDevice, 1U, &writeDescriptorSet, 0, nullptr);

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0U;
	pushConstantRange.size = sizeof(VkPushConstBlock);
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1U;
	pipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
	check(vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout));

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineInputAssemblyStateCreateInfo.flags = 0U;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.flags = 0U;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	VkPipelineColorBlendAttachmentState blendAttachmentState{};
	blendAttachmentState.blendEnable = VK_TRUE;
	blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1U;
	pipelineColorBlendStateCreateInfo.pAttachments = &blendAttachmentState;

	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;
	pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;
	pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.viewportCount = 1U;
	pipelineViewportStateCreateInfo.scissorCount = 1U;
	pipelineViewportStateCreateInfo.flags = 0U;

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineMultisampleStateCreateInfo.flags = 0U;

	std::vector<VkDynamicState> dynamicStateEnables = 
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
	pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	pipelineDynamicStateCreateInfo.flags = 0U;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = vkPipelineLayout;
	pipelineCreateInfo.renderPass = vkRenderpass;
	pipelineCreateInfo.flags = 0U;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
	pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(vkShaderStates.size());
	pipelineCreateInfo.pStages = vkShaderStates.data();
	pipelineCreateInfo.subpass = 0U;

	VkVertexInputBindingDescription vInputBindDescription{};
	vInputBindDescription.binding = 0U;
	vInputBindDescription.stride = sizeof(ImDrawVert);
	vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vInputAttribDescription0{};
	vInputAttribDescription0.location = 0U;
	vInputAttribDescription0.binding = 0U;
	vInputAttribDescription0.format = VK_FORMAT_R32G32_SFLOAT;
	vInputAttribDescription0.offset = offsetof(ImDrawVert, pos);
	VkVertexInputAttributeDescription vInputAttribDescription1{};
	vInputAttribDescription1.location = 1U;
	vInputAttribDescription1.binding = 0U;
	vInputAttribDescription1.format = VK_FORMAT_R32G32_SFLOAT;
	vInputAttribDescription1.offset = offsetof(ImDrawVert, uv);
	VkVertexInputAttributeDescription vInputAttribDescription2{};
	vInputAttribDescription2.location = 2U;
	vInputAttribDescription2.binding = 0U;
	vInputAttribDescription2.format = VK_FORMAT_R8G8B8A8_UNORM;
	vInputAttribDescription2.offset = offsetof(ImDrawVert, col);
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = 
	{
		vInputAttribDescription0,
		vInputAttribDescription1,
		vInputAttribDescription2
	};
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1U;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vInputBindDescription;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributes.data();

	pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;

	check(vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &pipelineCreateInfo, nullptr, &vkPipeline));
}

void BuildCommandBuffers()
{
	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.0f, 0.125f, 0.3f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = vkRenderpass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = vkWidth;
	renderPassBeginInfo.renderArea.extent.height = vkHeight;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	for (int32_t i = 0; i < vkDrawCmdBuffers.size(); ++i)
	{
		// Set target frame buffer
		renderPassBeginInfo.framebuffer = vkFrameBuffers[i];

		check(vkBeginCommandBuffer(vkDrawCmdBuffers[i], &cmdBufferBeginInfo));

		vkCmdBeginRenderPass(vkDrawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		const VkViewport viewport = 
		{
			0.0f, 0.0f, (float)vkWidth, (float)vkHeight, 0.0f, 1.0f 
		};
		const VkRect2D scissor = 
		{
			0, 0, vkWidth, vkHeight,
		};
		vkCmdSetViewport(vkDrawCmdBuffers[i], 0, 1, &viewport);
		vkCmdSetScissor(vkDrawCmdBuffers[i], 0, 1, &scissor);

		DrawUI(vkDrawCmdBuffers[i]);

		vkCmdEndRenderPass(vkDrawCmdBuffers[i]);

		check(vkEndCommandBuffer(vkDrawCmdBuffers[i]));
	}
}

VkPushConstBlock vkPushConstBlock;

void DrawUI(VkCommandBuffer commandBuffer)
{
	ImDrawData* imDrawData = ImGui::GetDrawData();
	int32_t vertexOffset = 0;
	int32_t indexOffset = 0;

	if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &vkDescriptorSet, 0, NULL);

	vkPushConstBlock.Scale = Vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	vkPushConstBlock.Translate = Vec2(-1.0f, -1.0f);
	vkCmdPushConstants(commandBuffer, vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkPushConstBlock), &vkPushConstBlock);

	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkVertexBuffer.Buffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, vkIndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);

	for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
	{
		const ImDrawList* cmd_list = imDrawData->CmdLists[i];
		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
			VkRect2D scissorRect;
			scissorRect.offset.x = std::max<int32_t>((int32_t)(pcmd->ClipRect.x), 0);
			scissorRect.offset.y = std::max<int32_t>((int32_t)(pcmd->ClipRect.y), 0);
			scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
			scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
			vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
			indexOffset += pcmd->ElemCount;
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
	}
}

void UpdateUI()
{
	ImGuiIO& io = ImGui::GetIO();
	::HWND hWnd = (::HWND)io.ImeWindowHandle;

	::RECT rect;
	::GetClientRect(hWnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;

	if (io.MouseDrawCursor)
	{
		::SetCursor(nullptr);
	}

	ImVec2 pos(10.0f, 10.0f);
	ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

	ImGui::NewFrame();

	ImGui::Begin("VulkanTest");

	ImGui::Text("VulkanTest");

	ImGui::End();

	ImGui::Render();

	if (UpdateUIBuffer())
	{
		BuildCommandBuffers();
	}
}

void Frame()
{
	check(vkAcquireNextImageKHR(vkDevice, vkSwapchain, UINT64_MAX, vkSemaphorePresent, (VkFence)nullptr, &vkImageIndex));

	vkSubmitInfo.commandBufferCount = 1;
	vkSubmitInfo.pCommandBuffers = &vkDrawCmdBuffers[vkImageIndex];

	check(vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vkSwapchain;
	presentInfo.pImageIndices = &vkImageIndex;
	if (vkSemaphoreComplete != VK_NULL_HANDLE)
	{
		presentInfo.pWaitSemaphores = &vkSemaphoreComplete;
		presentInfo.waitSemaphoreCount = 1;
	}
	check(vkQueuePresentKHR(vkQueue, &presentInfo));

	check(vkQueueWaitIdle(vkQueue));

	UpdateUI();
}

bool UpdateUIBuffer()
{
	ImDrawData* imDrawData = ImGui::GetDrawData();
	bool updateCmdBuffers = false;

	if (!imDrawData) 
	{ 
		return false; 
	};

	VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
	VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if ((vertexBufferSize == 0) || (indexBufferSize == 0)) 
	{
		return false;
	}

	if ((vkVertexBuffer.Buffer == VK_NULL_HANDLE) || (vkVertexCount != (uint32_t)imDrawData->TotalVtxCount)) 
	{
		vkVertexBuffer.UnMap();
		vkVertexBuffer.Destroy();
		vkVertexBuffer = CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBufferSize);
		vkVertexCount = imDrawData->TotalVtxCount;
		vkVertexBuffer.UnMap();
		vkVertexBuffer.Map();
		updateCmdBuffers = true;
	}

	///VkDeviceSize indexSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);
	if ((vkIndexBuffer.Buffer == VK_NULL_HANDLE) || (vkIndexCount < (uint32_t)imDrawData->TotalIdxCount)) 
	{
		vkIndexBuffer.UnMap();
		vkIndexBuffer.Destroy();
		vkIndexBuffer = CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBufferSize);
		vkIndexCount = imDrawData->TotalIdxCount;
		vkIndexBuffer.Map();
		updateCmdBuffers = true;
	}

	ImDrawVert* vtxDst = (ImDrawVert*)vkVertexBuffer.MappedMemory;
	ImDrawIdx* idxDst = (ImDrawIdx*)vkIndexBuffer.MappedMemory;

	for (int n = 0; n < imDrawData->CmdListsCount; n++) 
	{
		const ImDrawList* cmd_list = imDrawData->CmdLists[n];
		memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxDst += cmd_list->VtxBuffer.Size;
		idxDst += cmd_list->IdxBuffer.Size;
	}

	vkVertexBuffer.Flush();
	vkIndexBuffer.Flush();

	return updateCmdBuffers;
}

void RenderLoop()
{
	MSG msg;
	bool quitMessageReceived = false;
	while (!quitMessageReceived) 
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) 
			{
				quitMessageReceived = true;
				break;
			}
		}

		Frame();
	}

	check(vkDeviceWaitIdle(vkDevice));
}