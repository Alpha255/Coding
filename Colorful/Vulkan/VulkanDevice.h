#pragma once

#include "Colorful/Vulkan/VulkanPipeline.h"
#include "Colorful/Vulkan/VulkanCommand.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanImageView.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanRenderPass.h"
#include "Colorful/Vulkan/VulkanPipeline.h"
#include "Colorful/Vulkan/VulkanEnum.h"
#include "Colorful/Vulkan/VulkanQueue.h"

class VulkanDevice : public GfxDevice
{
public:
	class VulkanPhysicalDevice : public VulkanObject<VkPhysicalDevice>
	{
	};

	class VulkanLogicalDevice : public VulkanObject<VkDevice>
	{
	};

	void create(VkInstance instance);

	void destroy();

	void waitIdle();

	VkPhysicalDevice physicalDevice() const
	{
		return m_PhysicalDevice.Handle;
	}

	VkDevice logicalDevice() const
	{
		return m_LogicalDevice.Handle;
	}

	inline bool8_t isDebugMakerAvaliable() const
	{
		return m_DebugMarkerAvaliable;
	}
protected:
private:
	VulkanPhysicalDevice m_PhysicalDevice;
	VulkanLogicalDevice m_LogicalDevice;
	bool8_t m_DebugMarkerAvaliable = false;
};
