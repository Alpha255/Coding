#include "VulkanRenderPass.h"
#include "VulkanEngine.h"

void VulkanRenderPass::Create(bool depth, uint32_t colorFormat, uint32_t depthFormat, bool clear, uint32_t imageLayout)
{
	assert(!IsValid());

	VkAttachmentDescription layoutColor
	{
		0U,
		(VkFormat)colorFormat,
		VK_SAMPLE_COUNT_1_BIT,
		clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		(VkImageLayout)imageLayout,
		(VkImageLayout)imageLayout
	};
	VkAttachmentReference refColor
	{
		0U,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference refDepth
	{
		1U,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentDescription layoutDepth
	{
		0U,
		(VkFormat)depthFormat,
		VK_SAMPLE_COUNT_1_BIT,
		clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDes
	{
		0U,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0U,
		nullptr,
		1U,
		&refColor,
		nullptr,
		depth ? &refDepth : nullptr,
		0U,
		nullptr
	};

	VkAttachmentDescription attachmentDesc[] = { layoutColor, layoutDepth };
	VkRenderPassCreateInfo renderPassCreateInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0U,
		depth ? 2U : 1U,
		attachmentDesc,
		1U,
		&subpassDes,
		0U,
		nullptr
	};

	VKCheck(vkCreateRenderPass(VulkanEngine::Instance().GetDevice(), &renderPassCreateInfo, nullptr, &m_Handle));
}

void VulkanRenderPass::Destory()
{
	assert(IsValid());

	vkDestroyRenderPass(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}