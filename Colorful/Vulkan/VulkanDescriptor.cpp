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
	descriptorSet.m_Layout.create(m_Device, desc);

	VkDescriptorSetAllocateInfo allocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		Handle,
		1u,
		&descriptorSet.m_Layout.Handle
	};

	GfxVerifyVk(vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptorSet.Handle));

	return descriptorSet;
}

void VulkanDescriptorSet::VulkanDescriptorSetLayout::create(VkDevice device, const GfxDescriptorLayoutDesc& desc)
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
				VkDescriptorSetLayoutBinding binding
				{
					reflection.Binding,
					(VkDescriptorType)reflection.Type,
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
