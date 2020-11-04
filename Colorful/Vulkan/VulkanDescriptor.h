#pragma once

#include "Colorful/Vulkan/VulkanTexture.h"
#include "Colorful/Public/GfxResource.h"

/// VulkanDescriptor Manager???

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanDescriptorSetLayout)
class VulkanDescriptorSetLayout final : public VkObject<VkDescriptorSetLayout_T>, public VkDeviceResource
{
public:
	VulkanDescriptorSetLayout(VkDevice device, const PipelineResourceLayoutDesc& desc);

	void destroy(VkDevice device) override final
	{
		assert(device);
		vkDestroyDescriptorSetLayout(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_SHARED_PTR(VulkanDescriptorSet)
class VulkanDescriptorSet final : public VkObject<VkDescriptorSet_T>
{
};

DECLARE_SHARED_PTR(VulkanDescriptorPool)
class VulkanDescriptorPool final : public VkObject<VkDescriptorPool_T>
{
public:
	VulkanDescriptorPool(VkDevice device, VkPhysicalDevice physicalDevice);

	void reset()
	{
		/// return all descriptor sets allocated from a given pool to the pool
		/// flags is reserved for future use
		/// Resetting a descriptor pool recycles all of the resources from all of the descriptor sets allocated from the descriptor pool back to the descriptor pool, 
		/// and the descriptor sets are implicitly freed.
		assert(isValid());
		vkResetDescriptorPool(m_Device, get(), 0u);
	}

	void destroy()
	{
		/// When a pool is destroyed, all descriptor sets allocated from the pool are implicitly freed and become invalid. 
		/// Descriptor sets allocated from a given pool do not need to be freed before destroying that descriptor pool.
		vkDestroyDescriptorPool(m_Device, get(), VK_MEMORY_ALLOCATOR);
	}

	VulkanDescriptorSetPtr alloc(VkDescriptorSetLayout layout);
protected:
private:
	const VkDevice m_Device;
};

NAMESPACE_END(Gfx)