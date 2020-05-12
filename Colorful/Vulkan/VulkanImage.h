#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"
#include "AssetTool/Asset.h"

class VulkanSampler : public VulkanDeviceObject<VkSampler>, public GfxSampler
{
public:
	VulkanSampler(VkDevice device, const GfxSamplerDesc& desc);
	void destroy(VkDevice device) override final;
protected:
private:
};

class VulkanImage : public VulkanDeviceObject<VkImage>
{
public:
	VulkanImage(VkDevice device, const AssetTool::TextureBinary& binary);

	VulkanImage(
		VkDevice device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		VkFormat format,
		VkImageType type,
		VkImageUsageFlags usage);

	void destroy(VkDevice device) override final
	{
		if (isValid())
		{
			m_Memory.destroy(device);
			vkDestroyImage(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}

	inline VkFormat getFormat() const
	{
		return m_Format;
	}
protected:
	friend class VulkanImageView;
	void transitionImageLayout();
	VkImageType getImageType(eRTextureType type) const;
	void copyBufferToImage(VkDevice device, const AssetTool::TextureBinary &binary);
	void insertMemoryBarrier(
		const class VulkanCommandBuffer &commandBuffer, 
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
using VulkanImagePtr = std::shared_ptr<VulkanImage>;
