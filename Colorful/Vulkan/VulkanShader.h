#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VulkanShader final : public VkObject<VkShaderModule_T>, public Shader
{
public:
	VulkanShader(VkDevice device, EShaderStage stage, const std::vector<uint32_t>& spirv)
		: Shader(stage)
	{
		assert(device && stage < EShaderStage::ShaderStageCount);

		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0u,
			spirv.size() * sizeof(uint32_t),
			spirv.data()
		};

		VERIFY_VK(vkCreateShaderModule(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
	}
};

NAMESPACE_END(Gfx)