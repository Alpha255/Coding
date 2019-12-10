#include "VulkanShader.h"
#include "VulkanEngine.h"

vkShader::vkShader(const vkDevice &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary)
	: rShader(usage)
{
	assert(!isValid() && device.isValid());
	assert(binary.Binary && binary.Size > 0u && usage < eRShaderUsage_MaxEnum);
	assert((binary.Size % sizeof(uint32_t)) == 0);

	VkShaderModuleCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use
		binary.Size,
		(uint32_t *)binary.Binary.get()
	};

	VkShaderModule handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateShaderModule(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkShader::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyShaderModule(*device, **this, vkMemoryAllocator);
		reset();
	}
}