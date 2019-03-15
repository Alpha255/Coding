#pragma once

#include "VulkanView.h"

struct VulkanSubResourceData
{
	const void *Memory = nullptr;
	uint32_t MemPitch = 0U;
	uint32_t MemSlicePitch = 0U;
};

class VulkanTexture2D : public VulkanImage
{
public:
	void Create(
		uint32_t fmt,
		uint32_t width,
		uint32_t height,
		uint32_t bindFlags,
		uint32_t mipLevels = 0U,
		uint32_t arraySize = 1U,
		uint32_t cpuFlags = 0U,
		uint32_t miscFlags = 0U,
		uint32_t usage = 0U,
		const VulkanSubResourceData *pSubResourceData = nullptr);
protected:
private:
};
