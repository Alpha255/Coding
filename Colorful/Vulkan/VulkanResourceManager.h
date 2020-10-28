#pragma once

#include "Colorful/Public/GfxResourceManager.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanShader.h"

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
		return nullptr;
	}

	SamplerPtr createSampler(const SamplerDesc& desc) override final
	{
		return nullptr;
	}

	UniformBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		return nullptr;
	}

	InputLayoutPtr createInputLayout(const std::vector<VertexInputDesc>& descs, const std::vector<uint32_t>&) override final
	{
		return std::static_pointer_cast<InputLayout>(std::make_shared<VulkanInputLayout>(descs));
	}

	IndexBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return nullptr;
	}

	VertexBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return nullptr;
	}
protected:
private:
	const VkDevice m_Device;
	const VkPhysicalDevice m_PhysicalDevice;
};

NAMESPACE_END(Gfx)
