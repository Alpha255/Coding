#pragma once

#include "VulkanObject.h"

///  Subpasses are subsequent rendering operations that depend on the contents of framebuffers in previous
/// passes, for example a sequence of post - processing effects that are applied one after another

class VulkanRenderPass : public VulkanObject<VkRenderPass>
{
public:
	void Create();
	void Destory() override;
protected:
private:
};
