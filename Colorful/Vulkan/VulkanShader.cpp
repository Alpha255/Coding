#include "VulkanShader.h"
#include "VulkanEngine.h"

void VulkanInputLayout::Create(const void *, const std::vector<Geometry::VertexLayout> &layouts)
{
	assert(!m_bValid && layouts.size() > 0U);

	m_VertexInputAttrs.clear();

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
	}

	m_bValid = true;
}

void VulkanShader::Create(const std::string &, const std::string entryPoint)
{
	assert(!IsValid());

	///std::string shaderCode = System::GetShaderCode(pFileName, "#Vulkan", m_Type);
	std::string shaderCode("");
	
	///char workingDir[MAX_PATH] = {};
	///::GetCurrentDirectoryA(MAX_PATH, workingDir);

	///::SetCurrentDirectoryA(shaderFileDir.c_str());

	VkShaderModuleCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0U,
		shaderCode.length(),
		(uint32_t *)shaderCode.c_str()
	};
	Check(vkCreateShaderModule(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	m_ShaderStage = VkPipelineShaderStageCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0U,
		m_Stage,
		m_Handle,
		entryPoint.c_str(),
		nullptr
	};
	///::SetCurrentDirectoryA(workingDir);
}

void VulkanShader::Destory()
{
	assert(IsValid());

	vkDestroyShaderModule(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}
