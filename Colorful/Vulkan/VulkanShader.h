#pragma once

#include "Colorful/Public/RAsset.h"
#include "VulkanLoader.h"

class vkShader : public VulkanDeviceObject<VkShaderModule>, public GfxShader
{
public:
	vkShader(VkDevice device, eRShaderUsage usage, const rAsset::rShaderBinary &binary);
	void destroy(VkDevice device) override final;

	inline const VkPipelineVertexInputStateCreateInfo &getInputLayout() const
	{
		assert(m_InputLayout.InputState.sType != 0);
		return m_InputLayout.InputState;
	}

	void setInputLayout(const std::vector<GfxVertexAttributes>& vertexAttributes, size_t align) override final;
protected:
	struct vkInputLayout
	{
		VkPipelineVertexInputStateCreateInfo InputState{};
		std::vector<VkVertexInputBindingDescription> InputBindings;
		std::vector<VkVertexInputAttributeDescription> InputAttributes;
	};
private:
	vkInputLayout m_InputLayout;
};