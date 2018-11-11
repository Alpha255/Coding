#include "VulkanShader.h"
#include "VulkanEngine.h"
#include "System.h"
#include "ResourceFile.h"

void VulkanShader::Create(const char *pFileName)
{
	ResourceFile shaderFile(pFileName);

	///char workingDir[MAX_PATH] = {};
	///::GetCurrentDirectoryA(MAX_PATH, workingDir);

	///::SetCurrentDirectoryA(shaderFileDir.c_str());

	VkShaderModuleCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0U,
		shaderFile.GetSize(),
		(uint32_t *)shaderFile.Load()
	};
	VKCheck(vkCreateShaderModule(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));

	///::SetCurrentDirectoryA(workingDir);
}
