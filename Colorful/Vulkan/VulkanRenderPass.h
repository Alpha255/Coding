#pragma once

#include "VulkanObject.h"

class VulkanRenderPass : public VulkanObject<VkRenderPass>
{
public:
	void Create();
	void Destory() override;
protected:
private:
};
