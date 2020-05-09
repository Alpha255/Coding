#pragma once

#include "VulkanBuffer.h"
#include "Colorful/Public/RAsset.h"

class vkSampler : public VulkanDeviceObject<VkSampler>, public rSampler
{
public:
	vkSampler(VkDevice device, const GfxSamplerDesc &desc);
	void destroy(VkDevice device) override final;
protected:
private:
};

class vkImage : public VulkanDeviceObject<VkImage>
{
public:
	vkImage() = default;
	vkImage(VkDevice device, const rAsset::rTextureBinary &binary);

	void create(
		VkDevice device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkFormat format,
		VkImageType type,
		VkImageUsageFlags usage);

	void destroy(VkDevice device) override final;

	inline VkFormat getFormat() const
	{
		return m_Format;
	}
protected:
	friend class vkImageView;
	void transitionImageLayout();
	VkImageType getImageType(eRTextureType type) const;
	void copyBufferToImage(VkDevice device, const rAsset::rTextureBinary &binary);
	void insertMemoryBarrier(
		const class vkCommandBuffer &commandBuffer, 
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkAccessFlags srcAccessFlags,
		VkAccessFlags dstAccessFlags,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		const VkImageSubresourceRange &subresourceRange);
private:
	VulkanDeviceMemory m_Memory;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
};
