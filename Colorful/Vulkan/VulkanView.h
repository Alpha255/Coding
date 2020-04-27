#pragma once

#include "Colorful/Vulkan/VulkanTexture.h"

class vkImageView : public vkDeviceObject<VkImageView>, public rRenderSurface, public rGpuResource
{
public:
	void create(
		const class vkDevice &device, 
		const vkImage &image, 
		VkFormat format,
		VkImageAspectFlags aspectFlags);

	void create(
		const class vkDevice &device,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkImageViewType type,
		VkFormat format,
		VkImageUsageFlags usage,
		VkImageAspectFlags aspectFlags);

	void destroy(const class vkDevice &device) override final;

	inline VkFormat getFormat() const
	{
		return m_Format;
	}
protected:
	inline VkImageType getImageType(VkImageViewType type)
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
	vkImage m_Image;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
};

class vkBackBuffer : public vkImageView
{
public:
	class vkFence *m_Fence = nullptr;
protected:
private:
};

class vkRenderTargetView : public vkImageView
{
	inline void create(
		const class vkDevice &device,
		uint32_t width,
		uint32_t height,
		VkFormat format)
	{
		vkImageView::create(
			device,
			width,
			height,
			1u,
			1u,
			VK_IMAGE_VIEW_TYPE_2D,
			format,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);
	}
};

class vkDepthStencilView : public vkImageView
{
public:
	inline void create(
		const class vkDevice &device,
		uint32_t width,
		uint32_t height,
		VkFormat format)
	{
		vkImageView::create(
			device,
			width,
			height,
			1u,
			1u,
			VK_IMAGE_VIEW_TYPE_2D,
			format,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	}
};
