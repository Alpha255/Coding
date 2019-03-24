#pragma once

#include "VulkanBuffer.h"

class VulkanCommandPool : public VulkanObject<VkCommandPool>
{
public:
	enum ePoolType
	{
		eTemp,
		eGeneral,
		eProtected,
		eGeneralTemp,
		ePoolTypeCount
	};

	enum eBufferType
	{
		ePrimary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		eSecondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
	};

	void Create(ePoolType type);

	VulkanCommandBuffer Alloc(eBufferType type, uint32_t count);

	void Free(VulkanCommandBuffer &cmdBuffer);

	void Destory() override;
protected:
private:
	uint32_t m_BufferCount = 0U;
	ePoolType m_Type = ePoolTypeCount;
};

class VulkanDescriptorSetLayout : public VulkanObject<VkDescriptorSetLayout>
{
public:
	void Create(uint32_t targetShader, uint32_t slot, uint32_t count);
	void Destory() override;
protected:
private:
};

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
public:
	inline void CreateLayout(uint32_t targetShader, uint32_t slot, uint32_t count)
	{
		m_Layout.Create(targetShader, slot, count);
	}

	inline const VulkanDescriptorSetLayout &GetLayout() const
	{
		assert(m_Layout.IsValid());
		return m_Layout;
	}

	void Update(const class VulkanSamplerState &sampler, const class VulkanImageView &imageView, uint32_t slot);

	void Destory() override
	{
		m_Layout.Destory();

		Reset();
	}
protected:
private:
	VulkanDescriptorSetLayout m_Layout;
};

class VulkanDescriptorPool : public VulkanObject<VkDescriptorPool>
{
public:
	void Create();

	VulkanDescriptorSet Alloc(uint32_t targetShader, uint32_t slot, uint32_t count);

	void Free(VulkanDescriptorSet &descriptorSet);

	void ResetPool();

	void Destory() override;
protected:
	enum eLimits
	{
		///eMaxSetCount = 1024
	};
private:
	VkDescriptorType m_Type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
};
