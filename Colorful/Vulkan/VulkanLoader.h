#pragma once

#include "Colorful/Public/RInterface.h"

#if defined(UsingVkLoader)

#define vkFunctionDeclare(func) extern PFN_##func func;
#define vkFunctionDefinition(func) PFN_##func func = nullptr;
#define vkFunctionReset(func) vkFunctionDefinition(func)

#define vkGlobalFunctionTable(action)              \
	action(vkCreateInstance)                       \
	action(vkGetInstanceProcAddr)                  \
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
	action(vkGetPhysicalDeviceMemoryProperties)       \
	action(vkEnumerateDeviceExtensionProperties)      \
	action(vkGetPhysicalDeviceSurfaceSupportKHR)      \
	action(vkGetPhysicalDeviceSurfaceFormatsKHR)      \
	action(vkGetPhysicalDeviceQueueFamilyProperties)  \
	action(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) \
	action(vkGetPhysicalDeviceSurfacePresentModesKHR) 

#define vkDeviceFunctionTable(action)     \
	action(vkCmdDraw)                     \
	action(vkMapMemory)                   \
	action(vkFreeMemory)                  \
	action(vkResetFences)                 \
	action(vkUnmapMemory)                 \
	action(vkCreateImage)                 \
	action(vkCreateFence)                 \
	action(vkQueueSubmit)                 \
	action(vkCreateBuffer)                \
	action(vkDestroyImage)                \
	action(vkDestroyFence)                \
	action(vkDestroyDevice)               \
	action(vkCmdSetScissor)               \
	action(vkDestroyBuffer)               \
	action(vkWaitForFences)               \
	action(vkCmdCopyBuffer)               \
	action(vkCreateSampler)               \
	action(vkAllocateMemory)              \
	action(vkCmdSetViewport)              \
	action(vkDestroySampler)              \
	action(vkGetDeviceQueue)              \
	action(vkDeviceWaitIdle)              \
	action(vkBindImageMemory)             \
	action(vkDestroyPipeline)             \
	action(vkQueuePresentKHR)             \
	action(vkCreateSemaphore)             \
	action(vkCreateImageView)             \
	action(vkCmdBindPipeline)             \
	action(vkBindBufferMemory)            \
	action(vkDestroyImageView)            \
	action(vkCmdEndRenderPass)            \
	action(vkCreateRenderPass)            \
	action(vkEndCommandBuffer)            \
	action(vkDestroySemaphore)            \
	action(vkDestroyRenderPass)           \
	action(vkGetDeviceProcAddr)           \
	action(vkCreateCommandPool)           \
	action(vkCreateFramebuffer)           \
	action(vkDestroyFramebuffer)          \
	action(vkCmdBeginRenderPass)          \
	action(vkBeginCommandBuffer)          \
	action(vkCmdPipelineBarrier)          \
	action(vkCmdClearColorImage)          \
	action(vkFreeCommandBuffers)          \
	action(vkDestroyCommandPool)          \
	action(vkCreateSwapchainKHR)          \
	action(vkCreateShaderModule)          \
	action(vkAcquireNextImageKHR)         \
	action(vkDestroySwapchainKHR)         \
	action(vkDestroyShaderModule)         \
	action(vkCmdBindVertexBuffers)        \
	action(vkCmdCopyBufferToImage)        \
	action(vkCreateDescriptorPool)        \
	action(vkUpdateDescriptorSets)        \
	action(vkCreatePipelineLayout)        \
	action(vkCmdBindDescriptorSets)       \
	action(vkDestroyDescriptorPool)       \
	action(vkDestroyPipelineLayout)       \
	action(vkGetSwapchainImagesKHR)       \
	action(vkAllocateCommandBuffers)      \
	action(vkAllocateDescriptorSets)      \
	action(vkCreateGraphicsPipelines)     \
	action(vkFlushMappedMemoryRanges)     \
	action(vkCreateDescriptorSetLayout)   \
	action(vkGetImageMemoryRequirements)  \
	action(vkDestroyDescriptorSetLayout)  \
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

#endif
