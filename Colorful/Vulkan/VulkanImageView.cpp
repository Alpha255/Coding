#include "Colorful/Vulkan/VulkanImageView.h"
#include "Colorful/Vulkan/VulkanMap.h"

NAMESPACE_START(Gfx)

VulkanImageView::VulkanImageView(VkDevice device, const TextureDesc& desc, uint32_t usageFlags, uint32_t aspectFlags, VkImageLayout layout)
{
	assert(device);

	VkFormat format = VulkanMap::format(desc.Format);
	VkImageCreateInfo imageCreateInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0u,   /// flags is a bitmask of VkImageCreateFlagBits describing additional parameters of the image.
		VulkanMap::imageType(desc.Dimension),
		format,
		{
			desc.Width,
			desc.Height,
			desc.Depth
		},
		desc.MipLevels,
		desc.ArraySize,
		VulkanMap::samplerCount(desc.SampleCount),
		VK_IMAGE_TILING_OPTIMAL,
		usageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		layout
	};
	VERIFY_VK(vkCreateImage(device, &imageCreateInfo, VK_MEMORY_ALLOCATOR, &m_Image));

	VkMemoryRequirements memReq{};
	vkGetImageMemoryRequirements(device, m_Image, &memReq);
	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memReq.size,
		0u /// ??? 
		///VulkanBufferPool::instance()->memoryTypeIndex(usage, requirements.memoryTypeBits)
	};
	VERIFY_VK(vkAllocateMemory(device, &allocateInfo, VK_MEMORY_ALLOCATOR, &m_Buffer));
	VERIFY_VK(vkBindImageMemory(device, m_Image, m_Buffer, 0u));

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		m_Image,
		VulkanMap::imageViewType(desc.Dimension),
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
			desc.MipLevels,
			0u,
			desc.ArraySize
		}
	};
	VERIFY_VK(vkCreateImageView(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

/// From UE4
void VulkanImageView::getMemoryBarrierFlags(
	EImageLayout layout,
	VkPipelineStageFlags& stageFlags,
	VkAccessFlags& accessFlags,
	VkImageLayout& imageLayout)
{
	stageFlags = (VkPipelineStageFlags)0;

	switch (layout)
	{
	case EImageLayout::Undefined:
		accessFlags = 0;
		stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		break;
	case EImageLayout::TransferDst:
		accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		break;
	case EImageLayout::ColorAttachment:
		accessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case EImageLayout::DepthStencilAttachment:
		accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	case EImageLayout::TransferSrc:
		accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		break;
	case EImageLayout::Present:
		accessFlags = 0u;
		stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	case EImageLayout::FragmentShaderRead:
		accessFlags = VK_ACCESS_SHADER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case EImageLayout::PixelDepthStencilRead:
		accessFlags = VK_ACCESS_SHADER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		break;
	case EImageLayout::ComputeShaderReadWrite:
		accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	case EImageLayout::FragmentShaderReadWrite:
		accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	default:
		assert(0);
	}
}

NAMESPACE_END(Gfx)
