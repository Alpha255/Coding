#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanMap.h"

NAMESPACE_START(Gfx)

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, const PipelineResourceLayoutDesc& desc)
{
	assert(device);

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (uint32_t i = 0u; i < EShaderStage::ShaderStageCount; ++i)
	{
		for (uint32_t j = 0u; j < desc.Resources[i].size(); ++j)
		{
			auto& shaderResource = desc.Resources[i][j];
			VkDescriptorSetLayoutBinding binding
			{
				shaderResource.Type == ShaderResource::EResourceType::UniformBuffer ? 0u : shaderResource.Binding,
				VulkanMap::descriptorType(shaderResource.Type),
				1u,
				VulkanMap::shaderStage(static_cast<EShaderStage>(i)),
				nullptr
			};
			bindings.emplace_back(std::move(binding));
		}
	}

	VkDescriptorSetLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(bindings.size()),
		bindings.data()
	};
	VERIFY_VK(vkCreateDescriptorSetLayout(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device, VkPhysicalDevice physicalDevice)
	: m_Device(device)
{
	///VkPhysicalDeviceProperties deviceProperties{};
	///vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	///auto& deviceLimits = deviceProperties.limits;

	enum EDescriptorPoolLimits
	{
		Max_DesctriptorSet = 2048,
		Max_Sampler = 256,
		Max_CombinesImageSampler = 2048,
		Max_SampledImage = 2048,
		Max_StorageImage = 256,
		Max_UniformTexelBuffer = 256,
		Max_UniformBuffer = 1024,
		Max_StorageTexelBuffer = 256,
		Max_StorageBuffer = 1024,
		Max_InputAttachment = 64,

	};

	std::vector<VkDescriptorPoolSize> poolSizes
	{
		{
			VK_DESCRIPTOR_TYPE_SAMPLER,
			EDescriptorPoolLimits::Max_Sampler
		},
		{
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			EDescriptorPoolLimits::Max_CombinesImageSampler
		},
		{
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			EDescriptorPoolLimits::Max_SampledImage
		},
		{
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			EDescriptorPoolLimits::Max_StorageImage
		},
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
			EDescriptorPoolLimits::Max_UniformTexelBuffer
		},
		{
			VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
			EDescriptorPoolLimits::Max_StorageTexelBuffer
		},
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			EDescriptorPoolLimits::Max_UniformBuffer
		},
		{
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			EDescriptorPoolLimits::Max_StorageBuffer
		},
		{
			VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
			EDescriptorPoolLimits::Max_InputAttachment
		}
	};

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0u,
		Max_DesctriptorSet,
		static_cast<uint32_t>(poolSizes.size()),
		poolSizes.data()
	};
	VERIFY_VK(vkCreateDescriptorPool(device, &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanDescriptorSetPtr VulkanDescriptorPool::alloc(VkDescriptorSetLayout layout)
{
	assert(isValid());

	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		get(),
		1u,
		&layout
	};

	VulkanDescriptorSetPtr descriptorSet;
	VERIFY_VK(vkAllocateDescriptorSets(m_Device, &allocInfo, descriptorSet->reference()));
	return descriptorSet;
}

NAMESPACE_END(Gfx)

#if 0
#include "Colorful/Vulkan/VulkanEngine.h"

void VulkanDescriptorSet::update(VkDevice device, const VulkanResourceMap& resourceMap)
{
	assert(isValid());

	std::vector<VkWriteDescriptorSet> writeActions;
	std::vector<VkDescriptorBufferInfo> bufferInfos;
	std::vector<VkDescriptorImageInfo> imageInfos;

#if defined(UsingUnorderedMap)
	for (auto &resourceBinding : resourceMap)
	{
		VkWriteDescriptorSet writeAction
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			Handle,
			resourceBinding.first,
			0u,
			1u,
			resourceBinding.second.Type,
			nullptr,
			nullptr,
			nullptr
		};

		switch (resourceBinding.second.Type)
		{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
		{
			assert(resourceBinding.second.Sampler);
			VkDescriptorImageInfo imageInfo
			{
				resourceBinding.second.Sampler,
				VK_NULL_HANDLE,
				VK_IMAGE_LAYOUT_UNDEFINED
			};
			imageInfos.emplace_back(std::move(imageInfo));
			writeAction.pImageInfo = &imageInfos[imageInfos.size() - 1u];
		}
		break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		{
			assert(resourceBinding.second.CombinedImageSampler.ImageView);
			assert(resourceBinding.second.CombinedImageSampler.Sampler);

			VkDescriptorImageInfo imageInfo
			{
				resourceBinding.second.CombinedImageSampler.Sampler,
				resourceBinding.second.CombinedImageSampler.ImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			imageInfos.emplace_back(std::move(imageInfo));
			writeAction.pImageInfo = &imageInfos[imageInfos.size() - 1u];
		}
		break;
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		{
			assert(resourceBinding.second.ImageView);

			VkDescriptorImageInfo imageInfo
			{
				VK_NULL_HANDLE,
				resourceBinding.second.ImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			imageInfos.emplace_back(std::move(imageInfo));
			writeAction.pImageInfo = &imageInfos[imageInfos.size() - 1u];
		}
		break;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		{
			assert(resourceBinding.second.Buffer);

			VkDescriptorBufferInfo bufferInfo
			{
				resourceBinding.second.Buffer,
				0u,
				VK_WHOLE_SIZE
			};
			bufferInfos.emplace_back(std::move(bufferInfo));
			writeAction.pBufferInfo = &bufferInfos[bufferInfos.size() - 1u];
		}
		break;
		default:
			assert(0);
			break;
		}
		writeActions.emplace_back(writeAction);
	}
#else
	for (auto &resourceBinding : resourceMap)
	{
		VkWriteDescriptorSet writeAction
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			Handle,
			resourceBinding.Binding,
			0u,
			1u,
			resourceBinding.Type,
			nullptr,
			nullptr,
			nullptr
		};

		switch (resourceBinding.Type)
		{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
		{
			assert(resourceBinding.Sampler);
			VkDescriptorImageInfo imageInfo
			{
				resourceBinding.Sampler,
				VK_NULL_HANDLE,
				VK_IMAGE_LAYOUT_UNDEFINED
			};
			imageInfos.emplace_back(std::move(imageInfo));
			writeAction.pImageInfo = &imageInfos[imageInfos.size() - 1u];
		}
		break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		{
			assert(resourceBinding.CombinedImageSampler.ImageView);
			assert(resourceBinding.CombinedImageSampler.Sampler);

			VkDescriptorImageInfo imageInfo
			{
				resourceBinding.CombinedImageSampler.Sampler,
				resourceBinding.CombinedImageSampler.ImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			imageInfos.emplace_back(std::move(imageInfo));
			writeAction.pImageInfo = &imageInfos[imageInfos.size() - 1u];
		}
		break;
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		{
			assert(resourceBinding.ImageView);

			VkDescriptorImageInfo imageInfo
			{
				VK_NULL_HANDLE,
				resourceBinding.ImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			imageInfos.emplace_back(std::move(imageInfo));
			writeAction.pImageInfo = &imageInfos[imageInfos.size() - 1u];
		}
		break;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		{
			assert(resourceBinding.Buffer);

			VkDescriptorBufferInfo bufferInfo
			{
				resourceBinding.Buffer,
				0u,
				VK_WHOLE_SIZE
			};
			bufferInfos.emplace_back(std::move(bufferInfo));
			writeAction.pBufferInfo = &bufferInfos[bufferInfos.size() - 1u];
		}
		break;
		default:
			assert(0);
			break;
		}
		writeActions.emplace_back(writeAction);
	}
#endif

	assert(!writeActions.empty());
	vkUpdateDescriptorSets(device, (uint32_t)writeActions.size(), writeActions.data(), 0u, nullptr);
}
#endif
