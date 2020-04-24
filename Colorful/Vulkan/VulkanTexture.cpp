#include "VulkanTexture.h"
#include "VulkanEngine.h"

void vkImage::transitionImageLayout()
{
	/// The old layout must match the current layout of the image subresource range, with one exception. 
	/// The old layout can always be specified as VK_IMAGE_LAYOUT_UNDEFINED, though doing so invalidates the contents of the image subresource range.

	/// Setting the old layout to VK_IMAGE_LAYOUT_UNDEFINED implies that the contents of the image subresource need not be preserved. 
	/// Implementations may use this information to avoid performing expensive data transition operations.

	/// Applications must ensure that layout transitions happen-after all operations accessing the image with the old layout, 
	/// and happen-before any operations that will access the image with the new layout. 
	/// Layout transitions are potentially read/write operations, so not defining appropriate memory dependencies to guarantee this will result in a data race.

	/// The image layout is per-image subresource, and separate image subresources of the same image can be in different layouts at the same time with one exception - 
	/// depth and stencil aspects of a given image subresource must always be in the same layout.

	/// When performing a layout transition on an image subresource, the old layout value must either equal the current layout of the image subresource (at the time the transition executes), or else be VK_IMAGE_LAYOUT_UNDEFINED (implying that the contents of the image subresource need not be preserved). 
	/// The new layout used in a transition must not be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED.
}

VkImageType vkImage::getImageType(eRTextureType type) const
{
	VkImageType imageType = VK_IMAGE_TYPE_MAX_ENUM;
	switch (type)
	{
	case eTexture1D:
	case eTexture1DArray:
		imageType = VK_IMAGE_TYPE_1D;
		break;
	case eTexture2D:
	case eTexture2DArray:
	case eTextureCube:
	case eTextureCubeArray:
		imageType = VK_IMAGE_TYPE_2D;
		break;
	case eTexture3D:
		imageType = VK_IMAGE_TYPE_3D;
		break;
	}

	assert(imageType != VK_IMAGE_TYPE_MAX_ENUM);
	return imageType;
}

VkFormat vkImage::getImageFormat(eRFormat) const
{
	/// Need translate formats
	return VK_FORMAT_R8G8B8A8_UNORM;
}

void vkImage::copyBufferToImage(const vkDevice &device, const rAsset::rTextureBinary &binary)
{
	std::vector<VkBufferImageCopy> bufferImageCopy(binary.ArrayLayers * binary.MipLevels);
	uint32_t bufferOffsets = 0u;
	for (uint32_t i = 0u; i < binary.Images.size(); ++i)   /// array
	{
		for (uint32_t j = 0u; j < binary.Images[i].size(); ++j)  /// mip
		{
			/// bufferRowLength and bufferImageHeight specify in texels a subregion of a larger two- or three-dimensional image in buffer memory, 
			/// and control the addressing calculations. If either of these values is zero, 
			/// that aspect of the buffer memory is considered to be tightly packed according to the imageExtent.
			bufferImageCopy[i] = VkBufferImageCopy
			{
				bufferOffsets,
				0u,
				0u,
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					j,
					0u,
					1u
				},
				{
					0,
					0,
					0
				},
				{
					binary.Images[i][j].Width,
					binary.Images[i][j].Height,
					binary.Images[i][j].Depth,
				}
			};

			bufferOffsets += (uint32_t)binary.Images[i][j].Size;
		}
	}

	vkStagingBuffer stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, binary.Size, binary.Binary.get());
	vkCommandBuffer commandBuffer = device.allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	commandBuffer.begin();

	VkImageSubresourceRange subresourceRange
	{
		VK_IMAGE_ASPECT_COLOR_BIT,
		0u,
		binary.MipLevels,
		0u,
		binary.ArrayLayers
	};
	insertMemoryBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0u,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		subresourceRange);

	vkCmdCopyBufferToImage(
		*commandBuffer,
		*stagingBuffer, 
		**this, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		(uint32_t)bufferImageCopy.size(), 
		bufferImageCopy.data());

	insertMemoryBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subresourceRange);
	
	commandBuffer.end();

	device.freeCommandBuffer(commandBuffer);

	stagingBuffer.destroy(device);
}

void vkImage::insertMemoryBarrier(
	const vkCommandBuffer &commandBuffer, 
	VkPipelineStageFlags srcStageMask, 
	VkPipelineStageFlags dstStageMask, 
	VkAccessFlags srcAccessFlags,
	VkAccessFlags dstAccessFlags,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	const VkImageSubresourceRange &subresourceRange)
{
	/// When inserting a VkBufferMemoryBarrier or VkImageMemoryBarrier (see Pipeline Barriers), 
	/// a source and destination queue family index is specified to allow the ownership of a buffer or image to be transferred from one queue family to another. 
	VkImageMemoryBarrier imageMemoryBarrier
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		nullptr,
		srcAccessFlags,
		dstAccessFlags,
		oldLayout,
		newLayout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		**this,
		subresourceRange
	};
	vkCmdPipelineBarrier(
		*commandBuffer,
		srcStageMask,
		dstStageMask,
		0u,
		0u,
		nullptr,
		0u,
		nullptr,
		1u,
		&imageMemoryBarrier);
}

vkImage::vkImage(const vkDevice &device, const rAsset::rTextureBinary &binary)
{
	assert(!isValid() && binary.Size > 0u);

	VkImageCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0u,   /// flags is a bitmask of VkImageCreateFlagBits describing additional parameters of the image.
		getImageType(binary.Type),
		getImageFormat(binary.Format),
		{
			binary.Width,
			binary.Height,
			binary.Depth
		},
		binary.MipLevels,
		binary.ArrayLayers,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	VkImage handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateImage(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

	VkMemoryRequirements memoryRequirements{};
	vkGetImageMemoryRequirements(*device, handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadOnly, memoryRequirements);
	rVerifyVk(vkBindImageMemory(*device, handle, *m_Memory, 0u));

	copyBufferToImage(device, binary);
}

void vkImage::create(
	const vkDevice &device, 
	uint32_t width, 
	uint32_t height, 
	uint32_t depth, 
	uint32_t mipLevels,
	uint32_t arrayLayers,
	VkFormat format, 
	VkImageType type, 
	VkImageUsageFlags usage)
{
	assert(device.isValid() && !isValid());

	VkImageCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0u,
		type,
		format,
		{
			width,
			height,
			depth
		},
		mipLevels,
		arrayLayers,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	VkImage handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateImage(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);

	VkMemoryRequirements memoryRequirements{};
	vkGetImageMemoryRequirements(*device, handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadOnly, memoryRequirements);
	rVerifyVk(vkBindImageMemory(*device, handle, *m_Memory, 0u));
}

void vkImage::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		m_Memory.destroy(device);
		vkDestroyImage(*device, **this, vkMemoryAllocator);
		reset();
	}
}
