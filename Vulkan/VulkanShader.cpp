#include "VulkanShader.h"
#include "System.h"

#include <fstream>

void VulkanShader::CompileShaderFile(const char *pFileName, const char *pEntryPoint)
{
	std::string shaderFileDir = System::ResourceFileDirectory(System::eShader);
	std::string shaderFile = shaderFileDir + pFileName;

	std::ifstream file(shaderFile, std::ios::in);
	if (file.good())
	{
		file.seekg(0U, std::ios::end);
		size_t fileSize = (size_t)file.tellg();

		char* pData = new char[fileSize]();
		file.seekg(0U, std::ios::beg);
		file.read(pData, fileSize);

		char workingDir[MAX_PATH] = {};
		::GetCurrentDirectoryA(MAX_PATH, workingDir);

		::SetCurrentDirectoryA(shaderFileDir.c_str());



		::SetCurrentDirectoryA(workingDir);

		SafeDeleteArray(pData);
		file.close();
	}
	else
	{
		assert(!"Failed to open specified shader file!!!");
	}
}

void VulkanVertexShader::Create(const char *pFileName, const char *pEntryPoint)
{

}
