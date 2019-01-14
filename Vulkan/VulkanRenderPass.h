#pragma once

#include "VulkanView.h"
#include "VulkanBuffer.h"

class VulkanRenderPass : public VulkanObject<VkRenderPass>
{
public:
	inline VulkanRenderPass() = default;
	inline ~VulkanRenderPass()
	{
		Destory();
	}

	void Create();
protected:
	void Destory();
private:
	std::vector<VulkanImageView> m_Views;
	VulkanFrameBuffer m_FrameBuffer;
};
