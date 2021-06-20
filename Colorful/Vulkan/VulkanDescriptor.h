#pragma once

#include "Colorful/Vulkan/VulkanTexture.h"

/// VulkanDescriptor Manager???

NAMESPACE_START(Gfx)

class VulkanDescriptorSet final : public VkDeviceObject<VkDescriptorSet_T>
{
};

class VulkanDescriptorPool final : public VkDeviceObject<VkDescriptorPool_T>
{
public:
	VulkanDescriptorPool();
	~VulkanDescriptorPool();

	void reset();

	VulkanDescriptorSetPtr alloc(VkDescriptorSetLayout layout);
};

NAMESPACE_END(Gfx)