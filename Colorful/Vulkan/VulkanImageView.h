#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"

NAMESPACE_START(Gfx)

class VulkanImageView : public VkObject<VkImageView_T>
{
public:
	enum class EImageLayout : uint8_t
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

	VulkanImageView(VkDevice device, const TextureDesc& desc, uint32_t usageFlags, uint32_t aspectFlags, VkImageLayout layout);

	void destroy(VkDevice device)
	{
		assert(device);
		vkFreeMemory(device, m_Buffer, VK_MEMORY_ALLOCATOR);
		vkDestroyImage(device, m_Image, VK_MEMORY_ALLOCATOR);
		vkDestroyImageView(device, get(), VK_MEMORY_ALLOCATOR);
		m_Image = VK_NULL_HANDLE;
		m_Buffer = VK_NULL_HANDLE;
	}
protected:
	void getMemoryBarrierFlags(
		EImageLayout layout,
		VkPipelineStageFlags& stageFlags,
		VkAccessFlags& accessFlags,
		VkImageLayout& imageLayout);
private:
	VkImage m_Image = VK_NULL_HANDLE;
	VkDeviceMemory m_Buffer = VK_NULL_HANDLE;
};

DECLARE_SHARED_PTR(VulkanTexture1D)
class VulkanTexture1D final : public VulkanImageView
{
public:
	VulkanTexture1D(VkDevice device, const TextureDesc& desc)
		: VulkanImageView(
			device, 
			desc, 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
			VK_IMAGE_ASPECT_COLOR_BIT, 
			VK_IMAGE_LAYOUT_UNDEFINED)
	{
	}
};

DECLARE_SHARED_PTR(VulkanTexture2D)
class VulkanTexture2D final : public VulkanImageView
{
public:
	VulkanTexture2D(VkDevice device, const TextureDesc& desc)
		: VulkanImageView(
			device,
			desc,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED)
	{
	}
};

DECLARE_SHARED_PTR(VulkanTexture3D)
class VulkanTexture3D final : public VulkanImageView
{
public:
	VulkanTexture3D(VkDevice device, const TextureDesc& desc)
		: VulkanImageView(
			device,
			desc,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED)
	{
	}
};

#if 0
class VulkanRenderTarget : public VulkanImageView
{
public:
	VulkanRenderTarget(
		VkDevice device,
		uint32_t width,
		uint32_t height,
		eRFormat format)
		: VulkanImageView(
			device,
			width,
			height,
			1u,
			1u,
			1u,
			VK_IMAGE_VIEW_TYPE_2D,
			VulkanEnum::toFormat(format),
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT)
	{
	}
};

class VulkanDepthStencil : public VulkanImageView
{
public:
	VulkanDepthStencil(
		VkDevice device,
		uint32_t width,
		uint32_t height,
		eRFormat format)
		: VulkanImageView(
			device,
			width,
			height,
			1u,
			1u,
			1u,
			VK_IMAGE_VIEW_TYPE_2D,
			VulkanEnum::toFormat(format),
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
	{
	}
};
#endif

NAMESPACE_END(Gfx)
