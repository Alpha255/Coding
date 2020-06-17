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

VulkanDescriptorSet VulkanDescriptorPool::alloc(const GfxDescriptorLayoutDesc& desc) const
{
	assert(isValid());

	VulkanDescriptorSet descriptorSet;
	descriptorSet.createLayout(m_Device, desc);

	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		Handle,
		1u,
		&descriptorSet.m_Layout
	};

	GfxVerifyVk(vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptorSet.Handle));

	return descriptorSet;
}

void VulkanDescriptorSet::setUniformBuffer(const VulkanBufferPtr& buffer, uint32_t binding)
{
	assert(isValid());
	assert(buffer);
	assert(m_ResourceBindingMap[binding].Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	
	if (m_ResourceBindingMap[binding].Buffer != buffer->Handle)
	{
		m_ResourceBindingMap[binding].Buffer = buffer->Handle;
		m_ResourceBindingMap[binding].Dirty = true;
	}
}

void VulkanDescriptorSet::setTexture(const VulkanImageViewPtr& texture, uint32_t binding)
{
	assert(isValid());
	assert(texture && texture->isValid());
	assert(m_ResourceBindingMap[binding].Type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	
	if (m_ResourceBindingMap[binding].ImageView != texture->Handle)
	{
		m_ResourceBindingMap[binding].ImageView = texture->Handle;
		m_ResourceBindingMap[binding].Dirty = true;
	}
}

void VulkanDescriptorSet::setCombinedTextureSampler(const VulkanImageViewPtr& texture, uint32_t binding)
{
	assert(isValid());
	assert(texture && texture->isValid() && texture->sampler());
	assert(m_ResourceBindingMap[binding].Type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	
	auto sampler = std::static_pointer_cast<VulkanSampler>(texture->sampler());
	assert(sampler && sampler->isValid());

	if (m_ResourceBindingMap[binding].CombinedImageSampler.ImageView != texture->Handle || 
		m_ResourceBindingMap[binding].CombinedImageSampler.Sampler != sampler->Handle)
	{
		m_ResourceBindingMap[binding].CombinedImageSampler.ImageView = texture->Handle;
		m_ResourceBindingMap[binding].CombinedImageSampler.Sampler = sampler->Handle;
		m_ResourceBindingMap[binding].Dirty = true;
	}
}

void VulkanDescriptorSet::setSampler(const VulkanSamplerPtr& sampler, uint32_t binding)
{
	assert(isValid());
	assert(sampler && sampler->isValid());
	assert(m_ResourceBindingMap[binding].Type == VK_DESCRIPTOR_TYPE_SAMPLER);
	
	if (m_ResourceBindingMap[binding].Sampler != sampler->Handle)
	{
		m_ResourceBindingMap[binding].Sampler = sampler->Handle;
		m_ResourceBindingMap[binding].Dirty = true;
	}
}

void VulkanDescriptorSet::update(VkDevice device)
{
	assert(isValid());

	std::vector<VkWriteDescriptorSet> writeActions;
	std::vector<VkDescriptorBufferInfo> bufferInfos;
	std::vector<VkDescriptorImageInfo> imageInfos;

	for (auto &resourceBinding : m_ResourceBindingMap)
	{
		if (resourceBinding.second.Dirty)
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
					resourceBinding.second.CombinedImageSampler.ImageView,
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

			resourceBinding.second.Dirty = false;
			writeActions.emplace_back(writeAction);
		}
	}

	if (!writeActions.empty())
	{
		vkUpdateDescriptorSets(device, (uint32_t)writeActions.size(), writeActions.data(), 0u, nullptr);
	}
}

void VulkanDescriptorSet::createLayout(VkDevice device, const GfxDescriptorLayoutDesc& desc)
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

				assert(m_ResourceBindingMap.find(reflection.Binding) == m_ResourceBindingMap.end());
				ResourceBindingInfo bindingInfo
				{
					type,
				};
				m_ResourceBindingMap.insert(std::make_pair(reflection.Binding, bindingInfo));
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

	GfxVerifyVk(vkCreateDescriptorSetLayout(device, &createInfo, vkMemoryAllocator, &m_Layout));
}
