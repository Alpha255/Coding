#pragma once

#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "Colorful/Public/RAsset.h"

class vkTexture : public vkDeviceObject<VkImage>, public rTexture
{
public:
	vkTexture(const class vkDevice &device);
	vkTexture(const class vkDevice &device, const rAsset::rTextureBinary &binary);
	void destroy(const class vkDevice &device) override final;
protected:
	void transitionImageLayout();
	VkImageType getImageType(eRTextureType type);
	VkFormat getImageFormat(eRFormat format);
	void copyBufferToImage(const class vkDevice &device, const rAsset::rTextureBinary &binary);
	void insertMemoryBarrier(
		const class vkCommandBufferArray &command, 
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

class vkTexture1D : public vkTexture
{

};

class vkTexture2D : public vkTexture
{

};

class vkTexture3D : public vkTexture
{

};

class vkTextureCube : public vkTexture
{

};

class vkTexture1DArray : public vkTexture
{

};

class vkTexture2DArray : public vkTexture
{

};

class vkTexture3DArray : public vkTexture
{

};

class vkTextureCubeArray : public vkTexture
{

};
