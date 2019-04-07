#include "VulkanShader.h"
#include "VulkanEngine.h"
#include "Base/AssetFile.h"

void VulkanInputLayout::Create(const void *, const std::vector<Geometry::VertexLayout> &layouts)
{
	assert(!m_bValid && layouts.size() > 0U);

	m_VertexInputAttrs.clear();

	size_t stride = 0U;
	for (uint32_t i = 0U; i < layouts.size(); ++i)
	{
		VkVertexInputAttributeDescription inputAttrDesc
		{
			i,
			0U,
			(VkFormat)layouts[i].Format,
			layouts[i].Offset
		};
		m_VertexInputAttrs.emplace_back(inputAttrDesc);

		stride += layouts[i].Stride;
	}

	m_InputBinding = VkVertexInputBindingDescription
	{
		0U,
		(uint32_t)stride,
		VK_VERTEX_INPUT_RATE_VERTEX
	};

	m_CreateInfo = VkPipelineVertexInputStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&m_InputBinding,
		(uint32_t)m_VertexInputAttrs.size(),
		m_VertexInputAttrs.data()
	};

	m_bValid = true;
}

void VulkanShader::Create(const std::string &fileName, const std::string &entryPoint)
{
	assert(!IsValid());

	m_EntryName = entryPoint;

	AssetFile shaderFile(fileName + ".spv");
	auto shaderCode = shaderFile.Load();

	VkShaderModuleCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0U,
		shaderFile.GetSize(),
		(uint32_t *)shaderCode.get()
	};
	Check(vkCreateShaderModule(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	m_ShaderStage = VkPipelineShaderStageCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0U,
		m_Stage,
		m_Handle,
		m_EntryName.c_str(),
		nullptr
	};
}

void VulkanShader::Destory()
{
	assert(IsValid());

	vkDestroyShaderModule(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}
