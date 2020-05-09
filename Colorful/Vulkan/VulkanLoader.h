#pragma once

#include "Colorful/Public/GfxEngine.h"

#if defined(UsingVkLoader)

#define vkFunctionDeclare(func) extern PFN_##func func;
#define vkFunctionDefinition(func) PFN_##func func = nullptr;
#define vkFunctionReset(func) func = nullptr;

#define vkGlobalFunctionTable(action)              \
	action(vkCreateInstance)                       \
	action(vkGetDeviceProcAddr)                    \
	action(vkGetInstanceProcAddr)                  \
	action(vkEnumerateDeviceLayerProperties)       \
	action(vkEnumerateInstanceLayerProperties)     \
	action(vkEnumerateInstanceExtensionProperties) 

#define vkInstanceFunctionTable_General(action)       \
	action(vkCreateDevice)                            \
	action(vkDestroyInstance)                         \
	action(vkDestroySurfaceKHR)                       \
	action(vkSetHdrMetadataEXT)                       \
	action(vkEnumeratePhysicalDevices)                \
	action(vkGetPhysicalDeviceFeatures)               \
	action(vkGetPhysicalDeviceProperties)             \
	action(vkGetPhysicalDeviceFormatProperties)       \
	action(vkGetPhysicalDeviceMemoryProperties)       \
	action(vkEnumerateDeviceExtensionProperties)      \
	action(vkGetPhysicalDeviceSurfaceSupportKHR)      \
	action(vkGetPhysicalDeviceSurfaceFormatsKHR)      \
	action(vkGetPhysicalDeviceQueueFamilyProperties)  \
	action(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) \
	action(vkGetPhysicalDeviceSurfacePresentModesKHR) 

#define vkDeviceFunctionTable(action)    \
	action(vkCmdDraw)                    \
	action(vkSetEvent)                   \
	action(vkMapMemory)                  \
	action(vkFreeMemory)                 \
	action(vkResetEvent)                 \
	action(vkResetFences)                \
	action(vkUnmapMemory)                \
	action(vkCreateImage)                \
	action(vkCreateEvent)                \
	action(vkCreateFence)                \
	action(vkQueueSubmit)                \
	action(vkDestroyEvent)               \
	action(vkCreateBuffer)               \
	action(vkDestroyImage)               \
	action(vkDestroyFence)               \
	action(vkQueueWaitIdle)              \
	action(vkDestroyDevice)              \
	action(vkCmdSetScissor)              \
	action(vkDestroyBuffer)              \
	action(vkWaitForFences)              \
	action(vkCmdCopyBuffer)              \
	action(vkCreateSampler)              \
	action(vkCmdDrawIndexed)             \
	action(vkAllocateMemory)             \
	action(vkCmdSetViewport)             \
	action(vkDestroySampler)             \
	action(vkGetFenceStatus)             \
	action(vkGetDeviceQueue)             \
	action(vkDeviceWaitIdle)             \
	action(vkGetEventStatus)             \
	action(vkBindImageMemory)            \
	action(vkDestroyPipeline)            \
	action(vkQueuePresentKHR)            \
	action(vkCreateSemaphore)            \
	action(vkCreateImageView)            \
	action(vkCmdBindPipeline)            \
	action(vkResetCommandPool)           \
	action(vkBindBufferMemory)           \
	action(vkDestroyImageView)           \
	action(vkCmdEndRenderPass)           \
	action(vkCreateRenderPass)           \
	action(vkEndCommandBuffer)           \
	action(vkDestroySemaphore)           \
	action(vkDestroyRenderPass)          \
	action(vkCreateCommandPool)          \
	action(vkCreateFramebuffer)          \
	action(vkCmdExecuteCommands)         \
	action(vkDestroyFramebuffer)         \
	action(vkCmdBeginRenderPass)         \
	action(vkResetCommandBuffer)         \
	action(vkBeginCommandBuffer)         \
	action(vkCmdPipelineBarrier)         \
	action(vkCmdClearColorImage)         \
	action(vkFreeCommandBuffers)         \
	action(vkDestroyCommandPool)         \
	action(vkCreateSwapchainKHR)         \
	action(vkCreateShaderModule)         \
	action(vkCmdBindIndexBuffer)         \
	action(vkResetDescriptorPool)        \
	action(vkAcquireNextImageKHR)        \
	action(vkDestroySwapchainKHR)        \
	action(vkDestroyShaderModule)        \
	action(vkCreatePipelineCache)        \
	action(vkCmdBindVertexBuffers)       \
	action(vkCmdCopyBufferToImage)       \
	action(vkCreateDescriptorPool)       \
	action(vkUpdateDescriptorSets)       \
	action(vkCreatePipelineLayout)       \
	action(vkDestroyPipelineCache)       \
	action(vkCmdBindDescriptorSets)      \
	action(vkDestroyDescriptorPool)      \
	action(vkDestroyPipelineLayout)      \
	action(vkGetSwapchainImagesKHR)      \
	action(vkAllocateCommandBuffers)     \
	action(vkAllocateDescriptorSets)     \
	action(vkCreateGraphicsPipelines)    \
	action(vkFlushMappedMemoryRanges)    \
	action(vkCreateDescriptorSetLayout)  \
	action(vkGetImageMemoryRequirements) \
	action(vkDestroyDescriptorSetLayout) \
	action(vkGetBufferMemoryRequirements)    

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#define vkInstanceFunctionTable_Platform(action) \
	action(vkCreateWin32SurfaceKHR)
#else
#define vkInstanceFunctionTable_Platform(action)
#endif
	
#define vkInstanceFunctionTable(action)     \
	vkInstanceFunctionTable_General(action) \
	vkInstanceFunctionTable_Platform(action) 

#define vkFunctionTable(action)     \
	vkGlobalFunctionTable(action)   \
	vkInstanceFunctionTable(action) \
	vkDeviceFunctionTable(action)

#define vkFunctionTableDeclare vkFunctionTable(vkFunctionDeclare)
#define vkFunctionTableDefinition vkFunctionTable(vkFunctionDefinition)
#define vkFunctionTableReset vkFunctionTable(vkFunctionReset)

vkFunctionTableDeclare

class VulkanLoader
{
public:
	static void initializeGlobalFunctionTable();
	static void initializeInstanceFunctionTable(VkInstance instance);
	static void initializeDeviceFunctionTable(VkDevice device);

	static void finalize();
};

#endif
