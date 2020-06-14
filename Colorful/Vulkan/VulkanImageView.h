#pragma once

#include "Colorful/Vulkan/VulkanImage.h"
#include "Colorful/Vulkan/VulkanEnum.h"

class VulkanImageView : public VulkanDeviceObject<VkImageView>, public GfxRenderSurface, public GfxTexture
{
public:
	VulkanImageView(VkDevice device, const std::string& texName);

	VulkanImageView(VkDevice device, const VulkanImagePtr& image, VkImageViewType type, VkImageAspectFlags aspectFlags);

	VulkanImageView(
		VkDevice device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkImageViewType type,
		VkFormat format,
		VkImageUsageFlags usage,
		VkImageAspectFlags aspectFlags);

	VulkanImageView(
		VkDevice device,
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void* data,
		size_t dataSize);

	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			m_Image->destroy(device);
			
			vkDestroyImageView(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}

	inline VkFormat format() const
	{
		assert(m_Image);
		return m_Image->format();
	}
protected:
	inline VkImageType imageType(VkImageViewType type)
	{
		switch (type)
		{
		case VK_IMAGE_VIEW_TYPE_1D: 
		case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
			return VK_IMAGE_TYPE_1D;
		case VK_IMAGE_VIEW_TYPE_2D: 
		case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
			return VK_IMAGE_TYPE_2D;
		case VK_IMAGE_VIEW_TYPE_3D: 
			return VK_IMAGE_TYPE_3D;
		case VK_IMAGE_VIEW_TYPE_CUBE: 
		case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
			return VK_IMAGE_TYPE_2D;
		}

		return VK_IMAGE_TYPE_MAX_ENUM;
	}
private:
	VulkanImagePtr m_Image = nullptr;
};
using VulkanImageViewPtr = std::shared_ptr<VulkanImageView>;

class VulkanRenderTarget : public VulkanImageView
{
public:
	VulkanRenderTarget(
		VkDevice device,
		uint32_t width,
		uint32_t height,
		VkFormat format)
		: VulkanImageView(
			device,
			width,
			height,
			1u,
			1u,
			1u,
			VK_IMAGE_VIEW_TYPE_2D,
			format,
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
