#pragma once

#include "VulkanBuffer.h"
#include "Colorful/Public/RAsset.h"

class vkSampler : public VulkanDeviceObject<VkSampler>, public rSampler
{
public:
	vkSampler(const class VulkanDevice &device, const rSamplerDesc &desc);
	void destroy(const class VulkanDevice &device) override final;
protected:
private:
};

class vkImage : public VulkanDeviceObject<VkImage>
{
public:
	vkImage() = default;
	vkImage(const class VulkanDevice &device, const rAsset::rTextureBinary &binary);

	void create(
		const class VulkanDevice &device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkFormat format,
		VkImageType type,
		VkImageUsageFlags usage);

	void destroy(const class VulkanDevice &device) override final;

	inline VkFormat getFormat() const
	{
		return m_Format;
	}
protected:
	friend class vkImageView;
	void transitionImageLayout();
	VkImageType getImageType(eRTextureType type) const;
	void copyBufferToImage(const class VulkanDevice &device, const rAsset::rTextureBinary &binary);
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
	vkDeviceMemory m_Memory;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
};
