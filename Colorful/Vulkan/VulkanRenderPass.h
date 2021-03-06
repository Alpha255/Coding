#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"

/*******VkPipelineStageFlagBits*******
	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT specifies the stage of the pipeline where any commands are initially received by the queue.
	VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT specifies the stage of the pipeline where Draw / DispatchIndirect data structures are consumed.This stage also includes reading commands written by vkCmdProcessCommandsNVX.
	VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV specifies the task shader stage.
	VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV specifies the mesh shader stage.
	VK_PIPELINE_STAGE_VERTEX_INPUT_BIT specifies the stage of the pipeline where vertex and index buffers are consumed.
	VK_PIPELINE_STAGE_VERTEX_SHADER_BIT specifies the vertex shader stage.
	VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT specifies the tessellation control shader stage.
	VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT specifies the tessellation evaluation shader stage.
	VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT specifies the geometry shader stage.
	VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT specifies the fragment shader stage.

	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT specifies the stage of the pipeline where early fragment tests(depth and stencil tests before fragment shading) are performed.
		This stage also includes subpass load operations for framebuffer attachments with a depth / stencil format.

	VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT specifies the stage of the pipeline where late fragment tests(depth and stencil tests after fragment shading) are performed.
		This stage also includes subpass store operations for framebuffer attachments with a depth / stencil format.

	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT specifies the stage of the pipeline after blending where the final color values are output from the pipeline.
		This stage also includes subpass load and store operations and multisample resolve operations for framebuffer attachments with a color or depth / stencil format.

	VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT specifies the execution of a compute shader.
	VK_PIPELINE_STAGE_TRANSFER_BIT specifies the execution of copy commands.This includes the operations resulting from all copy commands, clear commands(with the exception of vkCmdClearAttachments), and vkCmdCopyQueryPoolResults.
	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT specifies the final stage in the pipeline where operations generated by all commands complete execution.
	VK_PIPELINE_STAGE_HOST_BIT specifies a pseudo - stage indicating execution on the host of reads / writes of device memory.This stage is not invoked by any commands recorded in a command buffer.
	VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV specifies the execution of the ray tracing shader stages.
	VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV specifies the execution of vkCmdBuildAccelerationStructureNV, vkCmdCopyAccelerationStructureNV, and vkCmdWriteAccelerationStructuresPropertiesNV.
	VK_PIPELINE_STAGE_ALL_COMMANDS_BIT is equivalent to the logical OR of every other pipeline stage flag that is supported on the queue it is used with.
	VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT specifies the stage of the pipeline where the predicate of conditional rendering is consumed.
	VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT specifies the stage of the pipeline where vertex attribute output values are written to the transform feedback buffers.
	VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX specifies the stage of the pipeline where device - side generation of commands via vkCmdProcessCommandsNVX is handled.
	VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV specifies the stage of the pipeline where the shading rate image is read to determine the shading rate for portions of a rasterized primitive.
	VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT specifies the stage of the pipeline where the fragment density map is read to generate the fragment areas.

	VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT specifies the execution of all graphics pipeline stages, and is equivalent to the logical OR of :
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
		VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
		VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV
		VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
		VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
		VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
		VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
		VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
		VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT
		VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV
		VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT
**************************************/

/********VkSubpassDependency*******
	If srcSubpass is equal to dstSubpass then the VkSubpassDependency describes a subpass self-dependency, and only constrains the pipeline barriers allowed within a subpass instance. 
	Otherwise, when a render pass instance which includes a subpass dependency is submitted to a queue, it defines a memory dependency between the subpasses identified by srcSubpass and dstSubpass.
	
	If srcSubpass is equal to VK_SUBPASS_EXTERNAL, the first synchronization scope includes commands that occur earlier in submission order than the vkCmdBeginRenderPass used to begin the render pass instance. Otherwise, 
	the first set of commands includes all commands submitted as part of the subpass instance identified by srcSubpass and any load, store or multisample resolve operations on attachments used in srcSubpass. 
	In either case, the first synchronization scope is limited to operations on the pipeline stages determined by the source stage mask specified by srcStageMask.

	If dstSubpass is equal to VK_SUBPASS_EXTERNAL, the second synchronization scope includes commands that occur later in submission order than the vkCmdEndRenderPass used to end the render pass instance. 
	Otherwise, the second set of commands includes all commands submitted as part of the subpass instance identified by dstSubpass and any load, store or multisample resolve operations on attachments used in dstSubpass. 
	In either case, the second synchronization scope is limited to operations on the pipeline stages determined by the destination stage mask specified by dstStageMask.

	The first access scope is limited to access in the pipeline stages determined by the source stage mask specified by srcStageMask. It is also limited to access types in the source access mask specified by srcAccessMask.

	The second access scope is limited to access in the pipeline stages determined by the destination stage mask specified by dstStageMask. It is also limited to access types in the destination access mask specified by dstAccessMask.

	The availability and visibility operations defined by a subpass dependency affect the execution of image layout transitions within the render pass.

	srcSubpass must be less than or equal to dstSubpass, unless one of them is VK_SUBPASS_EXTERNAL, to avoid cyclic dependencies and ensure a valid execution order

	srcSubpass and dstSubpass must not both be equal to VK_SUBPASS_EXTERNAL

	If srcSubpass is equal to dstSubpass and not all of the stages in srcStageMask and dstStageMask are framebuffer-space stages, 
	the logically latest pipeline stage in srcStageMask must be logically earlier than or equal to the logically earliest pipeline stage in dstStageMask

	If srcSubpass equals dstSubpass, and srcStageMask and dstStageMask both include a framebuffer-space stage, then dependencyFlags must include VK_DEPENDENCY_BY_REGION_BIT

	If dependencyFlags includes VK_DEPENDENCY_VIEW_LOCAL_BIT, srcSubpass must not be equal to VK_SUBPASS_EXTERNAL

	If dependencyFlags includes VK_DEPENDENCY_VIEW_LOCAL_BIT, dstSubpass must not be equal to VK_SUBPASS_EXTERNAL

	If srcSubpass equals dstSubpass and that subpass has more than one bit set in the view mask, then dependencyFlags must include VK_DEPENDENCY_VIEW_LOCAL_BIT
***********************************/

/// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#memory-model

NAMESPACE_START(Gfx)

class VulkanRenderPass final : public VkDeviceObject<VkRenderPass_T>
{
public:
	VulkanRenderPass(const RenderPassDesc& desc);
	~VulkanRenderPass();
protected:
private:
};

class VulkanFramebuffer final : public VkDeviceObject<VkFramebuffer_T>
{
public:
	VulkanFramebuffer(VkRenderPass renderPass, const FrameBufferDesc& desc);
	~VulkanFramebuffer();
protected:
private:
};

NAMESPACE_END(Gfx)