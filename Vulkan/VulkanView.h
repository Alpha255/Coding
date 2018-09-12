#pragma once

#include "VulkanObject.h"

class VulkanViewport : public VkViewport
{
public:
	VulkanViewport(
		float w,
		float h,
		float depthMin = 0.0f,
		float depthMax = 1.0f,
		float left = 0.0f,
		float top = 0.0f)
	{
		width = w;
		height = h;
		x = left;
		y = top;
		minDepth = depthMin;
		maxDepth = depthMax;
	}
protected:
private:
};
