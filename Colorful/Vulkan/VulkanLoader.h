#pragma once

#include "Colorful/Gfx/GfxRenderer.h"

#define USE_VK_LOADER

#if defined(USE_VK_LOADER)

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

NAMESPACE_START(Gfx)

#define VK_FUNC_DECLARE(Func) extern PFN_##Func Func;
#define VK_FUNC_DEFINITION(Func) PFN_##Func Func = nullptr;
#define VK_FUNC_RESET(Func) Func = nullptr;

#define VK_GLOBAL_FUNC_TABLE(Action)               \
	Action(vkCreateInstance)                       \
	Action(vkGetDeviceProcAddr)                    \
	Action(vkGetInstanceProcAddr)                  \
	Action(vkEnumerateDeviceLayerProperties)       \
	Action(vkEnumerateInstanceLayerProperties)     \
	Action(vkEnumerateInstanceExtensionProperties) 

#define VK_INSTANCE_FUNC_TABLE_GENERAL(Action)        \
	Action(vkCreateDevice)                            \
	Action(vkDestroyInstance)                         \
	Action(vkDestroySurfaceKHR)                       \
	Action(vkSetHdrMetadataEXT)                       \
	Action(vkCmdDebugMarkerEndEXT)                    \
	Action(vkCmdDebugMarkerBeginEXT)                  \
	Action(vkCmdDebugMarkerInsertEXT)                 \
	Action(vkEnumeratePhysicalDevices)                \
	Action(vkGetPhysicalDeviceFeatures)               \
	Action(vkDebugMarkerSetObjectNameEXT)             \
	Action(vkGetPhysicalDeviceProperties)             \
	Action(vkGetPhysicalDeviceFormatProperties)       \
	Action(vkGetPhysicalDeviceMemoryProperties)       \
	Action(vkEnumerateDeviceExtensionProperties)      \
	Action(vkGetPhysicalDeviceSurfaceSupportKHR)      \
	Action(vkGetPhysicalDeviceSurfaceFormatsKHR)      \
	Action(vkGetPhysicalDeviceQueueFamilyProperties)  \
	Action(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) \
	Action(vkGetPhysicalDeviceSurfacePresentModesKHR) 

#define VK_DEVICE_FUNC_TABLE(Action)       \
	Action(vkCmdDraw)                      \
	Action(vkSetEvent)                     \
	Action(vkMapMemory)                    \
	Action(vkFreeMemory)                   \
	Action(vkResetEvent)                   \
	Action(vkResetFences)                  \
	Action(vkUnmapMemory)                  \
	Action(vkCreateImage)                  \
	Action(vkCreateEvent)                  \
	Action(vkCreateFence)                  \
	Action(vkQueueSubmit)                  \
	Action(vkDestroyEvent)                 \
	Action(vkCreateBuffer)                 \
	Action(vkDestroyImage)                 \
	Action(vkDestroyFence)                 \
	Action(vkQueueWaitIdle)                \
	Action(vkDestroyDevice)                \
	Action(vkCmdSetScissor)                \
	Action(vkDestroyBuffer)                \
	Action(vkWaitForFences)                \
	Action(vkCmdCopyBuffer)                \
	Action(vkCreateSampler)                \
	Action(vkGetDeviceQueue)               \
	Action(vkCmdDrawIndexed)               \
	Action(vkAllocateMemory)               \
	Action(vkCmdSetViewport)               \
	Action(vkDestroySampler)               \
	Action(vkGetFenceStatus)               \
	Action(vkDeviceWaitIdle)               \
	Action(vkGetEventStatus)               \
	Action(vkBindImageMemory)              \
	Action(vkDestroyPipeline)              \
	Action(vkQueuePresentKHR)              \
	Action(vkCreateSemaphore)              \
	Action(vkCreateImageView)              \
	Action(vkCmdBindPipeline)              \
	Action(vkResetCommandPool)             \
	Action(vkBindBufferMemory)             \
	Action(vkDestroyImageView)             \
	Action(vkCmdEndRenderPass)             \
	Action(vkCreateRenderPass)             \
	Action(vkEndCommandBuffer)             \
	Action(vkDestroySemaphore)             \
	Action(vkDestroyRenderPass)            \
	Action(vkCreateCommandPool)            \
	Action(vkCreateFramebuffer)            \
	Action(vkCmdExecuteCommands)           \
	Action(vkDestroyFramebuffer)           \
	Action(vkCmdBeginRenderPass)           \
	Action(vkResetCommandBuffer)           \
	Action(vkBeginCommandBuffer)           \
	Action(vkCmdPipelineBarrier)           \
	Action(vkCmdClearColorImage)           \
	Action(vkFreeCommandBuffers)           \
	Action(vkDestroyCommandPool)           \
	Action(vkCreateSwapchainKHR)           \
	Action(vkCreateShaderModule)           \
	Action(vkCmdBindIndexBuffer)           \
	Action(vkResetDescriptorPool)          \
	Action(vkAcquireNextImageKHR)          \
	Action(vkDestroySwapchainKHR)          \
	Action(vkDestroyShaderModule)          \
	Action(vkCreatePipelineCache)          \
	Action(vkCmdBindVertexBuffers)         \
	Action(vkCmdCopyBufferToImage)         \
	Action(vkCreateDescriptorPool)         \
	Action(vkUpdateDescriptorSets)         \
	Action(vkCreatePipelineLayout)         \
	Action(vkDestroyPipelineCache)         \
	Action(vkCmdBindDescriptorSets)        \
	Action(vkDestroyDescriptorPool)        \
	Action(vkDestroyPipelineLayout)        \
	Action(vkGetSwapchainImagesKHR)        \
	Action(vkAllocateCommandBuffers)       \
	Action(vkAllocateDescriptorSets)       \
	Action(vkCreateGraphicsPipelines)      \
	Action(vkFlushMappedMemoryRanges)      \
	Action(vkCreateDescriptorSetLayout)    \
	Action(vkGetImageMemoryRequirements)   \
	Action(vkDestroyDescriptorSetLayout)   \
	Action(vkGetBufferMemoryRequirements)  \
	Action(vkInvalidateMappedMemoryRanges) \

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_INSTANCE_FUNC_TABLE_PLATFORM(Action) \
	Action(vkCreateWin32SurfaceKHR)
#else
	#error Unknown platform!
#endif
	
#define VK_INSTANCE_FUNC_TABLE(Action)     \
	VK_INSTANCE_FUNC_TABLE_GENERAL(Action) \
	VK_INSTANCE_FUNC_TABLE_PLATFORM(Action) 

#define VK_FUNC_TABLE(Action)      \
	VK_GLOBAL_FUNC_TABLE(Action)   \
	VK_INSTANCE_FUNC_TABLE(Action) \
	VK_DEVICE_FUNC_TABLE(Action)

#define VK_FUNC_TABLE_DECLARE    VK_FUNC_TABLE(VK_FUNC_DECLARE)
#define VK_FUNC_TABLE_DEFINITION VK_FUNC_TABLE(VK_FUNC_DEFINITION)
#define VK_FUNC_TABLE_RESET      VK_FUNC_TABLE(VK_FUNC_RESET)

VK_FUNC_TABLE_DECLARE

class VulkanLoader
{
public:
	static void loadGlobalFuncs();
	static void loadInstanceFuncs(VkInstance instance);
	static void loadDeviceFuncs(VkDevice device);

	static void free();
protected:
private:
	static System::DynamicLibraryPtr s_DynamicLib;
};

class VulkanErrorLog
{
public:
	static const char8_t* const getErrorMessage(VkResult result);
};

#define VERIFY_VK(Func)                                                                       \
{                                                                                             \
	VkResult result = (Func);                                                                 \
	if (result != VK_SUCCESS)                                                                 \
	{                                                                                         \
		LOG_ERROR("Failed to invoke VulkanAPI, %s", VulkanErrorLog::getErrorMessage(result)); \
	}                                                                                         \
}

#define VK_MEMORY_ALLOCATOR VK_NULL_HANDLE

template<class T>
using VkObject = GfxObject<T>;

class VkResource
{
protected:
	friend class VulkanRenderer;
	static std::shared_ptr<class VulkanDevice> s_Device;
};

template<class T>
class VkDeviceObject : public VkObject<T>, public VkResource
{
public:
	using VkObject<T>::VkObject;
};

NAMESPACE_END(Gfx)

#else
	#include <vulkan/vulkan.h>
#endif
