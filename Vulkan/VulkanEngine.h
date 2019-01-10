#pragma once

#include "VulkanContext.h"

class VulkanEngine : public NoneCopyable
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

	inline void SetViewport(const VulkanViewport &viewport, uint32_t slot = 0U)
	{
	}

	inline void SetInputLayout(const VulkanInputLayout &inputLayout)
	{
	}

	inline void SetVertexShader(const VulkanVertexShader &vertexShader)
	{
	}

	inline void SetPixelShader(const VulkanPixelShader &pixelShader)
	{
	}

	inline void SetConstantBuffer(const VulkanBuffer &constantBuffer, uint32_t slot, eRShaderType targetShader)
	{
	}

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
	}

	inline void SetSamplerState(const VulkanSamplerState &samplerState, uint32_t slot, eRShaderType targetShader)
	{
	}

	inline void SetRasterizerState(const VulkanRasterizerState &rasterizerState)
	{
	}

	inline void SetDepthStencilState(const VulkanDepthStencilState &depthStencilState, uint32_t stencilRef)
	{
	}

	inline void SetBlendState(const VulkanBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF)
	{
	}

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
	}

	inline void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U)
	{
	}

	//inline void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
	//{
	//}

	inline void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
	{
	}

	//inline void ClearRenderSurface(const Vec4 &color = Color::DarkBlue)
	//{

	//}

	inline VulkanCommandBuffer AllocCommandBuffer(VulkanCommandPool::ePoolType type, VulkanCommandPool::eBufferType bufferType)
	{
		assert(type < VulkanCommandPool::ePoolTypeCount && m_CommandPools[type].IsValid());
		return m_CommandPools[type].Alloc(bufferType);
	}

	inline const VulkanDevice &GetVulkanDevice() const
	{
		return m_Device;
	}

	inline VkDevice GetDevice() const
	{
		return m_Device.GetLogicalDevice();
	}

	inline VkInstance GetInstance() const
	{
		return m_Instance.Get();
	}

	inline VulkanSwapchain &GetSwapchain()
	{
		return m_Swapchain;
	}

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	inline void Resize(uint32_t width, uint32_t height) 
	{
		m_Swapchain.Resize(width, height);
	}

protected:
	VulkanEngine() = default;
	~VulkanEngine() = default;

private:
	static std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> s_Instance;

	VulkanDevice m_Device;
	VulkanSwapchain m_Swapchain;
	VulkanInstance m_Instance;
	std::array<VulkanCommandPool, VulkanCommandPool::ePoolTypeCount> m_CommandPools;

	bool m_Inited = false;
};
