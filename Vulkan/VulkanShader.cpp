#include "VulkanShader.h"
#include "VulkanEngine.h"
#include "System.h"

#include <fstream>

void VulkanShader::Create(const char *pFileName, const char *pEntryPoint)
{
	std::string shaderFileDir = System::ResourceFileDirectory(System::eShader);
	std::string shaderFile = shaderFileDir + pFileName;

	std::ifstream file(shaderFile, std::ios::in);
	if (file.good())
	{
		file.seekg(0U, std::ios::end);
		size_t fileSize = (size_t)file.tellg();

		std::unique_ptr<char> data(new char[fileSize]());
		file.seekg(0U, std::ios::beg);
		file.read(data.get(), fileSize);

		char workingDir[MAX_PATH] = {};
		::GetCurrentDirectoryA(MAX_PATH, workingDir);

		::SetCurrentDirectoryA(shaderFileDir.c_str());

		VkShaderModuleCreateInfo createInfo =
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0U,
			fileSize,
			(uint32_t *)data.get()
		};
		VKCheck(vkCreateShaderModule(VulkanEngine::Instance().GetDevice().GetRef(), &createInfo, nullptr, &m_Object));

		::SetCurrentDirectoryA(workingDir);

		file.close();
	}
	else
	{
		assert(!"Failed to open specified shader file!!!");
	}
}
