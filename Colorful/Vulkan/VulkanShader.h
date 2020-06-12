#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

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

	void allocResources() override final
	{
		uint32_t maxBinding[VK_DESCRIPTOR_TYPE_RANGE_SIZE]{};
		for each (auto &reflection in m_Reflections)
		{
			++maxBinding[reflection.Type];
		}

		for (uint32_t i = VK_DESCRIPTOR_TYPE_SAMPLER; i < VK_DESCRIPTOR_TYPE_RANGE_SIZE; ++i)
		{
			if (maxBinding[i] == 0)
			{
				continue;
			}
			switch (i)
			{
			case VK_DESCRIPTOR_TYPE_SAMPLER:
				m_Samplers.resize(maxBinding[i]);
				break;
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				m_CombinedTextureSamplers.resize(maxBinding[i]);
				break;
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				m_Textures.resize(maxBinding[i]);
				break;
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				break;
			case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
				break;
			case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
				break;
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				m_UniformBuffers.resize(maxBinding[i]);
				break;
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
				break;
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
				break;
			case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
				break;
			}
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