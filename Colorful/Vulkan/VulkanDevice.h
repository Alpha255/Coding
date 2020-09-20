#pragma once

#include "Colorful/Vulkan/VulkanInstance.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(VulkanDevice)
class VulkanDevice final : public VkObject<VkDevice_T>, public Adapter
{
public:
	VulkanDevice(VkInstance instance);

	~VulkanDevice()
	{
		wait();

		vkDestroyDevice(get(), VK_MEMORY_ALLOCATOR);
	}

	void wait()
	{
		assert(isValid());
		VERIFY_VK(vkDeviceWaitIdle(get()));
	}

	const bool8_t isDebugMarkerAvailable() const
	{
		return m_DebugMarkerAvailable;
	}

	VkPhysicalDevice physicalDevice() const
	{
		assert(m_PhysicalDevice);
		return m_PhysicalDevice;
	}
protected:
private:
	bool8_t m_DebugMarkerAvailable = false;
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
};

NAMESPACE_END(Gfx)
