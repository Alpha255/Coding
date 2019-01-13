#include "VulkanTexture.h"
#include "VulkanEngine.h"
#include "VulkanBuffer.h"

void VulkanTexture2D::Create(
	uint32_t fmt,
	uint32_t width,
	uint32_t height,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t arraySize,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	uint32_t usage,
	const VulkanSubResourceData *pSubResourceData)
{
	VulkanImage::Create(
		eImage2D,
		width,
		height,
		1U,
		fmt,
		mipLevels,
		arraySize,
		usage,
		VK_IMAGE_LAYOUT_UNDEFINED,
		pSubResourceData);
}