#pragma once

#include "VulkanView.h"
#include "VulkanBuffer.h"

class VulkanRenderPass : public VulkanObject<VkRenderPass>
{
public:
	void Create(bool depth, uint32_t colorFormat, uint32_t depthFormat, bool clear, uint32_t imageLayout);
	void Destory();
protected:
private:
	///std::vector<VulkanImageView> m_Views;
	///VulkanFrameBuffer m_FrameBuffer;
};
