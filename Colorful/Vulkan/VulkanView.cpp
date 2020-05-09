#include "VulkanView.h"
#include "VulkanEngine.h"

void vkImageView::create(
	VkDevice device, 
	const vkImage &image, 
	VkFormat format,
	VkImageAspectFlags aspectFlags)
{
	assert(image.isValid() && !isValid());

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		image.Handle,
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

	rVerifyVk(vkCreateImageView(device, &createInfo, vkMemoryAllocator, &Handle));
	m_Image = image;
	m_Format = format;
}

void vkImageView::create(
	VkDevice device, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels,
	uint32_t arrayLayers,
	VkImageViewType type,
	VkFormat format,
	VkImageUsageFlags usage,
	VkImageAspectFlags aspect)
{
	assert(!isValid() && !m_Image.isValid());

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
		m_Image.Handle,
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
	rVerifyVk(vkCreateImageView(device, &createInfo, vkMemoryAllocator, &Handle));

	m_Format = format;
}

void vkImageView::create(
	VkDevice device,
	eRTextureType type, 
	eRFormat format, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels, 
	uint32_t arrayLayers, 
	const void *data, 
	size_t dataSize)
{
	rAsset::rTextureBinary texBinary
	{
		type,
		(uint32_t)vkEngine::enumTranslator::toFormat(format),
		width,
		height,
		1u,
		mipLevels,
		arrayLayers,
		dataSize
	};
	texBinary.Binary = static_cast<const byte8_t *>(data);
	texBinary.Images.resize(1u);
	uint32_t mipWidth = width;
	uint32_t mipHeight = height;
	size_t mipSize = dataSize;
	for (uint32_t i = 0u; i < mipLevels; ++i)
	{
		rAsset::rTextureBinary::rImage image
		{
			mipWidth,
			mipHeight,
			1u,
			mipSize
		};
		texBinary.Images[0].emplace_back(std::move(image));

		mipWidth /= 2u;
		mipHeight /= 2u;
		mipSize = (size_t)(((float32_t)(mipWidth * mipHeight) / (width * height)) * dataSize); /// May incorrect
	}

	vkImage image(device, texBinary);

	create(device, image, image.getFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
}

void vkImageView::destroy(VkDevice device)
{
	if (isValid())
	{
		vkDestroyImageView(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;

		if (m_Image.m_Memory.isValid())
		{
			m_Image.destroy(device);
		}
		else
		{
			m_Image.Handle = VK_NULL_HANDLE;
		}
	}
}
