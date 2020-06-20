#pragma once

#include "Colorful/Vulkan/VulkanInstance.h"
#include "Colorful/Vulkan/VulkanDevice.h"

class VulkanEngine : public GfxEngine
{
public:
	static void logError(uint32_t result);

	void initialize(uint64_t windowHandle, const Gear::Configurations& config) override final;
	void finalize() override final;

	void present() override final;

	void handleWindowResize(uint32_t width, uint32_t height) override final
	{
		m_Device.waitIdle();

		m_Swapchain->resize(width, height);
	}

	inline void acquireNextFrame() override final
	{
		m_Swapchain->acquireNextFrame();
	}

	GfxShaderPtr createVertexShader(const std::string& shaderName) override final
	{
		auto shader = std::make_shared<VulkanShader>(m_Device.logicalDevice(), eVertexShader, shaderName);
		return std::static_pointer_cast<GfxShader>(shader);
	}
	GfxShaderPtr createFragmentShader(const std::string& shaderName) override final
	{
		auto shader = std::make_shared<VulkanShader>(m_Device.logicalDevice(), eFragmentShader, shaderName);
		return std::static_pointer_cast<GfxShader>(shader);
	}

	GfxGpuBufferPtr createIndexBuffer(eRBufferUsage usage, size_t size, const void* data) override final
	{
		auto buffer = VulkanBufferPool::instance()->allocIndexBuffer(usage, size, data);
		return static_cast<GfxGpuBufferPtr>(buffer);
	}
	GfxGpuBufferPtr createVertexBuffer(eRBufferUsage usage, size_t size, const void* data) override final
	{
		auto buffer = VulkanBufferPool::instance()->allocVertexBuffer(usage, size, data);
		return static_cast<GfxGpuBufferPtr>(buffer);
	}
	GfxGpuBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		auto buffer = VulkanBufferPool::instance()->allocUniformBuffer(size, data);
		return static_cast<GfxGpuBufferPtr>(buffer);
	}

	GfxRenderSurfacePtr createDepthStencil(uint32_t width, uint32_t height, eRFormat format) override final
	{
		auto depthStencilView = std::make_shared<VulkanDepthStencil>(m_Device.logicalDevice(), width, height, format);
		m_ImageViewList.emplace_back(std::static_pointer_cast<VulkanImageView>(depthStencilView));
		return std::static_pointer_cast<GfxRenderSurface>(depthStencilView);
	}

	GfxRenderSurfacePtr createRenderTarget() override final
	{
		return nullptr;
	}

	GfxRenderPassPtr createRenderPass(GfxFrameBufferDesc& desc) override final
	{
		auto renderPass = std::make_shared<VulkanRenderPass>(m_Device.logicalDevice(), desc);
		m_RenderPassList.emplace_back(renderPass);
		return std::static_pointer_cast<GfxRenderPass>(renderPass);
	}

	GfxTexturePtr createTexture(const std::string& textureName) override final
	{
		auto texture = std::make_shared<VulkanImageView>(m_Device.logicalDevice(), textureName);
		m_ImageViewList.emplace_back(texture);
		return std::static_pointer_cast<GfxTexture>(texture);
	}
	GfxTexturePtr createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void* data,
		size_t dataSize) override final
	{
		auto texture = std::make_shared<VulkanImageView>(
			m_Device.logicalDevice(),
			type,
			format,
			width,
			height,
			depth,
			mipLevels,
			arrayLayers,
			data,
			dataSize);
		m_ImageViewList.emplace_back(texture);
		return std::static_pointer_cast<GfxTexture>(texture);
	}

	GfxSamplerPtr createSampler(const GfxSamplerDesc& desc) override final
	{
		auto sampler = std::make_shared<VulkanSampler>(m_Device.logicalDevice(), desc);
		m_SamplerList.emplace_back(sampler);
		return std::static_pointer_cast<GfxSampler>(sampler);
	}

	GfxFrameBufferPtr backBuffer() override final
	{
		return std::static_pointer_cast<GfxFrameBuffer>(m_Swapchain->backBuffer());
	}

	void bindGfxPipelineState(const GfxPipelineState* state) override final;

	void drawIndexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset) override final;

	void beginDebugMarker(const char8_t* name, Vec4 color) override final;
	void insertDebugMarker(const char8_t* name, Vec4 color) override final;
	void endDebugMarker() override final;

	template <typename T> static std::vector<const char8_t*> getSupportedProperties(
		const std::vector<T>& supportedProperties,
		const std::vector<const char8_t*>& targetProperties)
	{
		std::vector<const char8_t*> result;

		for each (auto propertyName in targetProperties)
		{
			for (auto it = supportedProperties.begin(); it != supportedProperties.end(); ++it)
			{
				if (_stricmp((const char8_t*)(&(*it)), propertyName) == 0)
				{
					result.emplace_back(propertyName);
					break;
				}
			}
		}

		return result;
	}
protected:
	struct CurrentPipelineState
	{
		enum ePipelineType
		{
			eGraphics,
			eCompute,
			eRayTracing
		};

		struct DynamicState
		{
			enum eDirtyFlags : uint8_t
			{
				eViewport = 1u,
				eScissor = 2u,
				eVertexBuffer = 4u,
				eIndexBuffer = 8u
			};
			uint8_t Dirty = 0u;
			inline bool8_t isDirty(eDirtyFlags flags) const
			{
				return flags & Dirty;
			}

			inline void setDirty(eDirtyFlags flags)
			{
				Dirty |= flags;
			}

			inline void setViewport(const GfxViewport& viewport)
			{
				if (Viewport != viewport)
				{
					Viewport = viewport;
					setDirty(eViewport);
				}
			}

			inline void setScissor(const GfxScissor& scissor)
			{
				if (Scissor != scissor)
				{
					Scissor = scissor;
					setDirty(eScissor);
				}
			}

			inline void setVertexBuffer(const GfxGpuBufferPtr& buffer)
			{
				if (VertexBuffer != buffer)
				{
					VertexBuffer = buffer;
					setDirty(eVertexBuffer);
				}
			}

			inline void setIndexBuffer(const GfxGpuBufferPtr& buffer)
			{
				if (IndexBuffer != buffer)
				{
					IndexBuffer = buffer;
					setDirty(eIndexBuffer);
				}
			}

			GfxViewport Viewport;
			GfxScissor Scissor;
			GfxGpuBufferPtr VertexBuffer = nullptr;
			GfxGpuBufferPtr IndexBuffer = nullptr;
		};

		ePipelineType Type = eGraphics;
		bool8_t Dirty = false;
		VulkanFrameBufferPtr FrameBuffer = nullptr;
		VulkanGraphicsPipelinePtr GfxPipeline = nullptr;
		VkDescriptorSet CurDescriptorSet = VK_NULL_HANDLE;
		const GfxPipelineState* GfxPipelineState = nullptr;
		DynamicState Dynamic;

		inline void reset()
		{
			Dirty = false;
			FrameBuffer = nullptr;
			GfxPipeline = nullptr;
			GfxPipelineState = nullptr;

			Dynamic.VertexBuffer = nullptr;
			Dynamic.IndexBuffer = nullptr;
			Dynamic.Viewport = GfxViewport();
			Dynamic.Scissor = GfxScissor();
			Dynamic.Dirty = 0u;
		}
	};

	void freeResources();
	void prepareForDraw();
	void setDynamicStates();
private:
	VulkanInstance m_Instance;
	VulkanDevice m_Device;
	VulkanSwapchainPtr m_Swapchain = nullptr;
	VulkanCommandBufferPtr m_ActiveCmdBuffer = nullptr;
	CurrentPipelineState m_CurPipelineState;

	std::vector<VulkanRenderPassPtr> m_RenderPassList;
	std::vector<VulkanImageViewPtr> m_ImageViewList;
	std::vector<VulkanSamplerPtr> m_SamplerList;
};
