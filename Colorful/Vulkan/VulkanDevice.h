#pragma once

#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanView.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanRenderPass.h"
#include "Colorful/Vulkan/VulkanPipeline.h"

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

	vkFence *createFence(vkFence::eFenceStatus status) const;
	vkSemaphore *createSemaphore() const;
	vkEvent *createEvent() const;

	void destroy();

	uint32_t getMemoryTypeIndex(eRBufferUsage usage, uint32_t memoryTypeBits) const;

	inline vkCommandBuffer allocCommandBuffer(VkCommandBufferLevel level) const
	{
		return m_CommandPool.alloc(*this, level);
	}

	inline void freeCommandBuffer(vkCommandBuffer &commandBuffer) const
	{
		m_CommandPool.free(*this, commandBuffer);
	}

	inline const VkPhysicalDeviceLimits &getDeviceLimits() const
	{
		return m_DeviceLimits;
	}
public:
	rShader *createShader(eRShaderUsage usage, const std::string &shaderName);
	rTexture *createTexture(const std::string &textureName);
	rBuffer *createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
	rRenderSurface *createDepthStencilView(uint32_t width, uint32_t height, eRFormat format);
	rRenderPass *createRenderPass(const vkSwapchain &swapchain, rFrameBufferDesc &desc);
	
	inline vkGraphicsPipeline *getOrCreateGraphicsPipeline(const rGraphicsPipelineState &graphicsPipelineState)
	{
		return m_PipelinePool->getOrCreateGraphicsPipeline(graphicsPipelineState);
	}
protected:
	class vkGpuResourcePool
	{
	public:
		enum eResourceType
		{
			eShader,
			eTexture,
			eBuffer,
			ePipeline,
			eRenderPass,
			eFence,
			eSemaphore,
			eEvent,
			eImageView,
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
			assert(m_Resources.find(id) == m_Resources.end());
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

		inline void destroy(rGpuResource *)
		{
			///eResourceType type = getResourceType(resource->getID());
			//destroyResource<type>(resource);
		}
		void destoryAll();
	protected:
		template<eResourceType Type>
		inline void destroyResource(rGpuResource *resource)
		{
			assert(0);
		}
		template<>
		inline void destroyResource<eShader>(rGpuResource *resource)
		{
			auto shader = static_cast<vkShader *>(resource);
			assert(shader);
			shader->destroy(m_Device);
			//pop(resource);
		}
		template<>
		inline void destroyResource<eBuffer>(rGpuResource *resource)
		{
			auto buffer = static_cast<vkBuffer *>(resource);
			assert(buffer);
			buffer->destroy(m_Device);
			//pop(resource);
		}
		template<>
		inline void destroyResource<eImageView>(rGpuResource *resource)
		{
			auto imageView = static_cast<vkImageView *>(resource);
			assert(imageView);
			imageView->destroy(m_Device);
			//pop(resource);
		}
		template<>
		inline void destroyResource<eRenderPass>(rGpuResource *resource)
		{
			auto renderPass = static_cast<vkRenderPass *>(resource);
			assert(renderPass);
			renderPass->destroy(m_Device);
			//pop(resource);
		}
		template<>
		inline void destroyResource<ePipeline>(rGpuResource *resource)
		{
			auto pipeline = static_cast<vkPipeline *>(resource);
			assert(pipeline);
			pipeline->destroy(m_Device);
			//pop(resource);
		}

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

	class vkPipelinePool
	{
	public:
		vkPipelinePool(const vkDevice &device)
			: m_Device(device)
		{
		}

		vkGraphicsPipeline *getOrCreateGraphicsPipeline(const rGraphicsPipelineState &graphicsPipelineState);

		inline void destroyAll()
		{
			for (auto &it : m_Pipelines)
			{
				it.first->destroy(m_Device);
			}
			m_Pipelines.clear();
		}
	protected:
	private:
		const vkDevice &m_Device;
		std::unordered_map<vkPipeline *, rGraphicsPipelineState> m_Pipelines;
	};
private:
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	VkPhysicalDeviceFeatures m_DeviceFeatures{};
	VkPhysicalDeviceLimits m_DeviceLimits{};
	vkCommandPool m_CommandPool;
	vkPipelineCache m_PipelineCache;
	std::unique_ptr<vkGpuResourcePool> m_GpuResourcePool;
	std::unique_ptr<vkPipelinePool> m_PipelinePool;
};
