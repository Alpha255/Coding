#pragma once

#include "Colorful/Vulkan/VulkanTexture.h"

/// VulkanDescriptor Manager???

NAMESPACE_START(Gfx)

class VulkanDescriptorSet final : public VkDeviceObject<VkDescriptorSet_T>
{
};
DECLARE_SHARED_PTR(VulkanDescriptorSet)

class VulkanDescriptorPool final : public VkDeviceObject<VkDescriptorPool_T>
{
public:
	VulkanDescriptorPool();
	~VulkanDescriptorPool();

	void reset();

	VulkanDescriptorSetPtr alloc(VkDescriptorSetLayout layout);
};
DECLARE_SHARED_PTR(VulkanDescriptorPool)

NAMESPACE_END(Gfx)