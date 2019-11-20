#include "VulkanEngine.h"

#define vkLoadFunction(Function)                                       \
	extern PFN_##Function Function;                                    \
	Function = (PFN_##Function)::GetProcAddress(s_Library, #Function); \
	verifyWin(Function);

#if 0
void vkEngine::loadLibrary()
{
	/// #define VK_NO_PROTOTYPES
	s_Library = LoadLibraryA("vulkan-1.dll");
	verifyWin(s_Library);

	/// Global Level
	vkLoadFunction(vkCreateInstance);
	vkLoadFunction(vkEnumerateInstanceExtensionProperties);

	/// Instance Level
	vkLoadFunction(vkEnumeratePhysicalDevices);
	vkLoadFunction(vkGetPhysicalDeviceProperties);
	vkLoadFunction(vkGetPhysicalDeviceFeatures);
	vkLoadFunction(vkGetPhysicalDeviceQueueFamilyProperties);
	vkLoadFunction(vkCreateDevice);
	vkLoadFunction(vkGetDeviceProcAddr);
	vkLoadFunction(vkDestroyInstance);
	vkLoadFunction(vkEnumerateDeviceExtensionProperties);
	vkLoadFunction(vkGetPhysicalDeviceSurfaceSupportKHR);
	vkLoadFunction(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
	vkLoadFunction(vkGetPhysicalDeviceSurfaceFormatsKHR);
	vkLoadFunction(vkGetPhysicalDeviceSurfacePresentModesKHR);
	vkLoadFunction(vkDestroySurfaceKHR);
	vkLoadFunction(vkCreateWin32SurfaceKHR);
	vkLoadFunction(vkGetPhysicalDeviceMemoryProperties);

	/// Device Level
	vkLoadFunction(vkGetDeviceQueue);
	vkLoadFunction(vkDeviceWaitIdle);
	vkLoadFunction(vkDestroyDevice);
	vkLoadFunction(vkCreateSemaphore);
	vkLoadFunction(vkCreateCommandPool);
	vkLoadFunction(vkAllocateCommandBuffers);
	vkLoadFunction(vkBeginCommandBuffer);
	vkLoadFunction(vkCmdPipelineBarrier);
	vkLoadFunction(vkCmdClearColorImage);
	vkLoadFunction(vkEndCommandBuffer);
	vkLoadFunction(vkQueueSubmit);
	vkLoadFunction(vkFreeCommandBuffers);
	vkLoadFunction(vkDestroyCommandPool);
	vkLoadFunction(vkDestroySemaphore);
	vkLoadFunction(vkCreateSwapchainKHR);
	vkLoadFunction(vkGetSwapchainImagesKHR);
	vkLoadFunction(vkAcquireNextImageKHR);
	vkLoadFunction(vkQueuePresentKHR);
	vkLoadFunction(vkDestroySwapchainKHR);
	vkLoadFunction(vkCreateImageView);
	vkLoadFunction(vkCreateRenderPass);
	vkLoadFunction(vkCreateFramebuffer);
	vkLoadFunction(vkCreateShaderModule);
	vkLoadFunction(vkCreatePipelineLayout);
	vkLoadFunction(vkCreateGraphicsPipelines);
	vkLoadFunction(vkCmdBeginRenderPass);
	vkLoadFunction(vkCmdBindPipeline);
	vkLoadFunction(vkCmdDraw);
	vkLoadFunction(vkCmdEndRenderPass);
	vkLoadFunction(vkDestroyShaderModule);
	vkLoadFunction(vkDestroyPipelineLayout);
	vkLoadFunction(vkDestroyPipeline);
	vkLoadFunction(vkDestroyRenderPass);
	vkLoadFunction(vkDestroyFramebuffer);
	vkLoadFunction(vkDestroyImageView);
	vkLoadFunction(vkCreateFence);
	vkLoadFunction(vkCreateBuffer);
	vkLoadFunction(vkGetBufferMemoryRequirements);
	vkLoadFunction(vkAllocateMemory);
	vkLoadFunction(vkBindBufferMemory);
	vkLoadFunction(vkMapMemory);
	vkLoadFunction(vkFlushMappedMemoryRanges);
	vkLoadFunction(vkUnmapMemory);
	vkLoadFunction(vkCmdSetViewport);
	vkLoadFunction(vkCmdSetScissor);
	vkLoadFunction(vkCmdBindVertexBuffers);
	vkLoadFunction(vkWaitForFences);
	vkLoadFunction(vkResetFences);
	vkLoadFunction(vkFreeMemory);
	vkLoadFunction(vkDestroyBuffer);
	vkLoadFunction(vkDestroyFence);
	vkLoadFunction(vkCmdCopyBuffer);
	vkLoadFunction(vkCreateImage);
	vkLoadFunction(vkGetImageMemoryRequirements);
	vkLoadFunction(vkBindImageMemory);
	vkLoadFunction(vkCreateSampler);
	vkLoadFunction(vkCmdCopyBufferToImage);
	vkLoadFunction(vkCreateDescriptorSetLayout);
	vkLoadFunction(vkCreateDescriptorPool);
	vkLoadFunction(vkAllocateDescriptorSets);
	vkLoadFunction(vkUpdateDescriptorSets);
	vkLoadFunction(vkCmdBindDescriptorSets);
	vkLoadFunction(vkDestroyDescriptorPool);
	vkLoadFunction(vkDestroyDescriptorSetLayout);
	vkLoadFunction(vkDestroySampler);
	vkLoadFunction(vkDestroyImage);
}
#endif