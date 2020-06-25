#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class VulkanShader : public VulkanDeviceObject<VkShaderModule>, public GfxShader
{
public:
	VulkanShader(VkDevice device, eRShaderUsage usage, const std::string& shaderName);

	void destroy(VkDevice device) override final
	{
		/// A shader module can be destroyed while pipelines created using its shaders are still in use.
		if (isValid())
		{
			vkDestroyShaderModule(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};