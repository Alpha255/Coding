#include "VulkanView.h"
#include "VulkanEngine.h"
#include "Util/RawTexture.h"

void VulkanImage::Create(
	uint32_t type,
	uint32_t width, 
	uint32_t height, 
	uint32_t depth,
	uint32_t format, 
	uint32_t mipLevels, 
	uint32_t arraySize,
	uint32_t usage,
	uint32_t layout)
{
	assert(!IsValid() && type <= VK_IMAGE_TYPE_END_RANGE);

	m_Property = 
	{ 
		type,
		width,
		height,
		format,
		mipLevels,
		usage,
		layout
	};

	VkImageCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0U,
		(VkImageType)type,
		(VkFormat)format,
		{ width, height, depth },
		mipLevels,
		arraySize,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		(VkImageUsageFlags)usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0U,
		nullptr,
		(VkImageLayout)layout
	};

	VKCheck(vkCreateImage(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));

	VkMemoryRequirements memReq = {};
	vkGetImageMemoryRequirements(VulkanEngine::Instance().GetDevice(), m_Handle, &memReq);
	m_Memory.Alloc(memReq.size, VulkanDeviceMemory::GetMemoryTypeIndex(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	VKCheck(vkBindImageMemory(VulkanEngine::Instance().GetDevice(), m_Handle, m_Memory.Get(), 0));
}

void VulkanImageView::Create(VulkanImage &image)
{
	assert(!IsValid() && image.IsValid());

	auto imageProperty = image.GetProperty();
	VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
	if (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT == imageProperty.Usage)
	{
		aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT == imageProperty.Usage)
	{
		aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
	{
		System::Log("Not supported yet!!");
		assert(0);
	}

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0U,
		image.Get(),
		(VkImageViewType)imageProperty.Type,
		(VkFormat)imageProperty.Format,
		{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
		{ aspectFlags, 0U, imageProperty.MipLevels, 0U, 1U }
	};

	VKCheck(vkCreateImageView(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanImageView::CreateAsTexture(eRViewType type, VulkanImage &image, uint32_t fmt, uint32_t mipSlice, uint32_t aspectFlags)
{
	assert(!IsValid());

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0U,
		image.Get(),
		(VkImageViewType)type,
		(VkFormat)fmt,
		{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
		{ aspectFlags, 0U, mipSlice, 0U, 1U }
	};

	VKCheck(vkCreateImageView(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanShaderResourceView::Create(const char *pFileName)
{
	RawTexture rawDds;
	rawDds.CreateFromDds(pFileName, true);

	VulkanImage image;
	image.Create(
		rawDds.Dimension, 
		rawDds.Width, 
		rawDds.Height, 
		rawDds.Depth,
		rawDds.Format, 
		rawDds.MipLevels, 
		rawDds.ArraySize,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED);

	uint32_t subResCount = rawDds.ArraySize * rawDds.MipLevels;

	VulkanBuffer subResBuffer;
	subResBuffer.CreateAsSrcDynamicBuffer(rawDds.RawSize + 4 * subResCount);  /// pad block, as we may need to align the data to 4 bytes

	std::unique_ptr<VkBufferImageCopy> subResCopys(new VkBufferImageCopy[subResCount]());
	auto pSubResCopy = subResCopys.get();
	uint32_t offset = 0U;

	for (uint32_t i = 0U; i < rawDds.ArraySize; ++i)
	{
		uint32_t width = rawDds.Width;
		uint32_t height = rawDds.Height;

		for (uint32_t j = 0U; j < rawDds.MipLevels; ++j)
		{
			subResBuffer.Update(rawDds.RawMipTextures[j].RawMipData, rawDds.RawMipTextures[j].MipSize, offset);

			*pSubResCopy = VkBufferImageCopy
			{
				offset,
				0U,
				0U,
				{ VK_IMAGE_ASPECT_COLOR_BIT, j, i, 1 },
				{ 0, 0, 0 },
				{ width, height, rawDds.Depth }
			};

			++pSubResCopy;

			offset += ((rawDds.RawMipTextures[j].MipSize + 3 & (~0x03)));

			width >>= 1;
			width = width ? width : 1;

			height >>= 1;
			height = height ? height : 1;
		}
	}

	VulkanCommandBuffer tempBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eTemp, VulkanCommandPool::ePrimary);
	tempBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	{

	}

	vkCmdCopyBufferToImage(tempBuffer.Get(), subResBuffer.Get(), image.Get(), VK_IMAGE_LAYOUT_GENERAL, subResCount, subResCopys.get());
}