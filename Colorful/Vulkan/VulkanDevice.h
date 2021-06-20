#pragma once

#include "Colorful/Vulkan/VulkanInstance.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanTexture.h"
#include "Colorful/Vulkan/VulkanBuffer.h"

NAMESPACE_START(Gfx)

class VulkanDevice final : public VkObject<VkDevice_T>, public GfxDevice
{
public:
	enum EQueueFamilyType
	{
		Graphics,
		Transfer,
		Compute,
		TypeCount
	};

	VulkanDevice(VkInstance instance);

	~VulkanDevice()
	{
		waitIdle();

		vkDestroyDevice(get(), VK_MEMORY_ALLOCATOR);
	}

	void waitIdle()
	{
		assert(isValid());
		VERIFY_VK(vkDeviceWaitIdle(get()));
	}

	const bool8_t enableProfiler() const
	{
		return m_EnableProfiler;
	}

	VkPhysicalDevice physicalDevice() const
	{
		assert(m_PhysicalDevice);
		return m_PhysicalDevice;
	}

	VkInstance instance() const
	{
		assert(m_Instance);
		return m_Instance;
	}

	const VkPhysicalDeviceLimits limits() const
	{
		assert(isValid());
		return m_PhysicalLimits;
	}

	const VkPhysicalDeviceSparseProperties sparseProperties() const
	{
		assert(isValid());
		return m_PhysicalDeviceSparseProperties;
	}

	const VkPhysicalDeviceFeatures features() const
	{
		assert(isValid());
		return m_Features;
	}

	ShaderPtr createVertexShader(ShaderDesc& desc) override final
	{
		return createShader(desc);
	}
	ShaderPtr createHullShader(ShaderDesc& desc) override final
	{
		return createShader(desc);
	}
	ShaderPtr createDomainShader(ShaderDesc& desc) override final
	{
		return createShader(desc);
	}
	ShaderPtr createGeometryShader(ShaderDesc& desc) override final
	{
		return createShader(desc);
	}
	ShaderPtr createFragmentShader(ShaderDesc& desc) override final
	{
		return createShader(desc);
	}
	ShaderPtr createComputeShader(ShaderDesc& desc) override final
	{
		return createShader(desc);
	}

	TexturePtr create1DTexture(const TextureDesc& desc) override final
	{
		return createTexture(desc);
	}
	TexturePtr create2DTexture(const TextureDesc& desc) override final
	{
		return createTexture(desc);
	}
	TexturePtr create3DTexture(const TextureDesc& desc) override final
	{
		return createTexture(desc);
	}

	TexturePtr createRenderTarget(const TextureDesc& desc) override final
	{
		return createTexture(desc);
	}

	TexturePtr createDepthStencil(const TextureDesc& desc) override final
	{
		return createTexture(desc);
	}

	InputLayoutPtr createInputLayout(const VertexInputLayoutDesc& desc, const ShaderDesc&) override final
	{
		return create_shared<VulkanInputLayout>(desc);
	}

	GpuBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		return create_shared<VulkanBuffer>(EBindFlags::UniformBuffer, EBufferUsage::Dynamic, size, data);
	}

	GpuBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return create_shared<VulkanBuffer>(EBindFlags::IndexBuffer, usage, size, data);
	}

	GpuBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return create_shared<VulkanBuffer>(EBindFlags::VertexBuffer, usage, size, data);
	}

	template<EQueueFamilyType Type>
	inline uint32_t getQueueFamilyIndex() const
	{
		assert(Type < EQueueFamilyType::TypeCount);
		return m_QueueFamilyIndices[Type];
	}

	CommandBufferPtr getActiveCommandBuffer() override final;
protected:
	ShaderPtr createShader(ShaderDesc& desc)
	{
		return create_shared<VulkanShader>(desc);
	}

	TexturePtr createTexture(const TextureDesc& desc)
	{
		return create_shared<VulkanTexture>(desc, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	SamplerPtr createSampler_native(const SamplerDesc& desc) override final
	{
		return create_shared<VulkanSampler>(desc);
	}
private:
	bool8_t m_EnableProfiler = false;
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkInstance m_Instance = VK_NULL_HANDLE;
	VkPhysicalDeviceLimits m_PhysicalLimits;
	VkPhysicalDeviceSparseProperties m_PhysicalDeviceSparseProperties;
	VkPhysicalDeviceFeatures m_Features;
	std::array<uint32_t, EQueueFamilyType::TypeCount> m_QueueFamilyIndices;
};

NAMESPACE_END(Gfx)
