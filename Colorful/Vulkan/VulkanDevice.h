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

class vkInstance : public VulkanObject<VkInstance>
{
public:
	void create();
	void destroy();
};

class vkDebugUtilsMessenger : public VulkanObject<VkDebugUtilsMessengerEXT>
{
public:
	void create(const vkInstance &instance, bool8_t verbose);
	void destroy(const vkInstance &instance);
};

class vkPhysicalDevice : public VulkanObject<VkPhysicalDevice>
{
public:
	static std::vector<vkPhysicalDevice> enumeratePhysicalDevices(const vkInstance &instance);
};

class VulkanDevice : public VulkanObject<VkDevice>, public rDevice
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

	inline vkCommandBuffer allocCommandBuffer(VkCommandBufferLevel level, bool8_t signaleFence = true) const
	{
		return m_CommandPool.alloc(*this, level, signaleFence);
	}

	inline void freeCommandBuffer(vkCommandBuffer &commandBuffer) const
	{
		m_CommandPool.free(*this, commandBuffer);
	}

	inline vkDescriptorSet allocDescriptorSet(const vkDescriptorSetLayout &layout) const
	{
		return m_DescriptorPool.alloc(*this, layout);
	}

	inline const VkPhysicalDeviceLimits &getDeviceLimits() const
	{
		return m_DeviceLimits;
	}
public:
	rShader *createShader(eRShaderUsage usage, const std::string &shaderName);
	rTexture *createTexture(const std::string &textureName);
	rTexture *createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void *data,
		size_t dataSize);
	rBuffer *createBuffer(eRBufferBindFlags bindFlags, eRBufferUsage usage, size_t size, const void *pData);
	inline void destroyBuffer(rBuffer *buffer)
	{
		m_GpuResourcePool->destroyBuffer(buffer);
	}
	rRenderSurface *createDepthStencilView(uint32_t width, uint32_t height, eRFormat format);
	rRenderPass *createRenderPass(const vkSwapchain &swapchain, rFrameBufferDesc &desc);
	rSampler *createSampler(const rSamplerDesc &desc);

	const rGraphicsPipelineState *getGraphicsPipelineState(vkGraphicsPipeline *pipeline) const
	{
		return m_PipelinePool->getGraphicsPipelineState(pipeline);
	}
	
	inline vkGraphicsPipeline *getOrCreateGraphicsPipeline(const vkRenderPass &renderpass, const rGraphicsPipelineState &graphicsPipelineState)
	{
		return m_PipelinePool->getOrCreateGraphicsPipeline(renderpass, graphicsPipelineState);
	}

	inline vkCommandBuffer *getActiveCommandBuffer()
	{
		return m_CommandPool.getActiveCommandBuffer(*this);
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
			eSampler,
			ePipeline,
			eRenderPass,
			eFence,
			eSemaphore,
			eEvent,
			eImageView,
			eResourceType_MaxEnum
		};

		vkGpuResourcePool(const VulkanDevice &device)
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
				auto buffer = static_cast<vkBuffer *>(resource);
				delete buffer;
				it->second = nullptr;
				m_Resources.erase(resource->getID());
			}
		}

		inline void destroyBuffer(rGpuResource *resource)
		{
			destroyResource<eBuffer>(resource);
			pop(resource);
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
			if (buffer)
			{
				buffer->destroy(m_Device);
			}
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
		template<>
		inline void destroyResource<eSampler>(rGpuResource *resource)
		{
			auto sampler = static_cast<vkSampler *>(resource);
			assert(sampler);
			sampler->destroy(m_Device);
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
		const VulkanDevice &m_Device;
	};

	class vkPipelinePool
	{
	public:
		vkPipelinePool(const VulkanDevice &device);

		vkGraphicsPipeline *getOrCreateGraphicsPipeline(const vkRenderPass &renderpass, const rGraphicsPipelineState &graphicsPipelineState);

		inline void destroyAll()
		{
			for (auto &it : m_Pipelines)
			{
				it.second->destroy(m_Device);
			}
			m_Pipelines.clear();

			m_PipelineCache.destroy(m_Device);
		}

		const rGraphicsPipelineState *getGraphicsPipelineState(vkGraphicsPipeline *pipeline) const
		{
			for (uint32_t i = 0u; i < m_Pipelines.size(); ++i)
			{
				if (m_Pipelines[i].second == pipeline)
				{
					return &m_Pipelines[i].first;
				}
			}

			return nullptr;
		}
	protected:
	private:
		const VulkanDevice &m_Device;
		///std::unordered_map<rGraphicsPipelineState, vkPipeline *> m_Pipelines;
		std::vector<std::pair<rGraphicsPipelineState, vkPipeline *>> m_Pipelines;
		vkPipelineCache m_PipelineCache;
	};
private:
	VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties{};
	VkPhysicalDeviceFeatures m_DeviceFeatures{};
	VkPhysicalDeviceLimits m_DeviceLimits{};
	vkCommandPool m_CommandPool;
	vkDescriptorPool m_DescriptorPool;
	std::unique_ptr<vkGpuResourcePool> m_GpuResourcePool;
	std::unique_ptr<vkPipelinePool> m_PipelinePool;
};
