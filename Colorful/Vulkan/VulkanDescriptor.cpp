#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanEnumTranslator.h"

NAMESPACE_START(Gfx)

VulkanDescriptorPool::VulkanDescriptorPool()
{
	auto& limits = s_Device->limits();
	std::vector<VkDescriptorPoolSize> poolSizes = 
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, limits.maxDescriptorSetSamplers },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, limits.maxDescriptorSetSampledImages },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, limits.maxDescriptorSetSampledImages },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, limits.maxDescriptorSetStorageImages },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, limits.maxDescriptorSetUniformBuffers },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, limits.maxDescriptorSetStorageBuffers },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, limits.maxDescriptorSetUniformBuffers },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, limits.maxDescriptorSetStorageBuffers },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, limits.maxDescriptorSetUniformBuffersDynamic },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, limits.maxDescriptorSetStorageBuffersDynamic },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, limits.maxDescriptorSetInputAttachments }
	};

	VkDescriptorPoolCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0u,
		limits.maxBoundDescriptorSets,
		static_cast<uint32_t>(poolSizes.size()),
		poolSizes.data()
	};
	VERIFY_VK(vkCreateDescriptorPool(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	/// When a pool is destroyed, all descriptor sets allocated from the pool are implicitly freed and become invalid. 
	/// Descriptor sets allocated from a given pool do not need to be freed before destroying that descriptor pool.
	vkDestroyDescriptorPool(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

void VulkanDescriptorPool::reset()
{
	/// return all descriptor sets allocated from a given pool to the pool
	/// flags is reserved for future use
	/// Resetting a descriptor pool recycles all of the resources from all of the descriptor sets allocated from the descriptor pool back to the descriptor pool, 
	/// and the descriptor sets are implicitly freed.
	assert(isValid());
	vkResetDescriptorPool(s_Device->get(), get(), 0u);
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

	auto descriptorSet = create_shared<VulkanDescriptorSet>();
	VERIFY_VK(vkAllocateDescriptorSets(s_Device->get(), &allocInfo, descriptorSet->reference()));
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
