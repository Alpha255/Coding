#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(Gfx)

class VkEnumTranslator
{
public:
	static VkPolygonMode polygonMode(EPolygonMode mode);
	static VkCullModeFlags cullMode(ECullMode mode);
	static VkFrontFace frontFace(EFrontFace frontFace);
	static VkLogicOp logicOp(ELogicOp op);
	static VkBlendOp blendOp(EBlendOp op);
	static VkBlendFactor blendFactor(EBlendFactor factor);
	static VkColorComponentFlags colorComponentFlags(uint32_t colorMask);
	static VkCompareOp compareFunc(ECompareFunc func);
	static VkStencilOp stencilOp(EStencilOp op);
	static VkFormat format(EFormat format);
	static VkShaderStageFlagBits shaderStage(EShaderStage stage);
	static VkPrimitiveTopology primitiveTopology(EPrimitiveTopology primitiveTopology);
	static VkDescriptorType descriptorType(EDescriptorType type);
	static VkFilter textureFilter(ETextureFilter filter);
	static VkSamplerAddressMode samplerAddressMode(ESamplerAddressMode addressMode);
	static VkBorderColor borderColor(EBorderColor borderColor);
	static VkImageType imageType(ETextureType type);
	static VkImageViewType imageViewType(ETextureType type);
	static VkDebugReportObjectTypeEXT debugReportObjectType(EDescriptorType type);
	static VkAttachmentLoadOp attachmentLoadOp(RenderPassDesc::EAttachmentLoadOp op);
	static VkAttachmentStoreOp attachmentStoreOp(RenderPassDesc::EAttachmentStoreOp op);
	static VkImageLayout imageLayout(Texture::EImageLayout layout);
	static VkPipelineStageFlags pipelineStageFlags(uint64_t flags);
	static VkAccessFlags accessFlags(uint64_t flags);
};

NAMESPACE_END(Gfx)
