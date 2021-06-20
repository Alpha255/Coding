#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanTexture.h"
#include "Colorful/Vulkan/VulkanCommandPool.h"
#include "Colorful/Vulkan/VulkanAsync.h"

NAMESPACE_START(Gfx)

class VulkanQueue : public VkDeviceObject<VkQueue_T>
{
public:
	VulkanQueue(uint32_t queueFamilyIndex, uint32_t commandPoolCreateFlags);

	void submit(VulkanCommandBufferPtr cmdBuffer);

	void waitIdle();

	VulkanCommandBufferPtr getActiveCommand()
	{
		return m_MainCommandBufferPool->getOrAlloc();
	}

	inline VkSemaphore executionsCompleteSemaphore() const
	{
		return m_ExecutionsComplete->get();
	}
protected:
private:
	uint32_t m_QueueFamilyIndex = ~0u;
	VulkanCommandBufferPoolPtr m_MainCommandBufferPool = nullptr;
	VulkanSemaphorePtr m_ExecutionsComplete = nullptr;
};

class VkQueues
{
public:
	static VkQueues& instance();

	void initialize(uint32_t graphicsQueueFamilyIndex, uint32_t transferQueueFamilyIndex, uint32_t computeQueueFamilyIndex);
	void finalize();

	void submit();

	void enqueueTransferCommand(
		const TextureDesc& desc,
		VkImage image,
		VkImageSubresourceRange& subresourceRange,
		Texture::EImageLayout srcLayout,
		Texture::EImageLayout dstLayout)
	{
		m_TransferCommands.emplace(create_shared<VkTransferCommand>(desc, image, subresourceRange, srcLayout, dstLayout));
	}

	void enqueueTransferCommand(
		VkBuffer buffer,
		VkBufferCopy&& bufferRange,
		const void* data)
	{
		m_TransferCommands.emplace(create_shared<VkTransferCommand>(buffer, bufferRange, data));
	}

	void queuePresent(VkSwapchainKHR swapchain, VkSemaphore presentCompleteSemaphore, uint32_t* imageIndices);

	inline VulkanCommandBufferPtr getActiveGraphicsCommand()
	{
		return m_GraphicsQueue->getActiveCommand();
	}
protected:
	struct VkTransferCommand
	{
		VulkanBufferPtr StagingBuffer = nullptr;
		VkImage DstImage = VK_NULL_HANDLE;
		VkImageSubresourceRange ImageSubresourceRange{};
		Texture::EImageLayout SrcLayout = Texture::EImageLayout::Undefined;
		Texture::EImageLayout DstLayout = Texture::EImageLayout::Undefined;
		std::vector<VkBufferImageCopy> ImageCopyRanges;

		VkBuffer DstBuffer = VK_NULL_HANDLE;
		VkBufferCopy BufferRange{};

		VkTransferCommand(
			const TextureDesc& desc,
			VkImage image,
			const VkImageSubresourceRange& subresourceRange,
			Texture::EImageLayout srcLayout,
			Texture::EImageLayout dstLayout);
		VkTransferCommand(VkBuffer buffer, const VkBufferCopy& bufferRange, const void* data);

	protected:
		static void getImageBarrierMasks(
			Texture::EImageLayout layout, 
			__out VkPipelineStageFlags& stageFlags, 
			__out VkAccessFlags& accessFlags, 
			__out VkImageLayout& imageLayout);
	};
	using VkTransferCommandPtr = std::shared_ptr<VkTransferCommand>;

	void submitTransferCommands();
private:
	VulkanQueuePtr m_GraphicsQueue = nullptr;
	VulkanQueuePtr m_TransferQueue = nullptr;
	VulkanQueuePtr m_ComputeQueue = nullptr;

	std::queue<VkTransferCommandPtr> m_TransferCommands;
};

NAMESPACE_END(Gfx)
