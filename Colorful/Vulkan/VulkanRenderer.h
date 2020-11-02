#pragma once

#if 0
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

	void bindGfxPipelineState(const GfxPipelineState* state) override final;

	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset) override final;
	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex) override final;

	void beginDebugMarker(const char8_t* name, Vec4 color) override final;
	void insertDebugMarker(const char8_t* name, Vec4 color) override final;
	void endDebugMarker() override final;
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

#endif

#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanResourceManager.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(VulkanRender)
class VulkanRender final : public GfxRenderer
{
public:
	void createDevice() override;
	void createSwapchain(uint64_t instance, uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) override;

	void toggleFullScreen(bool8_t fullscreen) override;
	void toggleVSync(bool8_t VSync) override;

	void finalize() override;
protected:
private:
	VulkanInstancePtr m_Instance = nullptr;
	VulkanDebugUtilsMessengerPtr m_DebugUtilsMessenger = nullptr;
	VulkanDevicePtr m_Device = nullptr;
	VulkanSwapchainPtr m_Swapchain = nullptr;
	VulkanResourceManagerPtr m_ResourceManager = nullptr;
};

NAMESPACE_END(Gfx)
