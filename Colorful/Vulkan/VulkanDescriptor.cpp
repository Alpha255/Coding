#include "VulkanDescriptor.h"
#include "VulkanEngine.h"

void vkDescriptorPool::create(const vkDevice &device)
{
	assert(device.isValid() && !isValid());

	/// ToDo-May need a better way
	auto &deviceLimits = device.getDeviceLimits();

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

	VkDescriptorPool handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateDescriptorPool(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkDescriptorPool::resetPool(const vkDevice &device)
{
	/// return all descriptor sets allocated from a given pool to the pool
	/// flags is reserved for future use
	/// Resetting a descriptor pool recycles all of the resources from all of the descriptor sets allocated from the descriptor pool back to the descriptor pool, 
	/// and the descriptor sets are implicitly freed.
	assert(device.isValid() && isValid());
	vkResetDescriptorPool(*device, **this, 0u);
}

void vkDescriptorPool::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorPool(*device, **this, vkMemoryAllocator);
		reset();
	}
}

vkDescriptorSet vkDescriptorPool::alloc(const class vkDevice &device, const vkDescriptorSetLayout &layout) const
{
	assert(isValid() && layout.isValid());

	const VkDescriptorSetLayout descriptorSetlayout = *layout;
	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		**this,
		1u,
		&descriptorSetlayout
	};

	VkDescriptorSet handle = VK_NULL_HANDLE;
	rVerifyVk(vkAllocateDescriptorSets(*device, &allocInfo, &handle));

	vkDescriptorSet descriptorSet;
	descriptorSet.reset(handle);

	return descriptorSet;
}

void vkDescriptorSetLayout::create(const vkDevice &device, const rDescriptorLayoutDesc &desc)
{
	assert(device.isValid() && !isValid());

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		for (uint32_t j = 0u; j < desc[i].size(); ++j)
		{
			for (auto &reflection : desc[i])
			{
				VkDescriptorSetLayoutBinding binding
				{
					reflection.Binding,
					(VkDescriptorType)reflection.Type,
					1u,
					(VkShaderStageFlags)vkEngine::enumTranslator::toShaderStage(static_cast<eRShaderUsage>(i)),
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

	VkDescriptorSetLayout handle = VK_NULL_HANDLE;
	rVerifyVk(vkCreateDescriptorSetLayout(*device, &createInfo, vkMemoryAllocator, &handle));
	reset(handle);
}

void vkDescriptorSetLayout::destroy(const vkDevice &device)
{
	assert(device.isValid());

	if (isValid())
	{
		vkDestroyDescriptorSetLayout(*device, **this, vkMemoryAllocator);
		reset();
	}
}

void vkDescriptorUpdateTemplate::destroy(const vkDevice &device)
{
	/// A descriptor update template specifies a mapping from descriptor update information in host memory to descriptors in a descriptor set.
	/// It is designed to avoid passing redundant information to the driver when frequently updating the same set of descriptors in descriptor sets.
	/// For cases when an application wishes to update the same set of descriptors in multiple descriptor sets allocated using the same VkDescriptorSetLayout, 
	/// vkUpdateDescriptorSetWithTemplate can be used as a replacement for vkUpdateDescriptorSets.
	assert(device.isValid());

	if (isValid())
	{
		///vkDestroyDescriptorUpdateTemplate(*device, **this, vkMemoryAllocator);
		reset();
	}
}
