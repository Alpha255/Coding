#if 0
#include "Colorful/Vulkan/VulkanEngine.h"

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

void VulkanImage::queueCopyCommand(const AssetTool::TextureBinary& binary)
{
	bool8_t useStaging = true;
	//VkFormatProperties formatProperties = VulkanEngine::instance().getFormatProperties(m_Format);
	//useStaging = !(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

	if (useStaging)
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
				bufferImageCopy[i + j] = VkBufferImageCopy
				{
					bufferOffsets,
					0u,
					0u,
					{
						VK_IMAGE_ASPECT_COLOR_BIT,
						j,
						i,
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

		VkImageSubresourceRange subresourceRange
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0u,
			binary.MipLevels,
			0u,
			binary.ArrayLayers
		};

		VulkanQueueManager::instance()->queueImageCopyCommand(
			Handle,
			eTransferDst,
			eFragmentShaderRead,
			subresourceRange,
			bufferImageCopy,
			binary.Size,
			binary.Binary ? binary.Binary : binary.SharedBinary.get());
	}
	else
	{
		assert(0);
	}
}

VulkanImage::VulkanImage(VkDevice device, const AssetTool::TextureBinary& binary)
	: VulkanImage(
		binary.Width,
		binary.Height,
		binary.Depth,
		binary.MipLevels,
		binary.ArrayLayers,
		VulkanEnum::toImageType(binary.Type),
		(VkFormat)binary.Format)
{
	assert(!isValid() && binary.Size > 0u);

	VkImageCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0u,   /// flags is a bitmask of VkImageCreateFlagBits describing additional parameters of the image.
		m_Type,
		m_Format,
		{
			m_Width,
			m_Height,
			m_Depth
		},
		m_MipLevels,
		m_ArrayLayers,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	GfxVerifyVk(vkCreateImage(device, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetImageMemoryRequirements(device, Handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadOnly, memoryRequirements);
	GfxVerifyVk(vkBindImageMemory(device, Handle, m_Memory.Handle, 0u));

	queueCopyCommand(binary);
}

VulkanImage::VulkanImage(
	VkDevice device, 
	uint32_t width, 
	uint32_t height, 
	uint32_t depth, 
	uint32_t mipLevels,
	uint32_t arrayLayers,
	VkFormat format, 
	VkImageType type, 
	VkImageUsageFlags usage)
	: VulkanImage(width, height, depth, mipLevels, arrayLayers, type, format)
{
	assert(!isValid());

	VkImageCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0u,
		m_Type,
		m_Format,
		{
			m_Width,
			m_Height,
			m_Depth
		},
		m_MipLevels,
		m_ArrayLayers,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	GfxVerifyVk(vkCreateImage(device, &createInfo, vkMemoryAllocator, &Handle));

	VkMemoryRequirements memoryRequirements{};
	vkGetImageMemoryRequirements(device, Handle, &memoryRequirements);
	m_Memory.create(device, eGpuReadOnly, memoryRequirements);
	GfxVerifyVk(vkBindImageMemory(device, Handle, m_Memory.Handle, 0u));
}

VulkanSampler::VulkanSampler(VkDevice device, const GfxSamplerDesc& desc)
{
	assert(!isValid());
	assert(desc.MaxAnisotropy < 16u);

	VkSamplerCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0u,
		VulkanEnum::toFilter(desc.MinMagFilter),
		VulkanEnum::toFilter(desc.MinMagFilter),
		VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VulkanEnum::toAddressMode(desc.AddressModeU),
		VulkanEnum::toAddressMode(desc.AddressModeV),
		VulkanEnum::toAddressMode(desc.AddressModeW),
		desc.MipLodBias,
		desc.MinMagFilter == eAnisotropic ? true : false,
		(float32_t)desc.MaxAnisotropy,
		false,
		VulkanEnum::toCompareOp(desc.CompareOp),
		desc.MinLod,
		desc.MaxLod,
		VulkanEnum::toBorderColor(desc.BorderColor),
		false
	};
	
	GfxVerifyVk(vkCreateSampler(device, &createInfo, vkMemoryAllocator, &Handle));
}

void VulkanSampler::destroy(VkDevice device)
{
	if (isValid())
	{
		vkDestroySampler(device, Handle, vkMemoryAllocator);
		Handle = VK_NULL_HANDLE;
	}
}
#endif