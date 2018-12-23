#pragma once

#include "VulkanObject.h"
#include "VulkanSwapChain.h"
#include "D3DMath.h"

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
	}

	inline void SetVSync(bool bVSync)
	{
	}

	inline void SetFullScreen(bool bFullScreen)
	{
	}

	inline void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
	{
	}

	inline void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
	{
	}

	inline void ClearRenderSurface(const Vec4 &color = Color::DarkBlue)
	{

	}

	inline const VulkanPhysicalDevice &GetPhysicalDevice() const
	{
		assert(m_PhysicalDevice.IsValid());
		return m_PhysicalDevice;
	}

	inline const VulkanDevice &GetDevice() const
	{
		assert(m_Device.IsValid());
		return m_Device;
	}

	inline const VulkanInstance &GetInstance() const
	{
		assert(m_Instance.IsValid());
		return m_Instance;
	}

	inline VulkanSwapchain &GetSwapchain()
	{
		return m_Swapchain;
	}

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height) {}

protected:
	VulkanEngine() = default;
	~VulkanEngine() = default;

private:
	static std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> s_Instance;

	VulkanPhysicalDevice m_PhysicalDevice;
	VulkanDevice m_Device;
	VulkanSwapchain m_Swapchain;
	VulkanInstance m_Instance;

	bool m_Inited = false;
};
