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

	m_Reflections = binary.Reflections;
}

void vkShader::destroy(const vkDevice &device)
{
	/// A shader module can be destroyed while pipelines created using its shaders are still in use.
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyShaderModule(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkShader::setInputLayout(const std::vector<rVertexAttributes>& vertexAttributes)
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
			vkEngine::enumTranslator::toFormat(vertexAttributes[i].Format),
			stride
		};
		stride += rVertexAttributes::getFormatStride(vertexAttributes[i].Format);
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
