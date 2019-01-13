#pragma once

#include "VulkanObject.h"
#include "VulkanBuffer.h"
#include "Util/RawTexture.h"

class VulkanImage : public VulkanObject<VkImage>
{
public:
	enum eImageType
	{
		eImage1D = VK_IMAGE_TYPE_1D,
		eImage2D = VK_IMAGE_TYPE_2D,
		eImage3D = VK_IMAGE_TYPE_3D
	};

	struct ImageProperty
	{
		uint32_t Type = 0U;
		uint32_t Width = 0U;
		uint32_t Height = 0U;
		uint32_t Format = 0U;
		uint32_t MipLevels = 0U;
		uint32_t Usage = 0U;
		uint32_t Layout = 0U;
	};

	inline VulkanImage(VkImage image)
		: VulkanObject<VkImage>(image)
	{
	}

	inline VulkanImage() = default;
	inline ~VulkanImage() = default;

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
		const struct VulkanSubResourceData *pSubResourceData = nullptr);

	inline ImageProperty GetProperty() const
	{
		assert(IsValid());
		return m_Property;
	}
protected:
private:
	ImageProperty m_Property = {};
	VulkanDeviceMemory m_Memory;
};

class VulkanImageView : public VulkanObject<VkImageView>
{
public:
	void Create(VulkanImage &image);
	void CreateAsTexture(eRViewType type, VulkanImage &image, uint32_t fmt, uint32_t mipSlice, uint32_t aspectFlags);
protected:
private:
};

class VulkanRenderTargetView : public VulkanImageView
{
public:
	inline void CreateAsTexture(eRViewType type, VulkanImage &image, uint32_t fmt, uint32_t mipSlice)
	{
		VulkanImageView::CreateAsTexture(type, image, fmt, mipSlice, VK_IMAGE_ASPECT_COLOR_BIT);
	}
protected:
private:
};

class VulkanDepthStencilView : public VulkanImageView
{
public:
	void Create(uint32_t fmt, uint32_t width, uint32_t height);

	inline void CreateAsTexture(eRViewType type, VulkanImage &image, uint32_t fmt, uint32_t, uint32_t mipSlice)
	{
		VulkanImageView::CreateAsTexture(type, image, fmt, mipSlice, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	}
protected:
private:
};

class VulkanShaderResourceView : public VulkanImageView
{
public:
	void Create(const char *pFileName);
protected:
	RawTexture rawDds;
private:
};