#pragma once

#include "VulkanObject.h"

class VulkanInputLayout
{
public:
	inline void Create(const void *, const VertexLayout *pLayout, size_t layoutCount)
	{

	}
protected:
private:
	std::vector<VkVertexInputBindingDescription> m_VertexInputBindingDes;
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttrs;
	VkPipelineVertexInputStateCreateInfo m_InputState = {};
};

class VulkanShader : public VulkanObject<VkShaderModule>
{
public:
	VulkanShader(VkShaderStageFlagBits stage)
		: m_Stage(stage)
	{
	}

	void Create(const char *pFileName, const char *pEntryPoint);

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

	inline void *GetBlob() const
	{
		return nullptr;
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
