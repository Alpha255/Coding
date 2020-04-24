#include "VulkanView.h"
#include "VulkanEngine.h"

void vkImageView::create(
	const vkDevice &device, 
	const vkImage &image, 
	VkFormat format,
	VkImageAspectFlags aspectFlags)
{
	assert(device.isValid() && image.isValid() && !isValid());

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		*image,
		VK_IMAGE_VIEW_TYPE_2D,
		format,
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		{
			aspectFlags,
			0u,
			1u,
			0u,
			1u
		}
	};

	VkImageView handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateImageView(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
	m_Image = image;
	m_Format = format;
}

void vkImageView::create(
	const vkDevice &device, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels,
	uint32_t arrayLayers,
	VkImageViewType type,
	VkFormat format,
	VkImageUsageFlags usage,
	VkImageAspectFlags aspect)
{
	assert(device.isValid() && !isValid() && !m_Image.isValid());

	m_Image.create(
		device,
		width,
		height,
		1u,
		mipLevels,
		arrayLayers,
		format,
		getImageType(type),
		usage);

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		*m_Image,
		type,
		format,
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		{
			aspect,
			0u,
			mipLevels,
			0u,
			arrayLayers
		}
	};
	VkImageView handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateImageView(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

	m_Format = format;
}

void vkImageView::destroy(const vkDevice &device)
{
	assert(device.isValid());
	if (isValid())
	{
		vkDestroyImageView(*device, **this, vkMemoryAllocator);
		reset();

		if (m_Image.m_Memory.isValid())
		{
			m_Image.destroy(device);
		}
		else
		{
			m_Image.reset();
		}
	}
}
