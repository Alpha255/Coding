#pragma once

#include "VulkanObject.h"
#include "VulkanBuffer.h"

struct VulkanSubResourceData
{
	const void *Memory = nullptr;
	uint32_t MemPitch = 0U;
	uint32_t MemSlicePitch = 0U;
};

class VulkanImage : public VulkanObject<VkImage>
{
public:
	enum eImageType
	{
		eImage1D = VK_IMAGE_TYPE_1D,
		eImage2D = VK_IMAGE_TYPE_2D,
		eImage3D = VK_IMAGE_TYPE_3D
	};

	void Create(
		uint32_t type,
		uint32_t width, 
		uint32_t height, 
		uint32_t depth,
		uint32_t format, 
		uint32_t mipLevels, 
		uint32_t arraySize,
		uint32_t usage,
		uint32_t layout,
		const VulkanSubResourceData *pSubResourceData = nullptr);

	void Destory() override;

	inline const VkImageCreateInfo &GetImageInfo() const
	{
		assert(IsValid());
		return m_CreateInfo;
	}
protected:
private:
	VulkanDeviceMemory m_Memory;
	VkImageCreateInfo m_CreateInfo = {};
};

class VulkanImageView : public VulkanObject<VkImageView>
{
public:
	void Create(const VulkanImage &image, bool bKeepImage = true);
	void CreateAsTexture(eRViewType type, const VulkanImage &image, uint32_t fmt, uint32_t mipSlice, uint32_t aspectFlags, bool bKeepImage = true);
	void Destory() override;

	inline const VkImageViewCreateInfo &GetImageViewInfo() const
	{
		assert(IsValid());
		return m_CreateInfo;
	}
protected:
private:
	VkImageViewCreateInfo m_CreateInfo = {};
	VulkanImage m_Image;
};

class VulkanRenderTargetView : public VulkanImageView
{
public:
	inline void CreateAsTexture(eRViewType type, const VulkanImage &image, uint32_t fmt, uint32_t mipSlice, bool bKeepImage = true)
	{
		VulkanImageView::CreateAsTexture(type, image, fmt, mipSlice, VK_IMAGE_ASPECT_COLOR_BIT, bKeepImage);
	}
protected:
private:
};

class VulkanDepthStencilView : public VulkanImageView
{
public:
	///void Create(uint32_t fmt, uint32_t width, uint32_t height);

	inline void CreateAsTexture(eRViewType type, const VulkanImage &image, uint32_t fmt, uint32_t, uint32_t mipSlice, bool bKeepImage = true)
	{
		VulkanImageView::CreateAsTexture(type, image, fmt, mipSlice, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, bKeepImage);
	}
protected:
private:
};

class VulkanShaderResourceView : public VulkanImageView
{
public:
	void Create(const std::string &fileName);
protected:
	///RawTexture rawDds;
private:
};