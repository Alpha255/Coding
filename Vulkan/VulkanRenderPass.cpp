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
		clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	VkAttachmentReference refColor
	{
		0U,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	//VkAttachmentReference refDepth
	//{
	//	1U,
	//	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	//};

	//VkAttachmentDescription layoutDepth
	//{
	//	0U,
	//	(VkFormat)depthFormat,
	//	VK_SAMPLE_COUNT_1_BIT,
	//	clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
	//	VK_ATTACHMENT_STORE_OP_STORE,
	//	VK_ATTACHMENT_LOAD_OP_CLEAR,
	//	VK_ATTACHMENT_STORE_OP_STORE,
	//	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	//	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	//};

	VkSubpassDescription subpassDes
	{
		0U,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0U,
		nullptr,
		1U,
		&refColor,
		nullptr,
		nullptr,
		0U,
		nullptr
	};

	VkSubpassDependency subpass
	{
		VK_SUBPASS_EXTERNAL,
		0U,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0U,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		0U
	};

	//VkAttachmentDescription attachmentDesc[] = { layoutColor, layoutDepth };
	VkRenderPassCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0U,
		1U,
		&layoutColor,
		1U,
		&subpassDes,
		1U,
		&subpass
	};

	VKCheck(vkCreateRenderPass(VulkanEngine::Instance().GetDevice(), &createInfo, nullptr, &m_Handle));
}

void VulkanRenderPass::Destory()
{
	assert(IsValid());

	vkDestroyRenderPass(VulkanEngine::Instance().GetDevice(), m_Handle, nullptr);

	Reset();
}