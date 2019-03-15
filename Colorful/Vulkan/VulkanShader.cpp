#include "VulkanShader.h"
#include "VulkanEngine.h"
#include "Util/System.h"
#include "Util/ResourceFile.h"

void VulkanInputLayout::Create(const void *, const VertexLayout *pLayout, size_t layoutCount)
{
	assert(!m_bValid && pLayout && layoutCount);

	m_VertexInputAttrs.clear();

	const VertexLayout *pVertexLayout = pLayout;
	size_t stride = 0U;

	for (uint32_t i = 0U; i < layoutCount; ++i, pVertexLayout++)
	{
		VkVertexInputAttributeDescription inputAttrDesc
		{
			i,
			0U,
			(VkFormat)pVertexLayout->Format,
			pVertexLayout->Offset
		};
		m_VertexInputAttrs.emplace_back(inputAttrDesc);

		stride += pVertexLayout->Stride;
	}

	m_bValid = true;
}

void VulkanShader::Create(const char *pFileName, const char *pEntryPoint)
{
	assert(!IsValid());

	std::string shaderCode = System::GetShaderCode(pFileName, "#Vulkan", m_Type);

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
	VKCheck(vkCreateShaderModule(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));

	m_ShaderStage.flags = 0;
	m_ShaderStage.module = m_Handle;
	m_ShaderStage.pName = pEntryPoint;
	m_ShaderStage.pNext = nullptr;
	m_ShaderStage.pSpecializationInfo = nullptr;
	m_ShaderStage.stage = m_Stage;
	m_ShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	///::SetCurrentDirectoryA(workingDir);
}

void VulkanShader::Destory()
{
	assert(IsValid());

	vkDestroyShaderModule(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}
