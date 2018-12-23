#pragma once

#include "VulkanObject.h"

class VulkanShader : public VulkanObject<VkShaderModule>
{
public:
	VulkanShader(VkShaderStageFlagBits stage)
		: m_Stage(stage)
	{
	}

	void Create(const char *pFileName);

	VkPipelineShaderStageCreateInfo GetShaderStage() const
	{
		return m_ShaderStage;
	}
protected:
	VkShaderStageFlagBits m_Stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	VkPipelineShaderStageCreateInfo m_ShaderStage = {};
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
