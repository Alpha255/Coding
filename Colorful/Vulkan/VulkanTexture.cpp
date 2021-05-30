#include "Colorful/Vulkan/VulkanTexture.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanEnumTranslator.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"
#include "Colorful/Vulkan/VulkanQueue.h"

NAMESPACE_START(Gfx)

VulkanTexture::VulkanTexture(const TextureDesc& desc, uint32_t aspectFlags)
	: Texture(desc)
{
	assert(desc.Format != EFormat::Unknown && desc.Type != ETextureType::Null);
	assert(desc.Layers <= s_Device->limits().maxImageArrayLayers);
	assert(
		(desc.Type == ETextureType::T_1D && desc.Width <= s_Device->limits().maxImageDimension1D) ||
		((desc.Type == ETextureType::T_2D || desc.Type == ETextureType::T_2D_Array) && 
				desc.Width <= s_Device->limits().maxImageDimension2D && 
				desc.Height <= s_Device->limits().maxImageDimension2D) ||		
		((desc.Type == ETextureType::T_Cube || desc.Type == ETextureType::T_Cube_Array) && 
			desc.Width <= s_Device->limits().maxImageDimensionCube &&
			desc.Height <= s_Device->limits().maxImageDimensionCube) ||
		(desc.Type == ETextureType::T_3D && desc.Height <= s_Device->limits().maxImageDimension3D));

	///VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

	VkImageCreateFlags createFlags = 0u;
	if (desc.Type == ETextureType::T_Cube || desc.Type == ETextureType::T_Cube_Array)
	{
		createFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (desc.BindFlags & EBindFlags::RenderTarget)
	{
		usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (desc.BindFlags & EBindFlags::DepthStencil)
	{
		usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (desc.BindFlags & EBindFlags::UnorderedAccess)
	{
		usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
	}
	if (desc.BindFlags & EBindFlags::ShaderResource)
	{
		usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}
	if (desc.BindFlags & EBindFlags::InputAttachment)
	{
		usageFlags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	}

	/// VkFormatProperties

	VkImageCreateInfo imageCreateInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		createFlags,
		VkEnumTranslator::imageType(desc.Type),
		VkEnumTranslator::format(desc.Format),
		{
			desc.Width,
			desc.Type == ETextureType::T_1D || desc.Type == ETextureType::T_1D_Array ? 1u : desc.Height,
			desc.Type == ETextureType::T_3D ? desc.Layers : 1u
		},
		desc.MipLevels,
		desc.ArraySize,
		static_cast<VkSampleCountFlagBits>(desc.SampleCount),
		VK_IMAGE_TILING_OPTIMAL,
		usageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};
	VERIFY_VK(vkCreateImage(s_Device->get(), &imageCreateInfo, VK_MEMORY_ALLOCATOR, &m_Image));

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(s_Device->get(), m_Image, &memReq);
	VkMemoryAllocateInfo allocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memReq.size,
		VulkanMemoryAllocator::instance().getMemoryTypeIndex(EBufferUsage::Immutable, memReq.memoryTypeBits),
	};
	VERIFY_VK(vkAllocateMemory(s_Device->get(), &allocateInfo, VK_MEMORY_ALLOCATOR, &m_Memory));
	VERIFY_VK(vkBindImageMemory(s_Device->get(), m_Image, m_Memory, 0u));

	VkImageSubresourceRange subresourceRange
	{
		aspectFlags,
		0u,
		VK_REMAINING_MIP_LEVELS,
		0u,
		VK_REMAINING_ARRAY_LAYERS /// 6 * ArraySize for cubemap
	};

	assert(!desc.GenMipmaps);  /// Generate mipmaps ???

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		m_Image,
		VkEnumTranslator::imageViewType(desc.Type),
		VkEnumTranslator::format(desc.Format),
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		subresourceRange
	};
	VERIFY_VK(vkCreateImageView(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
	setDebugName(desc.Name.c_str());

	/// Other situations depend on usge flags ??? 
	VkQueues::instance().enqueueTransferCommand(desc, m_Image, subresourceRange, EImageLayout::Undefined, EImageLayout::FragmentShaderRead);
}

VulkanTexture::VulkanTexture(VkImage image, VkFormat format)
	: Texture(TextureDesc{}, EDescriptorType::Null)
{
	/// Don't hold image object for back buffers, it's belong to swapchain

	assert(image && format != VkFormat::VK_FORMAT_UNDEFINED);

	VkImageViewCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0u,
		image,
		VK_IMAGE_VIEW_TYPE_2D,
		format,
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0u,
			1u,
			0u,
			1u
		}
	};

	m_Format = FormatAttribute::attribute_Vk(format).Format;
	VERIFY_VK(vkCreateImageView(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanTexture::~VulkanTexture()
{
	if (m_Memory)
	{
		vkFreeMemory(s_Device->get(), m_Memory, VK_MEMORY_ALLOCATOR);
	}
	if (m_Image)
	{
		vkDestroyImage(s_Device->get(), m_Image, VK_MEMORY_ALLOCATOR);
	}

	vkDestroyImageView(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);

	m_Image = VK_NULL_HANDLE;
	m_Memory = VK_NULL_HANDLE;
}

VulkanSampler::VulkanSampler(const SamplerDesc& desc)
	: Sampler(EDescriptorType::Sampler)
{
	VkSamplerCreateInfo createInfo
	{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0u,
		VkEnumTranslator::textureFilter(desc.MinMagFilter),
		VkEnumTranslator::textureFilter(desc.MinMagFilter),
		desc.MinMagFilter == ETextureFilter::Nearest ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkEnumTranslator::samplerAddressMode(desc.AddressModeU),
		VkEnumTranslator::samplerAddressMode(desc.AddressModeV),
		VkEnumTranslator::samplerAddressMode(desc.AddressModeW),
		desc.MipLodBias,
		desc.MinMagFilter == ETextureFilter::Anisotropic ? true : false,
		static_cast<float32_t>(desc.MaxAnisotropy),
		false,
		VkEnumTranslator::compareFunc(desc.CompareOp),
		desc.MinLod,
		desc.MaxLod,
		VkEnumTranslator::borderColor(desc.BorderColor),
		false
	};
	VERIFY_VK(vkCreateSampler(s_Device->get(), &createInfo, VK_MEMORY_ALLOCATOR, reference()));
}

VulkanSampler::~VulkanSampler()
{
	vkDestroySampler(s_Device->get(), get(), VK_MEMORY_ALLOCATOR);
}

NAMESPACE_END(Gfx)