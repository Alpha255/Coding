#include "VulkanState.h"
#include "VulkanEngine.h"

void VulkanSamplerState::Create(uint32_t filter, uint32_t addressMode, float LODBias, uint32_t compFunc, const float *pBorderClr, float minLOD, float maxLOD)
{
	VkSamplerCreateInfo createInfo = 
	{ 
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0U,
		(VkFilter)filter,
		(VkFilter)filter,
		(VkSamplerMipmapMode)filter,
		(VkSamplerAddressMode)addressMode,
		(VkSamplerAddressMode)addressMode,
		(VkSamplerAddressMode)addressMode,
		LODBias,
		false,
		1.0f,
		true,
		(VkCompareOp)compFunc,
		minLOD,
		maxLOD,
		VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		false
	};

	VKCheck(vkCreateSampler(VulkanEngine::Instance().GetDevice().Get(), &createInfo, 0, &m_Handle));
}