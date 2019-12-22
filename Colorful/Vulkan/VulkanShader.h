#pragma once

#include "Colorful/Public/RAsset.h"
#include "VulkanLoader.h"

class vkShader : public vkDeviceObject<VkShaderModule>, public rShader
{
public:
	vkShader(const class vkDevice &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary);
	void destroy(const class vkDevice &device) override final;
protected:
private:
};