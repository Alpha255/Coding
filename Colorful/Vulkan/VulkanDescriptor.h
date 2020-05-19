#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet>
{
public:
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

	VkDescriptorSetLayout layout() const
	{
		assert(isValid());
		return m_Layout.Handle;
	}

	void destroy(VkDevice device)
	{
		m_Layout.destroy(device);
	}
protected:
	friend class VulkanDescriptorPool;
private:
	VulkanDescriptorSetLayout m_Layout;
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
