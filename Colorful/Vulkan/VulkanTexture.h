#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanTexture)
class VulkanTexture final : public VkObject<VkImageView_T>, public Texture
{
public:
	enum class ECreationMode
	{
		Immediately,
		Queued
	};

	enum class ELayout
	{
		Undefined,
		TransferDst,
		ColorAttachment,
		DepthStencilAttachment,
		TransferSrc,
		Present,
		FragmentShaderRead,
		PixelDepthStencilRead,
		ComputeShaderReadWrite,
		FragmentShaderReadWrite
	};

	VulkanTexture(VkDevice device, const TextureDesc& desc, uint32_t aspectFlags, ECreationMode mode);

	void destroy(VkDevice device)
	{
		assert(device);
		vkFreeMemory(device, m_Memory, VK_MEMORY_ALLOCATOR);
		vkDestroyImage(device, m_Image, VK_MEMORY_ALLOCATOR);
		vkDestroyImageView(device, get(), VK_MEMORY_ALLOCATOR);
		m_Image = VK_NULL_HANDLE;
		m_Memory = VK_NULL_HANDLE;
	}

	static void getBarrierFlags(ELayout layout, VkPipelineStageFlags& stageFlags, VkAccessFlags& accessFlags, VkImageLayout& imageLayout);
protected:
private:
	VkImage m_Image = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
};

DECLARE_SHARED_PTR(VulkanSampler)
class VulkanSampler final : public VkObject<VkSampler_T>, public Sampler
{
public:
	VulkanSampler(VkDevice device, const SamplerDesc& desc);

	void destroy(VkDevice device)
	{
		vkDestroySampler(device, get(), VK_MEMORY_ALLOCATOR);
	}
protected:
private:
};

NAMESPACE_END(Gfx)
