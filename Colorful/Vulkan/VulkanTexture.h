#pragma once

#include "VulkanBuffer.h"
#include "Colorful/Public/RAsset.h"

class vkSampler : public vkDeviceObject<VkSampler>
{

};

class vkImage : public vkDeviceObject<VkImage>, public rTexture
{
public:
	vkImage() = default;
	vkImage(const class vkDevice &device, const rAsset::rTextureBinary &binary);

	void create(
		const class vkDevice &device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkFormat format,
		VkImageType type,
		VkImageUsageFlags usage);

	void destroy(const class vkDevice &device) override final;
protected:
	friend class vkImageView;
	void transitionImageLayout();
	VkImageType getImageType(eRTextureType type) const;
	VkFormat getImageFormat(eRFormat format) const;
	void copyBufferToImage(const class vkDevice &device, const rAsset::rTextureBinary &binary);
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
};
