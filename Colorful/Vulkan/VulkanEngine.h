#pragma once

#include "Colorful/Public/IEngine.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

class VulkanEngine : public IEngine
{
public:
	static VulkanEngine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>>
				(new VulkanEngine(), [](VulkanEngine *pEngine) { SafeDelete(pEngine) });
		}
		return *s_Instance;
	}

	inline void Flush()
	{
		m_Swapchain.Flush();
	}

	inline void SetVSync(bool bVSync)
	{
		m_Swapchain.SetVSync(bVSync);
	}

	inline void SetFullScreen(bool bFullScreen)
	{
		m_Swapchain.SetFullScreen(bFullScreen);
	}

	inline void ResetDefaultRenderSurfaces(const Vec4 &clearClr = Color::DarkBlue)
	{
		m_Context.SetClearColor(clearClr);
	}

	inline void SetViewport(const VulkanViewport &viewport, uint32_t slot = 0U)
	{
		m_Context.SetViewport(viewport, slot);
	}

	inline void SetInputLayout(const VulkanInputLayout &inputLayout)
	{
		m_Context.SetInputLayout(inputLayout);
	}

	inline void SetVertexShader(const VulkanVertexShader &vertexShader)
	{
		m_Context.SetVertexShader(vertexShader);
	}

	inline void SetPixelShader(const VulkanPixelShader &pixelShader)
	{
		m_Context.SetPixelShader(pixelShader);
	}

	inline void SetUniformBuffer(const VulkanBuffer &uniformBuffer, uint32_t slot, eRShaderType targetShader)
	{
		m_Context.SetUniformBuffer(uniformBuffer, slot, targetShader);
	}

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
		m_Context.SetVertexBuffer(vertexBuffer, stride, offset, slot);
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
		m_Context.SetIndexBuffer(indexBuffer, fmt, offset);
	}

	inline void SetRasterizerState(const VulkanRasterizerState &rasterizerState)
	{
		m_Context.SetRasterizerState(rasterizerState);
	}

	inline void SetDepthStencilState(const VulkanDepthStencilState &depthStencilState, uint32_t stencilRef)
	{
		m_Context.SetDepthStencilState(depthStencilState, stencilRef);
	}

	inline void SetBlendState(const VulkanBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF)
	{
		m_Context.SetBlendState(blendState, blendFactor, mask);
	}

	inline void SetSamplerState(const VulkanSamplerState &sampler, uint32_t slot, eRShaderType targetShader)
	{
		m_Context.SetSamplerState(sampler, slot, targetShader);
	}

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
		m_Context.SetShaderResourceView(shaderResourceView, slot, targetShader);
	}

	inline void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U)
	{
		m_Context.SetScissorRect(rect, slot);
	}

	inline void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
	{
		m_Context.DrawIndexed(indexCount, startIndex, offset, primitive);
	}

	inline VulkanContext &GetIMContext()
	{
		return m_Context;
	}

	inline void Resize(uint32_t width, uint32_t height)
	{
		m_Swapchain.Resize(width, height);
	}

	inline const VulkanInstance &GetInstance() const
	{
		return m_Instance;
	}

	inline const VulkanDevice &GetDevice() const
	{
		return m_Device;
	}

	inline const VulkanRenderPass &GetRenderPass() const
	{
		return m_RenderPass;
	}

	inline const VulkanSwapchain &GetSwapchain() const
	{
		return m_Swapchain;
	}

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);
	void Finalize();

	inline VulkanCommandBuffer AllocCommandBuffer(VulkanCommandPool::ePoolType type, VulkanCommandPool::eBufferType bufferType, uint32_t count)
	{
		assert(type < VulkanCommandPool::ePoolTypeCount && m_CommandPools[type].IsValid());
		return m_CommandPools[type].Alloc(bufferType, count);
	}

	inline void FreeCommandBuffer(VulkanCommandBuffer &cmdBuffer)
	{
		m_CommandPools[cmdBuffer.GetPoolType()].Free(cmdBuffer);
	}
protected:
	VulkanEngine() = default;
	~VulkanEngine() = default;

	static void LoadVulkanLibrary();
private:
	static std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> s_Instance;

	VulkanInstance m_Instance;
	VulkanDevice m_Device;
	VulkanSwapchain m_Swapchain;
	VulkanRenderPass m_RenderPass;
	VulkanContext m_Context;
	std::array<VulkanCommandPool, VulkanCommandPool::ePoolTypeCount> m_CommandPools;

	static ::HMODULE s_Library;
};


#include "VulkanDevice.h"

class vkEngine : public rEngine, public gear::singleton<vkEngine>
{
	singletonDeclare(vkEngine);
public:
	void initialize(uint64_t windowHandle, const gear::appConfig &config) override final;
	void finalize() override final {}

	void logError(uint32_t result) const override final;

	inline rDevicePtr getDevice() const override final
	{
		assert(m_Device);
		return m_Device;
	}

	inline rContextPtr getIMContext() const override final
	{
		assert(m_IMContext);
		return m_IMContext;
	}
protected:
private:
	vkInstancePtr m_Instance = nullptr;
	vkDebugReportCallbackPtr m_DebugReportCallback = nullptr;
	vkDevicePtr m_Device = nullptr;
	vkPhysicalDevicePtr m_PhysicalDevice = nullptr;
	vkContextPtr m_IMContext = nullptr;
};