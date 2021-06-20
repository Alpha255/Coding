#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VulkanTexture : public VkDeviceObject<VkImageView_T>, public Texture
{
public:
	VulkanTexture(VkImage image, VkFormat format); /// For back buffers

	VulkanTexture(const TextureDesc& desc, uint32_t aspectFlags);
	~VulkanTexture();
protected:
private:
	VkImage m_Image = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
};

class VulkanSampler final : public VkDeviceObject<VkSampler_T>, public Sampler
{
public:
	VulkanSampler(const SamplerDesc& desc);
	~VulkanSampler();
protected:
private:
};

NAMESPACE_END(Gfx)
