#pragma once

#include "Colorful/Public/GfxResourceManager.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanTexture.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(VulkanResourceManager)
class VulkanResourceManager final : public GfxResourceManager
{
public:
	VulkanResourceManager(const VkDevice device, const VkPhysicalDevice physicalDevice)
		: m_Device(device)
		, m_PhysicalDevice(physicalDevice)
	{
		assert(device && physicalDevice);
	}

	ShaderPtr createShader(const ShaderDesc& desc) override final
	{
		return std::static_pointer_cast<Shader>(std::make_shared<VulkanShader>(m_Device, desc.Stage, desc.Binary));
	}

	TexturePtr createTexture(const TextureDesc& desc) override final
	{
		return std::static_pointer_cast<Texture>(std::make_shared<VulkanTexture>(m_Device, desc, VK_IMAGE_ASPECT_COLOR_BIT, VulkanTexture::ECreationMode::Immediately));
	}

	SamplerPtr createSampler(const SamplerDesc& desc) override final
	{
		return std::static_pointer_cast<Sampler>(std::make_shared<VulkanSampler>(m_Device, desc));
	}

	InputLayoutPtr createInputLayout(const std::vector<VertexInputDesc>& descs, const std::vector<uint32_t>&) override final
	{
		return std::static_pointer_cast<InputLayout>(std::make_shared<VulkanInputLayout>(descs));
	}

	GPUBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		return std::static_pointer_cast<GPUBuffer>(std::make_shared<VulkanBuffer>(m_Device, EBindFlags::Bind_UniformBuffer, EBufferUsage::Dynamic, size, data));
	}

	GPUBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return std::static_pointer_cast<GPUBuffer>(std::make_shared<VulkanBuffer>(m_Device, EBindFlags::Bind_IndexBuffer, usage, size, data));
	}

	GPUBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return std::static_pointer_cast<GPUBuffer>(std::make_shared<VulkanBuffer>(m_Device, EBindFlags::Bind_VertexBuffer, usage, size, data));
	}
protected:
private:
	const VkDevice m_Device;
	const VkPhysicalDevice m_PhysicalDevice;
};

NAMESPACE_END(Gfx)
