#pragma once

#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "VulkanDescriptor.h"

class vkInstance : public vkObject<VkInstance>
{
public:
	void create();
	void destroy();
};

class vkDebugUtilsMessenger : public vkObject<VkDebugUtilsMessengerEXT>
{
public:
	void create(const vkInstance &instance, bool8_t verbose);
	void destroy(const vkInstance &instance);
};

class vkPhysicalDevice : public vkObject<VkPhysicalDevice>
{
public:
	static std::vector<vkPhysicalDevice> enumeratePhysicalDevices(const vkInstance &instance);
};

class vkDevice : public vkObject<VkDevice>, public rDevice
{
public:
	uint32_t create(
		const std::vector<vkPhysicalDevice> &physicalDevices,
		uint32_t &graphicsQueueIndex,
		uint32_t &computeQueueIndex,
		uint32_t &transferQueueIndex
	);

	void waitIdle();

	vkFence *createFence(vkFence::eFenceStatus status, bool8_t autoReset = false);
	vkSemaphore *createSemaphore(bool8_t autoReset = false);
	vkEvent *createEvent(bool8_t autoReset = false);

	void destroy();

	uint32_t getMemoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const;

	inline vkCommandBuffer allocCommandBuffer(VkCommandBufferLevel level) const
	{
		return m_CommandPool.allocCommandBuffer(*this, level);
	}

	inline void freeCommandBuffer(vkCommandBuffer &commandBuffer) const
	{
		m_CommandPool.freeCommandBuffer(*this, commandBuffer);
	}

	inline const VkPhysicalDeviceLimits &getDeviceLimits() const
	{
		return m_DeviceLimits;
	}
public:
	rShader *createShader(eRShaderUsage usage, const std::string &shaderName);
	rTexture *createTexture(const std::string &textureName);
	rBuffer *createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
protected:
	class vkGpuResourcePool
	{
	public:
		enum eResourceType
		{
			eShader,
			eTexture,
			eBuffer,
			eResourceType_MaxEnum
		};

		vkGpuResourcePool(const vkDevice &device)
			: m_Device(device)
		{
		}

		template<eResourceType Type>
		inline void push(rGpuResource *resource)
		{
			assert(resource);
			uint64_t id = makeID(Type);
			resource->setID(id);
			m_Resources.insert(std::make_pair(id, resource));
		}

		inline void pop(rGpuResource *resource)
		{
			assert(resource);
			auto it = m_Resources.find(resource->getID());
			if (it != m_Resources.end())
			{
				m_Resources.erase(resource->getID());
			}
		}

		void destory(rGpuResource *resource);
		void destoryAll();
	protected:
		inline uint64_t makeID(eResourceType type)
		{
			/// Duplicate ????????
			uint64_t id = m_IDPool++;
			id |= (((uint64_t)type) << 32ull);
			return id;
		}

		inline uint32_t getResourceID(uint64_t id) const
		{
			return (uint32_t)((id << 32ull) >> 32ull);
		}

		inline eResourceType getResourceType(uint64_t id) const
		{
			return (eResourceType)(id >> 32ull);
		}
	private:
		uint32_t m_IDPool = 0u;
		std::unordered_map<uint64_t, rGpuResource *> m_Resources;
		const vkDevice &m_Device;
	};
private:
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	VkPhysicalDeviceFeatures m_DeviceFeatures{};
	VkPhysicalDeviceLimits m_DeviceLimits{};
	vkCommandPool m_CommandPool;
	vkPipelineCache m_PipelineCache;
	std::unique_ptr<vkGpuResourcePool> m_GpuResourcePool;
};

class vkDeviceQueue : public vkObject<VkQueue>
{
public:
	void create(uint32_t queueFamilyIndex, const vkDevice &device)
	{
		assert(!isValid() && device.isValid());

		/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
		/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.
		m_FamilyIndex = queueFamilyIndex;
		VkQueue handle = VK_NULL_HANDLE;
		vkGetDeviceQueue(*device, queueFamilyIndex, 0u, &handle);
		reset(handle);

		/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device
	}

	void waitIdle();
protected:
private:
	uint32_t m_FamilyIndex = UINT32_MAX;
};
