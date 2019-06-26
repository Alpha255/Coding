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
	void Create(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings);
	void Destory() override;
protected:
private:
};

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
public:
	inline void CreateLayout(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings)
	{
		m_Layout.Create(layoutBindings);
	}

	inline const VulkanDescriptorSetLayout &GetLayout() const
	{
		assert(m_Layout.IsValid());
		return m_Layout;
	}

	void SetupImage(const class VulkanImageView &image, uint32_t slot);
	void SetupSampler(const class VulkanSamplerState &sampler, uint32_t slot);
	void SetupCombinedImage(const class VulkanImageView &image, uint32_t slot);
	void SetupUniformBuffer(const class VulkanBuffer &uniformBuffer, uint32_t slot);

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
	enum eDescriptorType
	{
		eImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		eSampler = VK_DESCRIPTOR_TYPE_SAMPLER,
		eCombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		eUniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		ePoolTypeCount = 2U
	};

	enum eLimits
	{
		eMaxSamplers = 16U,
		eMaxUniformBuffers = 32U,
		eMasDescriptorSets = 64U
	};

	void Create();

	VulkanDescriptorSet Alloc(const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings);

	void Free(VulkanDescriptorSet &descriptorSet);

	void ResetPool();

	void Destory() override;
protected:
private:
};
