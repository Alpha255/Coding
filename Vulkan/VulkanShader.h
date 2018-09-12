#pragma once

#include "VulkanObject.h"

class VulkanShader : public VulkanObject<VkShaderModule>
{
public:
	VulkanShader(VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
		: m_Stage(stage)
	{
	}
protected:
	void Create(const char *pFileName, const char *pEntryPoint);
	VkShaderStageFlagBits m_Stage;
private:
};

class VulkanVertexShader : public VulkanShader
{
public:
	VulkanVertexShader()
		: VulkanShader(VK_SHADER_STAGE_VERTEX_BIT)
	{
	}
protected:
private:
};

class VulkanPixelShader : public VulkanShader
{
public:
	VulkanPixelShader()
		: VulkanShader(VK_SHADER_STAGE_FRAGMENT_BIT)
	{
	}
protected:
private:
};
