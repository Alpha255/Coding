#include "VulkanTexture.h"
#include "VulkanEngine.h"
#include "VulkanBuffer.h"

//void VulkanTexture2D::Create(
//	uint32_t format, 
//	uint32_t width, 
//	uint32_t height, 
//	uint32_t depth,
//	uint32_t mipLevels,
//	uint32_t arrayLayers,
//	uint32_t samples, 
//	uint32_t usage, 
//	uint32_t flags,
//	uint32_t aspectFlags)
//{
//	VulkanImage image;
//	image.Create(
//		format,
//		width,
//		height,
//		depth,
//		mipLevels,
//		arrayLayers,
//		samples,
//		usage,
//		flags);
//
//	VkMemoryRequirements memoryRequiments{};
//	vkGetImageMemoryRequirements(VulkanEngine::Instance().GetDevice(), image.Get(), &memoryRequiments);
//
//	VulkanDeviceMemory memory;
//	memory.Alloc(memoryRequiments.size, VulkanDeviceMemory::GetMemoryType(memoryRequiments.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
//
//	VKCheck(vkBindImageMemory(VulkanEngine::Instance().GetDevice(), image.Get(), memory.Get(), 0U));
//
//	VkImageViewCreateInfo imageViewCreateInfo
//	{
//		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//		nullptr,
//		flags,
//		image.Get(),
//		VK_IMAGE_VIEW_TYPE_2D,
//		(VkFormat)format,
//		{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
//		{ (VkImageAspectFlags)aspectFlags, 0U, 1U, 0U, 1U }
//	};
//	if (format == VK_FORMAT_D16_UNORM_S8_UINT ||
//		format == VK_FORMAT_D24_UNORM_S8_UINT ||
//		format == VK_FORMAT_D32_SFLOAT_S8_UINT)
//	{
//		imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//	}
//
//	VKCheck(vkCreateImageView(VulkanEngine::Instance().GetDevice(), &imageViewCreateInfo, nullptr, &m_Handle));
//}