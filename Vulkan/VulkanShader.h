#pragma once

#include "VulkanObject.h"

class VulkanShader
{
public:
	void CompileShaderFile(const char *pFileName, const char *pEntryPoint);
protected:
private:
};

class VulkanVertexShader : public VulkanObject<VkShaderModule>, public VulkanShader
{
public:
	void Create(const char *pFileName, const char *pEntryPoint);
protected:
private:
};

class VulkanPixelShader : public VulkanObject<VkShaderModule>, public VulkanShader
{
};
