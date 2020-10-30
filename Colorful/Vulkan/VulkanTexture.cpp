#include "Colorful/Vulkan/VulkanTexture.h"
#include "Colorful/Vulkan/VulkanMap.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"

NAMESPACE_START(Gfx)

VulkanTexture::VulkanTexture(VkDevice device, const TextureDesc& desc, ECreationMode mode)
{
	assert(device);

	VkImageCreateInfo imageCreateInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0u,  /// VkImageCreateFlagBits
		VulkanMap::imageType(desc.Dimension),
		VulkanMap::format(desc.Format),
		{
			desc.Width,
			desc.Height,
			desc.Depth
		},
		desc.MipLevels,
		desc.ArraySize,
		VulkanMap::samplerCount(desc.SampleCount),
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};
	VERIFY_VK(vkCreateImage(device, &imageCreateInfo, VK_MEMORY_ALLOCATOR, &m_Image));

	VkMemoryRequirements memReq{};
	vkGetImageMemoryRequirements(device, m_Image, &memReq);
	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memReq.size,
		VulkanMemoryAllocator::instance()->getMemoryTypeIndex(EBufferUsage::Immutable, memReq.memoryTypeBits),
	};
	VERIFY_VK(vkAllocateMemory(device, &allocateInfo, VK_MEMORY_ALLOCATOR, &m_Memory));
	VERIFY_VK(vkBindImageMemory(device, m_Image, m_Memory, 0u));

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		m_Image,
		VulkanMap::imageViewType(desc.Dimension),
		VulkanMap::format(desc.Format),
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		{
			VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT,
			0u,
			desc.MipLevels,
			0u,
			desc.ArraySize
		}
	};
	VERIFY_VK(vkCreateImageView(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));

	bool8_t useStaging = true;
	if (useStaging)
	{
		std::vector<VkBufferImageCopy> copies(desc.Subresources.size());
		for (uint32_t i = 0u; i < desc.Subresources.size(); ++i)
		{
			copies[i] = VkBufferImageCopy
			{
				desc.Subresources[i].Offset,
				desc.Subresources[i].RowBytes,
				desc.Subresources[i].Height,
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					desc.Subresources[i].MipIndex,
					desc.Subresources[i].ArrayIndex,
					1u
				},
				{ 0u, 0u, 0u },
				{
					desc.Subresources[i].Width,
					desc.Subresources[i].Height,
					desc.Subresources[i].Depth
				}
			};
		}

		VkImageSubresourceRange subresourceRange
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0u,
			desc.MipLevels,
			0u,
			desc.ArraySize
		};

		if (mode == ECreationMode::Immediately)
		{

		}
		else
		{

		}
	}
	else
	{
		assert(0);
	}
}

void VulkanTexture::getBarrierFlags(ELayout layout, VkPipelineStageFlags& stageFlags, VkAccessFlags& accessFlags, VkImageLayout& imageLayout)
{
	stageFlags = 0u;

	switch (layout)
	{
	case ELayout::Undefined:
		accessFlags = 0;
		stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		break;
	case ELayout::TransferDst:
		accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		break;
	case ELayout::ColorAttachment:
		accessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case ELayout::DepthStencilAttachment:
		accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	case ELayout::TransferSrc:
		accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		break;
	case ELayout::Present:
		accessFlags = 0u;
		stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	case ELayout::FragmentShaderRead:
		accessFlags = VK_ACCESS_SHADER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case ELayout::PixelDepthStencilRead:
		accessFlags = VK_ACCESS_SHADER_READ_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		break;
	case ELayout::ComputeShaderReadWrite:
		accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	case ELayout::FragmentShaderReadWrite:
		accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	}
}

VulkanSampler::VulkanSampler(VkDevice device, const SamplerDesc& desc)
{
	assert(device);

	VkSamplerCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0u,
		VulkanMap::textureFilter(desc.MinMagFilter),
		VulkanMap::textureFilter(desc.MinMagFilter),
		desc.MinMagFilter == ETextureFilter::Nearest ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VulkanMap::samplerAddressMode(desc.AddressModeU),
		VulkanMap::samplerAddressMode(desc.AddressModeV),
		VulkanMap::samplerAddressMode(desc.AddressModeW),
		desc.MipLodBias,
		desc.MinMagFilter == ETextureFilter::Anisotropic ? true : false,
		static_cast<float32_t>(desc.MaxAnisotropy),
		false,
		VulkanMap::compareFunc(desc.CompareOp),
		desc.MinLod,
		desc.MaxLod,
		VulkanMap::borderColor(desc.BorderColor),
		false
	};
	VERIFY_VK(vkCreateSampler(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

NAMESPACE_END(Gfx)