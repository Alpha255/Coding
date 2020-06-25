#include "Colorful/Vulkan/VulkanEngine.h"
#include "AssetTool/AssetDatabase.h"

VulkanShader::VulkanShader(VkDevice device, eRShaderUsage usage, const std::string& shaderName)
	: GfxShader(usage)
{
	assert(!isValid() && usage < eRShaderUsage_MaxEnum);

	auto binary = AssetTool::AssetDatabase().instance().tryToGetShaderBinary(Configurations::eVulkan, usage, shaderName);
	assert(binary.Size > 0ull);

	VkShaderModuleCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use
		binary.Size,
		(uint32_t*)binary.Binary.get()
	};

	GfxVerifyVk(vkCreateShaderModule(device, &createInfo, vkMemoryAllocator, &Handle));

	m_Reflections = binary.Reflections;
}
