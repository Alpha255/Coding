#include "VulkanShader.h"
#include "VulkanEngine.h"
#include "System.h"
#include "ResourceFile.h"

#include <fstream>

void VulkanShader::Create(const char *pFileName, const char *pEntryPoint)
{
	assert(0);

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
	VKCheck(vkCreateShaderModule(VulkanEngine::Instance().GetDevice().GetRef(), &createInfo, nullptr, &m_Object));

	///::SetCurrentDirectoryA(workingDir);
}
