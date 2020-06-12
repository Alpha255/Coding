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

	allocResources();
}

void VulkanShader::setInputLayout(const std::vector<GfxVertexAttributes>& vertexAttributes, size_t align)
{
	assert(m_Usage == eVertexShader && m_InputLayout.InputState.sType == 0);

	m_InputLayout.InputBindings.resize(1u);
	m_InputLayout.InputAttributes.resize(vertexAttributes.size());

	uint32_t stride = 0u;
	for (uint32_t i = 0u; i < vertexAttributes.size(); ++i)
	{
		m_InputLayout.InputAttributes[i] = VkVertexInputAttributeDescription
		{
			i,
			0u,
			VulkanEnum::toFormat(vertexAttributes[i].Format),
			stride
		};

		stride += ((uint32_t)GfxVertexAttributes::formatStride(vertexAttributes[i].Format));
		stride = (uint32_t)Gear::LinearAllocator::align_to(stride, align);
	}

	m_InputLayout.InputBindings[0] = VkVertexInputBindingDescription
	{
		0u,
		stride,
		VK_VERTEX_INPUT_RATE_VERTEX
	};

	m_InputLayout.InputState = VkPipelineVertexInputStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)m_InputLayout.InputBindings.size(),
		m_InputLayout.InputBindings.data(),
		(uint32_t)m_InputLayout.InputAttributes.size(),
		m_InputLayout.InputAttributes.data()
	};
}
