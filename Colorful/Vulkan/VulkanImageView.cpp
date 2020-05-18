#include "Colorful/Vulkan/VulkanEngine.h"
#include "AssetTool/AssetDatabase.h"

VulkanImageView::VulkanImageView(VkDevice device, const VulkanImagePtr& image, VkImageViewType type, VkImageAspectFlags aspectFlags)
{
	assert(image && image->isValid() && !isValid());
	m_Image = image;

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		m_Image->Handle,
		type,
		m_Image->format(),
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		{
			aspectFlags,
			0u,
			m_Image->mipLevels(),
			0u,
			m_Image->arrayLayers()
		}
	};

	GfxVerifyVk(vkCreateImageView(device, &createInfo, vkMemoryAllocator, &Handle));
}

VulkanImageView::VulkanImageView(VkDevice device, const std::string& texName)
{
	assert(!isValid());

	auto texBinary = AssetTool::AssetDatabase::instance().tryToGetTextureBinary(Configurations::eVulkan, texName);
	assert(texBinary.Size > 0u);

	auto image = std::make_shared<VulkanImage>(device, texBinary);
	VulkanImageView::VulkanImageView(
		device, 
		image,
		VulkanEnum::toImageViewType(texBinary.Type),
		VK_IMAGE_ASPECT_COLOR_BIT);
}

VulkanImageView::VulkanImageView(
	VkDevice device, 
	uint32_t width, 
	uint32_t height, 
	uint32_t depth,
	uint32_t mipLevels,
	uint32_t arrayLayers,
	VkImageViewType type,
	VkFormat format,
	VkImageUsageFlags usage,
	VkImageAspectFlags aspect)
{
	assert(!isValid());

	auto image = std::make_shared<VulkanImage>(
		device,
		width,
		height,
		depth,
		mipLevels,
		arrayLayers,
		format,
		imageType(type),
		usage);
	VulkanImageView::VulkanImageView(device, image, type, aspect);
}

VulkanImageView::VulkanImageView(
	VkDevice device,
	eRTextureType type, 
	eRFormat format, 
	uint32_t width, 
	uint32_t height, 
	uint32_t depth,
	uint32_t mipLevels, 
	uint32_t arrayLayers, 
	const void* data, 
	size_t dataSize)
{
	assert(!isValid());

	AssetTool::TextureBinary texBinary
	{
		type,
		(uint32_t)VulkanEnum::toFormat(format),
		width,
		height,
		depth,
		mipLevels,
		arrayLayers,
		dataSize
	};
	texBinary.Binary = data;
	texBinary.Images.resize(1u);
	uint32_t mipWidth = width;
	uint32_t mipHeight = height;
	size_t mipSize = dataSize;
	for (uint32_t i = 0u; i < mipLevels; ++i)
	{
		AssetTool::TextureBinary::Image image
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

	auto image = std::make_shared<VulkanImage>(device, texBinary);
	VulkanImageView::VulkanImageView(
		device, 
		image,
		VulkanEnum::toImageViewType(type),
		VK_IMAGE_ASPECT_COLOR_BIT);
}
