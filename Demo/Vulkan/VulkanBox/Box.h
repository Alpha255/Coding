#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "VulkanPool.h"
#include "VulkanContext.h"

class AppBox : public RenderApp
{
public:
	AppBox()
	{
		m_bRenderGUI = false;
	}

	~AppBox() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	struct Frame
	{
		VulkanFence Fence;
		VulkanCommandBuffer CmdBuffer;
	};

	struct BackBuffer
	{
		uint32_t Index = 0;
		VulkanSemaphore AcquireSemaphore;
		VulkanSemaphore RenderSemaphore;
		VulkanFence PresentFence;
	};
private:
	Geometry::Mesh m_Box;
	VulkanContext m_Context;
	VulkanVertexShader m_VS;
	VulkanPixelShader m_PS;

	VulkanCommandPool m_CmdPool;
	VulkanCommandBuffer m_CmdBuffer;

	VulkanSemaphore m_Wait;
	VulkanFence m_Fence;
};
