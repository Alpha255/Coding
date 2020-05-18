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
using VulkanSamplerPtr = std::shared_ptr<VulkanSampler>;

class VulkanImage : public VulkanDeviceObject<VkImage>
{
public:
	VulkanImage(VkImage handle, VkFormat format)
		: m_Format(format)
	{
		Handle = handle;
	}

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
		if (isValid() && m_Memory.isValid())
		{
			m_Memory.destroy(device);

			vkDestroyImage(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
		else
		{
			Handle = VK_NULL_HANDLE; /// For BackBuffer
		}
	}

	inline uint32_t mipLevels() const
	{
		return m_MipLevels;
	}

	inline uint32_t arrayLayers() const
	{
		return m_ArrayLayers;
	}

	inline VkFormat format() const
	{
		return m_Format;
	}
protected:
	VulkanImage(uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t arrayLayers, VkImageType type, VkFormat format)
		: m_Width(width)
		, m_Height(height)
		, m_Depth(depth)
		, m_MipLevels(mipLevels)
		, m_ArrayLayers(arrayLayers)
		, m_Type(type)
		, m_Format(format)
	{
	}

	void transitionImageLayout();
	void queueCopyCommand(VkDevice device, const AssetTool::TextureBinary& binary);
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

	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_Depth = 0u;
	uint32_t m_MipLevels = 1u;
	uint32_t m_ArrayLayers = 1u;
	VkImageType m_Type = VK_IMAGE_TYPE_MAX_ENUM;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
};
using VulkanImagePtr = std::shared_ptr<VulkanImage>;
