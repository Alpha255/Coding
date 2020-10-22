#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanSampler)
class VulkanSampler final : public VkObject<VkSampler_T>
{
public:
	VulkanSampler(VkDevice device, const SamplerDesc& desc);
	
	void destroy(VkDevice device)
	{

	}
protected:
private:
};

DECLARE_SHARED_PTR(VulkanImage)
class VulkanImage final : public VkObject<VkImage_T>
{
public:
	static void makeMemoryBarrierFlags(
		eImageLayout layout, 
		__out VkPipelineStageFlags& stageFlags, 
		__out VkAccessFlags& accessFlags, 
		__out VkImageLayout& imageLayout);

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

	void queueCopyCommand(const AssetTool::TextureBinary& binary);
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

NAMESPACE_END(Gfx)
