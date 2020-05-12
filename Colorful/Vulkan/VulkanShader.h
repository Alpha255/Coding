#pragma once

#include "VulkanLoader.h"

class VulkanShader : public VulkanDeviceObject<VkShaderModule>, public GfxShader
{
public:
	VulkanShader(VkDevice device, eRShaderUsage usage, const std::string& shaderName);

	inline const VkPipelineVertexInputStateCreateInfo& inputLayout() const
	{
		assert(m_InputLayout.InputState.sType != 0);
		return m_InputLayout.InputState;
	}

	void setInputLayout(const std::vector<GfxVertexAttributes>& vertexAttributes, size_t align) override final;

	void destroy(VkDevice device) override final
	{
		/// A shader module can be destroyed while pipelines created using its shaders are still in use.
		if (isValid())
		{
			vkDestroyShaderModule(device, Handle, vkMemoryAllocator);
			Handle = VK_NULL_HANDLE;
		}
	}
protected:
	struct VulkanInputLayout
	{
		VkPipelineVertexInputStateCreateInfo InputState{};
		std::vector<VkVertexInputBindingDescription> InputBindings;
		std::vector<VkVertexInputAttributeDescription> InputAttributes;
	};
private:
	VulkanInputLayout m_InputLayout;
};