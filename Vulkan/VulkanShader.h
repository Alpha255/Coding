#pragma once

#include "VulkanObject.h"

class VulkanShader : public VulkanObject<VkShaderModule>
{
public:
protected:
	void CompileShaderFile(const char *pFileName, const char *pEntryPoint);
	VkShaderStageFlagBits m_Stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
private:
};

class VulkanVertexShader : public VulkanShader
{
public:
	VulkanVertexShader()
	{
		m_Stage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	void Create(const char *pFileName, const char *pEntryPoint);
protected:
private:
};

class VulkanPixelShader : public VulkanShader
{
};
