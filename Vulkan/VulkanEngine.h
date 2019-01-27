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

	inline void ResetDefaultRenderSurfaces(const Vec4 &clearClr = Color::DarkBlue)
	{

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

	inline void SetConstantBuffer(const VulkanBuffer &constantBuffer, uint32_t slot, eRShaderType targetShader)
	{
		m_Context.SetConstantBuffer(constantBuffer, slot, targetShader);
	}

	inline void SetVertexBuffer(const VulkanBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U)
	{
		m_Context.SetVertexBuffer(vertexBuffer, stride, offset, slot);
	}

	inline void SetIndexBuffer(const VulkanBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
		m_Context.SetIndexBuffer(indexBuffer, fmt, offset);
	}

	inline void SetSamplerState(const VulkanSamplerState &samplerState, uint32_t slot, eRShaderType targetShader)
	{
		m_Context.SetSamplerState(samplerState, slot, targetShader);
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

	inline void SetShaderResourceView(const VulkanShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
		m_Context.SetShaderResourceView(shaderResourceView, slot, targetShader);
	}

	inline void SetScissorRect(const VulkanRect &rect, uint32_t slot = 0U)
	{
		m_Context.SetScissorRect(rect, slot);
	}

	//inline void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
	//{
	//}

	inline void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
	{
		m_Context.DrawIndexed(indexCount, startIndex, offset, primitive);
	}

	//inline void ClearRenderSurface(const Vec4 &color = Color::DarkBlue)
	//{

	//}

	inline VulkanCommandBuffer AllocCommandBuffer(VulkanCommandPool::ePoolType type, VulkanCommandPool::eBufferType bufferType)
	{
		assert(type < VulkanCommandPool::ePoolTypeCount && m_CommandPools[type].IsValid());
		VulkanCommandBuffer newCmdBuffer = m_CommandPools[type].Alloc(bufferType);

		RegisterCommandBuffer(type, newCmdBuffer);

		return newCmdBuffer;
	}

	inline void FreeCommandBuffer(VulkanCommandBuffer &cmdBuffer)
	{
		DeregisterCommandBuffer(cmdBuffer);

		m_CommandPools[cmdBuffer.GetPoolType()].Free(cmdBuffer);
	}

	inline VulkanCommandBufferList AllocCommandBufferList(VulkanCommandPool::ePoolType type, VulkanCommandPool::eBufferType bufferType, uint32_t count)
	{
		assert(type < VulkanCommandPool::ePoolTypeCount && m_CommandPools[type].IsValid());
		VulkanCommandBufferList newCmdBufferList = m_CommandPools[type].Alloc(bufferType, count);

		RegisterCommandBufferList(type, newCmdBufferList);

		return newCmdBufferList;
	}

	inline void FreeCommandBufferList(VulkanCommandBufferList &cmdBufferList)
	{
		DeregisterCommandBufferList(cmdBufferList);

		m_CommandPools[cmdBufferList.GetPoolType()].Free(cmdBufferList);
	}

	inline const VulkanPhysicalDevice &GetPhysicalDevice() const
	{
		return m_Device.GetPhysicalDevice();
	}

	inline VkDevice GetDevice() const
	{
		return m_Device.Get();
	}

	inline VkQueue GetQueue() const
	{
		return m_Device.GetQueue();
	}

	inline VkInstance GetInstance() const
	{
		return m_Instance.Get();
	}

	inline VulkanSwapchain &GetSwapchain()
	{
		return m_Swapchain;
	}

	inline const VulkanRenderPass &GetDefaultRenderPass() const
	{
		assert(m_DefaultRenderPass.IsValid());
		return m_DefaultRenderPass;
	}

	inline void Resize(uint32_t width, uint32_t height) 
	{
		m_Swapchain.Resize(width, height);
	}

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

protected:
	VulkanEngine() = default;
	~VulkanEngine();

	inline void RegisterCommandBuffer(VulkanCommandPool::ePoolType type, VulkanCommandBuffer &cmdBuffer)
	{
		m_CommandBuffers[type].insert(std::make_pair(m_CommandBufferCount, cmdBuffer));
		cmdBuffer.SetPoolType(type);
		++m_CommandBufferCount;
		cmdBuffer.SetIndex(m_CommandBufferCount);
	}

	inline void DeregisterCommandBuffer(VulkanCommandBuffer &cmdBuffer)
	{
		m_CommandBuffers[cmdBuffer.GetPoolType()].erase(cmdBuffer.GetIndex());
		cmdBuffer.SetPoolType(UINT32_MAX);
		cmdBuffer.SetIndex(UINT32_MAX);
		--m_CommandBufferCount;
	}

	inline void RegisterCommandBufferList(VulkanCommandPool::ePoolType type, VulkanCommandBufferList &cmdBufferList)
	{
		/// May need to...
		m_CommandBufferLists[type].insert(std::make_pair(m_CommandBufferCount, cmdBufferList));
		cmdBufferList.SetPoolType(type);
		m_CommandBufferCount += cmdBufferList.GetCount();
		cmdBufferList.SetIndex(m_CommandBufferCount);
	}

	inline void DeregisterCommandBufferList(VulkanCommandBufferList &cmdBufferList)
	{
		m_CommandBufferLists[cmdBufferList.GetPoolType()].erase(cmdBufferList.GetIndex());
		cmdBufferList.SetPoolType(UINT32_MAX);
		cmdBufferList.SetIndex(UINT32_MAX);
		m_CommandBufferCount -= cmdBufferList.GetCount();
	}

	void FreeCommandBuffers();
private:
	static std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> s_Instance;

	VulkanDevice m_Device;
	VulkanSwapchain m_Swapchain;
	VulkanInstance m_Instance;
	VulkanRenderPass m_DefaultRenderPass;
	VulkanContext m_Context;
	std::array<VulkanCommandPool, VulkanCommandPool::ePoolTypeCount> m_CommandPools;
	std::array<std::unordered_map<uint32_t, VulkanCommandBuffer>, VulkanCommandPool::ePoolTypeCount> m_CommandBuffers = {};
	std::array<std::unordered_map<uint32_t, VulkanCommandBufferList>, VulkanCommandPool::ePoolTypeCount> m_CommandBufferLists = {};
	uint32_t m_CommandBufferCount = 0U;

	bool m_Inited = false;
};
