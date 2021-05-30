#include "Colorful/Vulkan/VulkanRenderPass.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanEnumTranslator.h"

/*************************************************
	When defining a memory dependency, using only VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT would never
	make any accesses available and/or visible because these stages do not access memory.

	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful for accomplishing layout transitions and queue
	ownership operations when the required execution dependency is satisfied by other means - for example, semaphore operations between queues.


	A render pass represents a collection of attachments, subpasses, and dependencies between the subpasses,
	and describes how the attachments are used over the course of the subpasses.
	The use of a render pass in a command buffer is a render pass instance.

	A subpass represents a phase of rendering that reads and writes a subset of the attachments in a render pass.
	Rendering commands are recorded into a particular subpass of a render pass instance.

	A subpass description describes the subset of attachments that is involved in the execution of a subpass.
	Each subpass can read from some attachments as input attachments, write to some as color attachments or depth/stencil attachments,
	and perform multisample resolve operations to resolve attachments. A subpass description can also include a set of preserve attachments,
	which are attachments that are not read or written by the subpass but whose contents must be preserved throughout the subpass.

	The subpasses in a render pass all render to the same dimensions,
	and fragments for pixel (x,y,layer) in one subpass can only read attachment contents written by previous subpasses at that same (x,y,layer) location.

	Subpass dependencies describe execution and memory dependencies between subpasses.

	A render pass describes the structure of subpasses and attachments independent of any specific image views for the attachments.
	A render pass and a framebuffer define the complete render target state for one or more subpasses as well as the algorithmic dependencies between the subpasses.
	A render pass must have at least one subpass.
**************************************************/

NAMESPACE_START(Gfx)

VulkanRenderPass::VulkanRenderPass(const RenderPassDesc& desc)
{
	/// pPreserveAttachments is a pointer to an array of preserveAttachmentCount render pass attachment indices identifying attachments that are not used by this subpass, 
	/// but whose contents must be preserved throughout the subpass.

	/*********************************************************************************************************************
	VK_ATTACHMENT_LOAD_OP_LOAD specifies that the previous contents of the image within the render area will be preserved. 
	For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT. 
	For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_READ_BIT.

	VK_ATTACHMENT_LOAD_OP_CLEAR specifies that the contents within the render area will be cleared to a uniform value, 
	which is specified when a render pass instance is begun. 
	For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT. 
	For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.

	VK_ATTACHMENT_LOAD_OP_DONT_CARE specifies that the previous contents within the area need not be preserved; 
	the contents of the attachment will be undefined inside the render area. 
	For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT. 
	For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
	**********************************************************************************************************************/

	/*********************************************************************************************************************
	VK_ATTACHMENT_STORE_OP_STORE specifies the contents generated during the render pass and within the render area are written to memory. 
	For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT. 
	For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.

	VK_ATTACHMENT_STORE_OP_DONT_CARE specifies the contents within the render area are not needed after rendering, and may be discarded; 
	the contents of the attachment will be undefined inside the render area. 
	For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT. 
	For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.

	VK_ATTACHMENT_STORE_OP_NONE_QCOM specifies that the contents within the render area were not written during rendering, 
	and may not be written to memory. If the attachment was written to during the renderpass, 
	the contents of the attachment will be undefined inside the render area.
	*********************************************************************************************************************/

	assert(desc.AttachmentDescs.size() > 0u);

	std::vector<VkAttachmentDescription> attachmentDescs(desc.AttachmentDescs.size());
	for (uint32_t i = 0u; i < desc.AttachmentDescs.size(); ++i)
	{
		auto const& attachmentDesc = desc.AttachmentDescs[i];
		attachmentDescs[i] =
		{
			0u,
			VkEnumTranslator::format(attachmentDesc.Format),
			static_cast<VkSampleCountFlagBits>(attachmentDesc.SampleCount),
			VkEnumTranslator::attachmentLoadOp(attachmentDesc.LoadOp),
			VkEnumTranslator::attachmentStoreOp(attachmentDesc.StoreOp),
			VkEnumTranslator::attachmentLoadOp(attachmentDesc.StencilLoadOp),
			VkEnumTranslator::attachmentStoreOp(attachmentDesc.StencilStoreOp),
			VkEnumTranslator::imageLayout(attachmentDesc.InitialLayout),
			VkEnumTranslator::imageLayout(attachmentDesc.FinalLayout)
		};
	}

	uint32_t curAttachmentRef = 0u;
	std::vector<VkAttachmentReference> attachmentRefs;
	auto getAttachmentRefs = [&](size_t count, const RenderPassDesc::AttachmentReference* refs) -> const VkAttachmentReference* {
		if (count > 0u)
		{
			auto const* curAttachmentRefs = &attachmentRefs[curAttachmentRef];
			for (uint32_t i = 0u; i < count; ++i, ++curAttachmentRef)
			{
				auto const& ref = refs[i];
				assert(ref.Index < desc.AttachmentDescs.size());
				attachmentRefs.emplace_back(
					VkAttachmentReference
					{
						ref.Index,
						VkEnumTranslator::imageLayout(ref.Layout)
					});
			}

			return curAttachmentRefs;
		}

		return nullptr;
	};

	/**************************************************************************************************************
	pPreserveAttachments is a pointer to an array of preserveAttachmentCount render pass attachment indices identifying attachments that are not used by this subpass, 
	but whose contents must be preserved throughout the subpass.
	***************************************************************************************************************/

	std::vector<VkSubpassDescription> subpassDescs(desc.SubPasses.size());
	for (uint32_t i = 0u; i < desc.SubPasses.size(); ++i)
	{
		auto const& subpassDesc = desc.SubPasses[i];
		assert(subpassDesc.ResolveAttachments.size() == 0u || subpassDesc.ResolveAttachments.size() == subpassDesc.ColorAttachments.size());

		subpassDescs[i] =
		{
			0u,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			static_cast<uint32_t>(subpassDesc.InputAttachments.size()),
			getAttachmentRefs(subpassDesc.InputAttachments.size(), subpassDesc.InputAttachments.data()),
			static_cast<uint32_t>(subpassDesc.ColorAttachments.size()),
			getAttachmentRefs(subpassDesc.ColorAttachments.size(), subpassDesc.ColorAttachments.data()),
			getAttachmentRefs(subpassDesc.ResolveAttachments.size(), subpassDesc.ResolveAttachments.data()),
			getAttachmentRefs(1u, &subpassDesc.DepthStencilAttachment),
			static_cast<uint32_t>(subpassDesc.PreserveAttachments.size()),
			subpassDesc.PreserveAttachments.data()
		};
	}

	/***********************************************************************************
	VK_DEPENDENCY_BY_REGION_BIT specifies that dependencies will be framebuffer-local.

	VK_DEPENDENCY_VIEW_LOCAL_BIT specifies that a subpass has more than one view.

	VK_DEPENDENCY_DEVICE_GROUP_BIT specifies that dependencies are non-device-local dependency.
	************************************************************************************/

	std::vector<VkSubpassDependency> subpassDependencies(desc.SubPassDependencies.size());
	for (uint32_t i = 0u; i < desc.SubPassDependencies.size(); ++i)
	{
		auto const& dependency = desc.SubPassDependencies[i];
		subpassDependencies[i] =
		{
			dependency.SrcSubpassIndex,
			dependency.DstSubpassIndex,
			VkEnumTranslator::pipelineStageFlags(dependency.SrcStageFlags),
			VkEnumTranslator::pipelineStageFlags(dependency.DstStageFlags),
			VkEnumTranslator::accessFlags(dependency.SrcAccessFlags),
			VkEnumTranslator::accessFlags(dependency.DstAccessFlags),
			VK_DEPENDENCY_BY_REGION_BIT /// ????? 
		};
	}

	VkRenderPassCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(attachmentDescs.size()),
		attachmentDescs.data(),
		static_cast<uint32_t>(subpassDescs.size()),
		subpassDescs.data(),
		static_cast<uint32_t>(subpassDependencies.size()),
		subpassDependencies.data()
	};

	VERIFY_VK(vkCreateRenderPass(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanRenderPass::~VulkanRenderPass()
{
	vkDestroyRenderPass(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

VulkanFramebuffer::VulkanFramebuffer(VkRenderPass renderPass, const FrameBufferDesc& desc)
{
	assert(renderPass);

	std::vector<VkImageView> attachments;
	for (auto& colorAttach : desc.ColorAttachments)
	{
		if (colorAttach)
		{
			auto image = std::static_pointer_cast<VulkanTexture>(colorAttach);
			assert(image);
			assert(image->width() == desc.Width && image->height() == desc.Height && image->layers() == desc.Layers);
			attachments.emplace_back(image->get());
		}
	}
	if (desc.DepthStencilAttachment)
	{
		auto image = std::static_pointer_cast<VulkanTexture>(desc.DepthStencilAttachment);
		assert(image);
		attachments.emplace_back(image->get());
	}

	VkFramebufferCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0u,
		renderPass,
		static_cast<uint32_t>(attachments.size()),
		attachments.data(),
		desc.Width,
		desc.Height,
		desc.Layers
	};

	VERIFY_VK(vkCreateFramebuffer(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	vkDestroyFramebuffer(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

NAMESPACE_END(Gfx)