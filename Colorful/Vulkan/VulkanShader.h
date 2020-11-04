#pragma once

#include "Colorful/Vulkan/VulkanMap.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(VulkanShader)
class VulkanShader final : public VkObject<VkShaderModule_T>, public VkDeviceResource, public Shader
{
public:
	VulkanShader(VkDevice device, EShaderStage stage, const std::vector<uint32_t>& spirv)
		: Shader(stage)
	{
		assert(device && stage < EShaderStage::ShaderStageCount && spirv.size());

		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0u,
			spirv.size() * sizeof(uint32_t),
			spirv.data()
		};

		VERIFY_VK(vkCreateShaderModule(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
	}

	void destroy(VkDevice device) override final
	{
		assert(device);
		vkDestroyShaderModule(device, get(), VK_MEMORY_ALLOCATOR);
	}
};

DECLARE_SHARED_PTR(VulkanInputLayout)
class VulkanInputLayout final : public InputLayout
{
public:
	VulkanInputLayout(const std::vector<VertexInputDesc>& descs)
	{
		for (uint32_t i = 0u; i < descs.size(); ++i)
		{
			VkVertexInputBindingDescription binding
			{
				descs[i].Slot,
				0u,
				descs[i].InputRate == VertexInputDesc::EVertexInputRate::Instance ?
					VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX
			};

			uint32_t stride = 0u;
			for (auto& layout : descs[i].Layouts)
			{
				VkVertexInputAttributeDescription attr
				{
					layout.Location,
					i,
					VulkanMap::format(layout.Format),
					stride
				};
				m_Attrs.emplace_back(attr);

				stride += layout.Stride;
			}

			binding.stride = stride;
			m_Bindings.emplace_back(binding);
		}
	}

	VkPipelineVertexInputStateCreateInfo getInputState() const
	{
		assert(m_Bindings.size() && m_Attrs.size());

		VkPipelineVertexInputStateCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0u,
			static_cast<uint32_t>(m_Bindings.size()),
			m_Bindings.data(),
			static_cast<uint32_t>(m_Attrs.size()),
			m_Attrs.data()
		};

		return createInfo;
	}
protected:
private:
	std::vector<VkVertexInputBindingDescription> m_Bindings{};
	std::vector<VkVertexInputAttributeDescription> m_Attrs{};
};

NAMESPACE_END(Gfx)