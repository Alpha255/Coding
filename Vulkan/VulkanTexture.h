#pragma once

#include "VulkanObject.h"

class VulkanImage : public VulkanObject<VkImage>
{
public:
	void Create(
		uint32_t format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		uint32_t samples,
		uint32_t usage,
		uint32_t flags);
protected:
private:
};

class VulkanTexture2D : public VulkanObject<VkImageView>
{
public:
	void Create(
		uint32_t format, 
		uint32_t width, 
		uint32_t height, 
		uint32_t depth, 
		uint32_t mipLevels,
		uint32_t arrayLayers,
		uint32_t samples, 
		uint32_t usage, 
		uint32_t flags,
		uint32_t aspectFlags);
protected:
private:
};
