#pragma once

#include "VulkanObject.h"
#include "Public/Geometry.h"

class VulkanInputLayout
{
public:
	inline bool IsValid() const
	{
		return m_bValid;
	}

	inline uint32_t GetVertexStride() const
	{
		return 0ull;
	}

	void Create(const void *, const std::vector<Geometry::VertexLayout> & layouts);

	inline const VkPipelineVertexInputStateCreateInfo &Get() const
	{
		assert(m_bValid);
		return m_CreateInfo;
	}
protected:
private:
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttrs;
	VkPipelineVertexInputStateCreateInfo m_CreateInfo = {};
	VkVertexInputBindingDescription m_InputBinding = {};
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
		assert(IsValid());
		return m_ShaderStage;
	}

	void Create(const std::string &fileName, const std::string &entryPoint);

	void Destory() override;
protected:
	VkShaderStageFlagBits m_Stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	VkPipelineShaderStageCreateInfo m_ShaderStage = {};
	eRShaderType m_Type = eRShaderTypeCount;
	std::string m_EntryName;
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
