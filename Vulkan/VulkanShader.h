#pragma once

#include "VulkanObject.h"

class VulkanInputLayout
{
public:
	void Create(const void *, const VertexLayout *pLayout, size_t layoutCount);

	inline const std::vector<VkVertexInputAttributeDescription> &Get() const
	{
		assert(m_bValid);
		return m_VertexInputAttrs;
	}
protected:
private:
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttrs;
	bool m_bValid = false;
};

class VulkanShader : public VulkanObject<VkShaderModule>
{
public:
	VulkanShader(VkShaderStageFlagBits stage, eRShaderType type)
		: m_Stage(stage)
		, m_Type(type)
	{
	}

	VkPipelineShaderStageCreateInfo GetShaderStage() const
	{
		return m_ShaderStage;
	}

	void Create(const char *pFileName, const char *pEntryPoint);
	void Destory();
protected:
	VkShaderStageFlagBits m_Stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	VkPipelineShaderStageCreateInfo m_ShaderStage = {};
	eRShaderType m_Type = eRShaderTypeCount;
private:
};

class VulkanVertexShader : public VulkanShader
{
public:
	VulkanVertexShader()
		: VulkanShader(VK_SHADER_STAGE_VERTEX_BIT, eVertexShader)
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
		: VulkanShader(VK_SHADER_STAGE_FRAGMENT_BIT, ePixelShader)
	{
	}
protected:
private:
};
