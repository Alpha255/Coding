#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(Gfx)

VulkanShader::VulkanShader(ShaderDesc& desc)
	: Shader(desc.Stage, desc.Reflections)
{
	assert(m_Stage < EShaderStage::ShaderStageCount && desc.BinarySize);

	VkShaderModuleCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0u,
		desc.BinarySize,
		desc.Binary.data()
	};

	VERIFY_VK(vkCreateShaderModule(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

NAMESPACE_END(Gfx)