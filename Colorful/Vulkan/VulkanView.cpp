#include "VulkanView.h"
#include "VulkanEngine.h"

void VulkanImage::Create(
	uint32_t type,
	uint32_t width, 
	uint32_t height, 
	uint32_t depth,
	uint32_t format, 
	uint32_t mipLevels, 
	uint32_t arraySize,
	uint32_t usage,
	uint32_t layout,
	const VulkanSubResourceData *)
{
	assert(!IsValid() && type <= VK_IMAGE_TYPE_END_RANGE);

	m_CreateInfo = VkImageCreateInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0U,
		(VkImageType)type,
		(VkFormat)format,
		{ 
			width, 
			height, 
			depth 
		},
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

	Check(vkCreateImage(VulkanEngine::Instance().GetDevice().Get(), &m_CreateInfo, nullptr, &m_Handle));

	VkMemoryRequirements memRequirements = {};
	vkGetImageMemoryRequirements(VulkanEngine::Instance().GetDevice().Get(), m_Handle, &memRequirements);
	m_Memory.Alloc(memRequirements.size, memRequirements.memoryTypeBits, eGpuReadOnly);

	Check(vkBindImageMemory(VulkanEngine::Instance().GetDevice().Get(), m_Handle, m_Memory.Get(), 0));

#if 0
	if (pSubResourceData)
	{
		if (!m_CommandBuffer.IsValid())
		{
			m_CommandBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eGeneral, VulkanCommandPool::ePrimary);
		}

		m_CommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		/// Upload to Buffer
		m_Buffer.CreateAsTransferBuffer(memReq.size, eGpuCopyToCpu, true);
		m_Buffer.Update(pSubResourceData->Memory, memReq.size, 0U, true);

		/// Copy to Image
		VkImageMemoryBarrier copy_barrier[1] = {};
		copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier[0].image = m_Handle;
		copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_barrier[0].subresourceRange.levelCount = 1;
		copy_barrier[0].subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(m_CommandBuffer.Get(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

		VkBufferImageCopy region = {};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageExtent.width = width;
		region.imageExtent.height = height;
		region.imageExtent.depth = 1;
		vkCmdCopyBufferToImage(m_CommandBuffer.Get(), m_Buffer.Get(), m_Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VkImageMemoryBarrier use_barrier[1] = {};
		use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier[0].image = m_Handle;
		use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		use_barrier[0].subresourceRange.levelCount = 1;
		use_barrier[0].subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(m_CommandBuffer.Get(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);

		m_CommandBuffer.End();

		VkSubmitInfo info
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0U,
			nullptr,
			nullptr,
			1U,
			&m_CommandBuffer,
			0U,
			nullptr
		};
		VKCheck(vkQueueSubmit(VulkanEngine::Instance().GetQueue(), 1U, &info, VK_NULL_HANDLE));

		VKCheck(vkDeviceWaitIdle(VulkanEngine::Instance().GetDevice()));
	}
#endif
}

void VulkanImage::Destory()
{
	assert(IsValid());

	vkDestroyImage(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	m_Memory.Destory();

	m_CreateInfo = {};

	Reset();
}

void VulkanImageView::Create(VulkanImage &)
{
#if 0
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
#endif
}

void VulkanImageView::CreateAsTexture(eRViewType, VulkanImage &, uint32_t, uint32_t, uint32_t)
{
#if 0
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
#endif
}

void VulkanImageView::Destory()
{
	assert(IsValid());

	vkDestroyImageView(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}

void VulkanShaderResourceView::Create(const std::string &)
{
#if 0
	rawDds.CreateFromDds(pFileName, true);

	//VulkanImage image;
	//image.Create(
	//	rawDds.Dimension, 
	//	rawDds.Width, 
	//	rawDds.Height, 
	//	rawDds.Depth,
	//	rawDds.Format, 
	//	rawDds.MipLevels, 
	//	rawDds.ArraySize,
	//	VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
	//	VK_IMAGE_LAYOUT_UNDEFINED);

	VulkanBuffer subResBuffer;
	subResBuffer.CreateAsTransferBuffer(rawDds.RawSize, eGpuReadCpuWrite);
	subResBuffer.Update(rawDds.RawData.get(), rawDds.RawSize, 0U);

	uint32_t subResCount = rawDds.ArraySize * rawDds.MipLevels;

	std::unique_ptr<VkBufferImageCopy> subResCopys(new VkBufferImageCopy[subResCount]());
	auto pSubResCopy = subResCopys.get();
	uint32_t offset = 0U;

	for (uint32_t i = 0U; i < rawDds.ArraySize; ++i)
	{
		for (uint32_t j = 0U; j < rawDds.MipLevels; ++j)
		{
			*pSubResCopy = VkBufferImageCopy
			{
				offset,
				0U,
				0U,
				{ VK_IMAGE_ASPECT_COLOR_BIT, j, i, 1 },
				{ 0, 0, 0 },
				{ rawDds.RawMipTextures[j].MipWidth, rawDds.RawMipTextures[j].MipHeight, rawDds.Depth }
			};

			++pSubResCopy;

			offset += ((rawDds.RawMipTextures[j].MipSize + 3 & (~0x03)));
		}
	}

	VulkanImage image2D;
	image2D.Create(
		rawDds.Dimension,
		rawDds.Width,
		rawDds.Height,
		rawDds.Depth,
		VK_FORMAT_R8G8B8A8_UNORM,
		rawDds.MipLevels,
		rawDds.ArraySize,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED);

	VulkanCommandBuffer tempBuffer = VulkanEngine::Instance().AllocCommandBuffer(VulkanCommandPool::eTemp, VulkanCommandPool::ePrimary);
	tempBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkImageMemoryBarrier imageMemoryBarrier
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		nullptr,
		0U,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		image2D.Get(),
		{ VK_IMAGE_ASPECT_COLOR_BIT, 0U, rawDds.MipLevels, 0U, rawDds.ArraySize }
	};
	vkCmdPipelineBarrier(tempBuffer.Get(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0U, 0U, nullptr, 0U, nullptr, 1, &imageMemoryBarrier);
	vkCmdCopyBufferToImage(tempBuffer.Get(), subResBuffer.Get(), image2D.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subResCount, subResCopys.get());

	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkCmdPipelineBarrier(tempBuffer.Get(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	tempBuffer.End();

	VulkanFence fence;
	fence.Create();

	VkSubmitInfo submitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0U,
		nullptr,
		nullptr,
		1U,
		&tempBuffer,
		0U,
		nullptr
	};
	
	VKCheck(vkQueueSubmit(VulkanEngine::Instance().GetQueue(), 1U, &submitInfo, fence.Get()));
	VKCheck(vkWaitForFences(VulkanEngine::Instance().GetDevice(), 1U, &fence, VK_TRUE, UINT32_MAX));
#endif
}