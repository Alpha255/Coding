#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

class VulkanEnum
{
public:
	static VkPolygonMode toPolygonMode(eRPolygonMode mode);
	static VkCullModeFlags toCullMode(eRCullMode mode);
	static VkFrontFace toFrontFace(eRFrontFace frontFace);
	static VkLogicOp toLogicOp(eRLogicOp op);
	static VkBlendOp toBlendOp(eRBlendOp op);
	static VkBlendFactor toBlendFactor(eRBlendFactor factor);
	static VkColorComponentFlags toColorComponentFlags(uint32_t colorMask);
	static VkCompareOp toCompareOp(eRCompareOp op);
	static VkStencilOp toStencilOp(eRStencilOp op);
	static VkFormat toFormat(eRFormat format);
	static VkShaderStageFlagBits toShaderStage(eRShaderUsage usage);
	static VkPrimitiveTopology toPrimitiveTopology(eRPrimitiveTopology primitiveTopology);
	static VkDescriptorType toDescriptorType(eRDescriptorType type);
	static VkFilter toFilter(eRFilter filter);
	static VkSamplerAddressMode toAddressMode(eRSamplerAddressMode addressMode);
	static VkBorderColor toBorderColor(eRBorderColor borderColor);
};
