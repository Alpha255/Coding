#if 0
#include "Colorful/Vulkan/VulkanEngine.h"

VulkanDescriptorPool::VulkanDescriptorPool(const VkDevice device, VkPhysicalDevice physicalDevice)
	: m_Device(device)
{
	assert(!isValid());

	/// ToDo-May need a better way
	VkPhysicalDeviceProperties deviceProperties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	auto deviceLimits = deviceProperties.limits;

	std::array<VkDescriptorPoolSize, VK_DESCRIPTOR_TYPE_RANGE_SIZE> descriptorPoolSizes;
	descriptorPoolSizes[0] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_SAMPLER,
		deviceLimits.maxDescriptorSetSamplers
	};
	descriptorPoolSizes[1] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		deviceLimits.maxDescriptorSetSamplers  /// Not sure
	};
	descriptorPoolSizes[2] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		deviceLimits.maxDescriptorSetSampledImages
	};
	descriptorPoolSizes[3] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		deviceLimits.maxDescriptorSetStorageImages
	};
	descriptorPoolSizes[4] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
		deviceLimits.maxDescriptorSetUniformBuffers  /// Not sure
	};
	descriptorPoolSizes[5] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		deviceLimits.maxDescriptorSetStorageBuffers  /// Not sure
	};
	descriptorPoolSizes[6] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		deviceLimits.maxDescriptorSetUniformBuffers
	};
	descriptorPoolSizes[7] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		deviceLimits.maxDescriptorSetStorageBuffers
	};
	descriptorPoolSizes[8] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		deviceLimits.maxDescriptorSetUniformBuffersDynamic
	};
	descriptorPoolSizes[9] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		deviceLimits.maxDescriptorSetStorageBuffersDynamic
	};
	descriptorPoolSizes[10] = VkDescriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
		deviceLimits.maxDescriptorSetInputAttachments
	};

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0u,
		deviceLimits.maxBoundDescriptorSets,  /// Incorrect, maxBoundDescriptorSets is the maximum number of descriptor sets that can be simultaneously used by a pipeline
		VK_DESCRIPTOR_TYPE_RANGE_SIZE,
		descriptorPoolSizes.data()
	};

	GfxVerifyVk(vkCreateDescriptorPool(device, &createInfo, vkMemoryAllocator, &Handle));
}

void VulkanDescriptorSetLayout::create(VkDevice device, const GfxDescriptorLayoutDesc& desc)
{
	assert(!isValid());

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		for (uint32_t j = 0u; j < desc[i].size(); ++j)
		{
			for (auto &reflection : desc[i])
			{
				assert(reflection.Type != std::numeric_limits<uint32_t>::max());

				auto type = static_cast<VkDescriptorType>(reflection.Type);
				VkDescriptorSetLayoutBinding binding
				{
					reflection.Binding,
					type,
					1u,
					(VkShaderStageFlags)VulkanEnum::toShaderStage(static_cast<eRShaderUsage>(i)),
					nullptr
				};
				bindings.emplace_back(std::move(binding));
			}
		}
	}

	VkDescriptorSetLayoutCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		(uint32_t)bindings.size(),
		bindings.data()
	};

	GfxVerifyVk(vkCreateDescriptorSetLayout(device, &createInfo, vkMemoryAllocator, &Handle));
}

VulkanDescriptorSet VulkanDescriptorPool::alloc(VkDescriptorSetLayout layout, const VulkanDescriptorSet::VulkanResourceMap& resourceMap) const
{
	assert(isValid());

	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		Handle,
		1u,
		&layout
	};

	VulkanDescriptorSet descriptorSet;
	GfxVerifyVk(vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptorSet.Handle));
	descriptorSet.update(m_Device, resourceMap);
	return descriptorSet;
}

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
