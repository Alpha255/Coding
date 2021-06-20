#pragma once

#include "Colorful/Vulkan/VulkanEnumTranslator.h"

NAMESPACE_START(Gfx)

class VulkanShader final : public VkDeviceObject<VkShaderModule_T>, public Shader
{
public:
	VulkanShader(ShaderDesc& desc);
	~VulkanShader();
};

class VulkanInputLayout final : public InputLayout
{
public:
	VulkanInputLayout(const VertexInputLayoutDesc& desc)
		: InputLayout(desc.PrimitiveTopology)
	{
		for (uint32_t i = 0u; i < desc.VertexInputs.size(); ++i)
		{
			VkVertexInputBindingDescription binding
			{
				desc.VertexInputs[i].Binding,
				0u,
				desc.VertexInputs[i].InputRate == VertexInputLayoutDesc::EVertexInputRate::Instance ?
					VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX
			};

			uint32_t stride = 0u;
			for (auto& layout : desc.VertexInputs[i].Layouts)
			{
				VkVertexInputAttributeDescription attr
				{
					layout.Location,
					i,
					VkEnumTranslator::format(layout.Format),
					stride
				};
				m_Attrs.emplace_back(attr);

				stride += layout.Stride;
			}

			binding.stride = stride;
			m_Bindings.emplace_back(binding);
		}
	}

	VkPipelineVertexInputStateCreateInfo inputStateCreateInfo() const
	{
		assert(m_Bindings.size() && m_Attrs.size());

		return VkPipelineVertexInputStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0u,
			static_cast<uint32_t>(m_Bindings.size()),
			m_Bindings.data(),
			static_cast<uint32_t>(m_Attrs.size()),
			m_Attrs.data()
		};
	}

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo() const
	{
		assert(m_PrimitiveTopology != EPrimitiveTopology::Unknown);

		return VkPipelineInputAssemblyStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0u,
			VkEnumTranslator::primitiveTopology(m_PrimitiveTopology),
			false
		};
	}
protected:
private:
	std::vector<VkVertexInputBindingDescription> m_Bindings;
	std::vector<VkVertexInputAttributeDescription> m_Attrs;
};

NAMESPACE_END(Gfx)