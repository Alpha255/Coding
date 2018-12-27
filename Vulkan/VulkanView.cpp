#include "VulkanView.h"
#include "VulkanEngine.h"

void VulkanRenderTargetView::Create()
{
	//VkAttachmentDescription attachments[2] = {};
	//attachments[0].format = (VkFormat)VulkanEngine::Instance().GetSwapchain().GetFormat();
	//attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	//attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//attachments[0].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//attachments[0].flags = 0;

	//attachments[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
	//attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	//attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	//attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//attachments[1].flags = 0;

	//VkSubpassDescription subPass;
	//subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//subPass.inputAttachmentCount = 0;
	//subPass.pInputAttachments = NULL;

	//VkAttachmentReference colorRefs[1] = { { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
	//subPass.colorAttachmentCount = _countof(colorRefs);
	//subPass.pColorAttachments = colorRefs;
	//subPass.flags = 0;

	//VkAttachmentReference depthRefs[1] = { { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };

	//subPass.pDepthStencilAttachment = depthRefs;

	//subPass.preserveAttachmentCount = 0;
	//subPass.pPreserveAttachments = NULL;
	//subPass.pResolveAttachments = NULL;

	//VkRenderPassCreateInfo rpInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	//rpInfo.pAttachments = attachments;
	//rpInfo.attachmentCount = _countof(attachments);
	//rpInfo.pSubpasses = &subPass;
	//rpInfo.subpassCount = 1;
	//rpInfo.pDependencies = NULL;
	//rpInfo.dependencyCount = 0;

	//VKCheck(vkCreateRenderPass(VulkanEngine::Instance().GetDevice(), &rpInfo, NULL, &m_Handle));
}