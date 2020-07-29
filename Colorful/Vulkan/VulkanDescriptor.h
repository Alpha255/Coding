#pragma once

#include "Colorful/Vulkan/VulkanImageView.h"

/// VulkanDescriptor Manager???

class VulkanDescriptorSetLayout : public VulkanDeviceObject<VkDescriptorSetLayout>
{
public:
	void create(VkDevice device, const GfxDescriptorLayoutDesc& desc);

	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			vkDestroyDescriptorSetLayout(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
};

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
public:
	struct VulkanResourceInfo
	{
#if !defined(UsingUnorderedMap)
		uint32_t Binding = std::numeric_limits<uint32_t>::max();
#endif
		VkDescriptorType Type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
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
#if defined(UsingUnorderedMap)
	using VulkanResourceMap = std::unordered_map<uint32_t, VulkanResourceInfo>;
#else
	using VulkanResourceMap = std::vector<VulkanResourceInfo>;
#endif

	void update(VkDevice device, const VulkanResourceMap& resourceMap);

#if !defined(UsingUnorderedMap)
	inline size_t hash() const
	{
		return m_Hash;
	}

	inline void setHash(size_t hash)
	{
		m_Hash = hash;
	}
#endif
protected:
private:
#if !defined(UsingUnorderedMap)
	size_t m_Hash = 0u;
#endif
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

	VulkanDescriptorSet alloc(VkDescriptorSetLayout layout, const VulkanDescriptorSet::VulkanResourceMap& resourceMap) const;
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