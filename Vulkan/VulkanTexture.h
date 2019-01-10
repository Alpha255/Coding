#pragma once

#include "VulkanView.h"

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
		const void *pSysMem = nullptr,
		uint32_t sysMemPitch = 0U,
		uint32_t sysMemSlicePitch = 0U);
protected:
private:
};
