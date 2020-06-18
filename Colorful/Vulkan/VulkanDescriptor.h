#pragma once

#include "Colorful/Vulkan/VulkanImageView.h"

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
public:
	VkDescriptorSetLayout layout() const
	{
		assert(isValid());
		return m_Layout;
	}

	void destroy(VkDevice device)
	{
		if (isValid())
		{
			vkDestroyDescriptorSetLayout(device, m_Layout, vkMemoryAllocator);
			m_Layout = VK_NULL_HANDLE;
			m_ResourceBindingMap.clear();
		}
	}

	void setUniformBuffer(const VulkanBufferPtr& buffer, uint32_t binding);

	void setTexture(const VulkanImageViewPtr& texture, uint32_t binding);

	void setCombinedTextureSampler(const VulkanImageViewPtr& texture, const VulkanSamplerPtr& sampler, uint32_t binding);

	void setSampler(const VulkanSamplerPtr& sampler, uint32_t binding);

	void update(VkDevice device);
protected:
	struct ResourceBindingInfo
	{
		VkDescriptorType Type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
		bool8_t Dirty = true;

		union
		{
			struct VkCombinedImageSampler
			{
				VkImageView ImageView;
				VkSampler Sampler;
			} CombinedImageSampler;

			VkBuffer Buffer;
			VkImageView ImageView;
			VkSampler Sampler;
			VkBufferView TexelBufferView;
		};
	};

	void createLayout(VkDevice device, const GfxDescriptorLayoutDesc& desc);

	friend class VulkanDescriptorPool;
private:
	VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
	std::unordered_map<uint32_t, ResourceBindingInfo> m_ResourceBindingMap;
};

class VulkanDescriptorPool : public VulkanObject<VkDescriptorPool>
{
public:
	VulkanDescriptorPool(const VkDevice device, VkPhysicalDevice physicalDevice);

	void reset()
	{
		/// return all descriptor sets allocated from a given pool to the pool
		/// flags is reserved for future use
		/// Resetting a descriptor pool recycles all of the resources from all of the descriptor sets allocated from the descriptor pool back to the descriptor pool, 
		/// and the descriptor sets are implicitly freed.
		assert(isValid());
		vkResetDescriptorPool(m_Device, Handle, 0u);
	}

	void destroy()
	{
		/// When a pool is destroyed, all descriptor sets allocated from the pool are implicitly freed and become invalid. 
		/// Descriptor sets allocated from a given pool do not need to be freed before destroying that descriptor pool.
		if (isValid())
		{
			vkDestroyDescriptorPool(m_Device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}

	VulkanDescriptorSet alloc(const GfxDescriptorLayoutDesc& desc) const;
protected:
private:
	const VkDevice m_Device;
};

class VulkanMainDescriptorPool : public VulkanDescriptorPool, public LazySingleton<VulkanMainDescriptorPool>
{
	lazySingletonDeclare(VulkanMainDescriptorPool);
public:
protected:
	VulkanMainDescriptorPool(const VkDevice device, VkPhysicalDevice physicalDevice)
		: VulkanDescriptorPool(device, physicalDevice)
	{
	}

	void cleanup() override final
	{
		destroy();
	}
private:
};