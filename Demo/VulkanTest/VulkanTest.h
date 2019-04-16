#pragma once

#include <Windows.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <array>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "Base/Base.h"

#define check(func) if (VK_SUCCESS != func) { assert(0); }

extern VkDevice vkDevice;

struct VkQueueFamilyIndex
{
	uint32_t Graphics = 0U;
	uint32_t Compute = 0U;
	uint32_t Transfer = 0U;
};

struct VkBackBuffer
{
	VkImage vkImage = VK_NULL_HANDLE;
	VkImageView vkImageView = VK_NULL_HANDLE;
};

struct VkDepthStencilView
{
	VkImage vkImage = VK_NULL_HANDLE;
	VkDeviceMemory vkMemory = VK_NULL_HANDLE;
	VkImageView vkImageView = VK_NULL_HANDLE;
};

struct VkMemoryBuffer
{
	VkBuffer Buffer = VK_NULL_HANDLE;
	VkDeviceMemory Memory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo DescriptorInfo = {};
	VkDeviceSize Size = 0U;
	VkDeviceSize Alignment = 0U;
	void *MappedMemory = nullptr;
	VkBufferUsageFlags Usage = 0U;
	VkMemoryPropertyFlags Property = 0U;

	inline void Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0U)
	{
		check(vkMapMemory(vkDevice, Memory, offset, size, 0U, &MappedMemory));
	}

	inline void UnMap()
	{
		if (MappedMemory)
		{
			vkUnmapMemory(vkDevice, Memory);
			MappedMemory = nullptr;
		}
	}

	inline void Bind(VkDeviceSize offset = 0U)
	{
		check(vkBindBufferMemory(vkDevice, Buffer, Memory, offset));
	}

	inline void SetDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0U)
	{
		DescriptorInfo.offset = offset;
		DescriptorInfo.buffer = Buffer;
		DescriptorInfo.range = size;
	}

	inline void Copy(const void *pData, VkDeviceSize size)
	{
		if (MappedMemory)
		{
			memcpy(MappedMemory, pData, size);
		}
	}

	inline void Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0U)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = Memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		check(vkFlushMappedMemoryRanges(vkDevice, 1, &mappedRange));
	}

	inline void Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = Memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		check(vkInvalidateMappedMemoryRanges(vkDevice, 1, &mappedRange));
	}

	inline void Destroy()
	{
		if (Buffer)
		{
			vkDestroyBuffer(vkDevice, Buffer, nullptr);
		}
		if (Memory)
		{
			vkFreeMemory(vkDevice, Memory, nullptr);
		}
	}
};

struct VkPushConstBlock 
{
	Vec2 Scale;
	Vec2 Translate;
};

bool ExtensionSupported(std::string extension);
uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);
void CreateInstance();
void CreateDevice();
VkCommandPool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
VkFormat GetSupportedDepthFormat();
void MakeWindow(const wchar_t *pTitle, uint32_t width, uint32_t height, uint32_t windowStyle);
void Resize(uint32_t width, uint32_t height);
void InitSwapchain(::HINSTANCE hInst);
void CreateSwapchain();
void CreateCommandBuffers();
void CreateFence();
void CreateDepthStencilView();
void CreateRenderpass();
void CreatePipelineCache();
void CreateFrameBuffers();
VkShaderModule CreateShader(const char *fileName);
void CreateShaders();
void InitImGUI();
VkMemoryBuffer CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, void *data = nullptr);
VkCommandBuffer AllocCommandBuffer(VkCommandBufferLevel level, uint32_t bufferCount, bool begin = false);
void SetImageLayout(
	VkCommandBuffer cmdbuffer,
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask);
void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true);
void BuildCommandBuffers();
void DrawUI(VkCommandBuffer commandBuffer);
void RenderLoop();
void Frame();
void UpdateUI();
bool UpdateUIBuffer();
