#include "VulkanRenderPass.h"
#include "VulkanEngine.h"

void VulkanRenderPass::Create()
{
	assert(!IsValid());

	std::vector<VkAttachmentDescription> attachmentDescriptions
	{
		VkAttachmentDescription
		{
			0U,
			VulkanEngine::Instance().GetSwapchain().GetColorFormat(),
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		},

		VkAttachmentDescription
		{
			0U,
			VulkanEngine::Instance().GetSwapchain().GetDepthStencilFormat(),
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		}
	};

	VkAttachmentReference attachmentRefColor
	{
		0U,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference attachmentRefDepth
	{
		1U,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDescription
	{
		0U,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0U,
		nullptr,
		1U,
		&attachmentRefColor,
		nullptr,
		&attachmentRefDepth,
		0U,
		nullptr
	};

	std::vector<VkSubpassDependency> subpassDependencies =
	{
		VkSubpassDependency
		{
			VK_SUBPASS_EXTERNAL,
			0U,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		},

		VkSubpassDependency
		{
			0U,
			VK_SUBPASS_EXTERNAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		},
	};

	VkRenderPassCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0U,
		(uint32_t)attachmentDescriptions.size(),
		attachmentDescriptions.data(),
		1U,
		&subpassDescription,
		(uint32_t)subpassDependencies.size(),
		subpassDependencies.data()
	};

	Check(vkCreateRenderPass(VulkanEngine::Instance().GetDevice().Get(), &createInfo, nullptr, &m_Handle));
}

void VulkanRenderPass::Destory()
{
	assert(IsValid());

	vkDestroyRenderPass(VulkanEngine::Instance().GetDevice().Get(), m_Handle, nullptr);

	Reset();
}