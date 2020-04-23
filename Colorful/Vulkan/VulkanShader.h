#pragma once

#include "Colorful/Public/RAsset.h"
#include "VulkanLoader.h"

class vkShader : public vkDeviceObject<VkShaderModule>, public rShader
{
public:
	vkShader(const class vkDevice &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary);
	void destroy(const class vkDevice &device) override final;

	inline const VkPipelineVertexInputStateCreateInfo &getInputLayout() const
	{
		assert(m_InputLayout.InputState.sType != 0);
		return m_InputLayout.InputState;
	}

	void setInputLayout(const std::vector<rVertexAttributes>& vertexAttributes) override final;
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