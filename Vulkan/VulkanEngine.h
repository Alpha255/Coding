#pragma once

#include "VulkanObject.h"

class VkEngine : public NoneCopyable
{
public:
	static VkEngine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<VkEngine, std::function<void(VkEngine *)>>
				(new VkEngine(), [](VkEngine *pEngine) { SafeDelete(pEngine) });
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

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height) {}

protected:
	VkEngine() = default;
	~VkEngine() = default;

	void InitLayerProperties();
	void CreateDevice(const VulkanPhysicalDevice &vkpDevice);
private:
	static std::unique_ptr<VkEngine, std::function<void(VkEngine *)>> s_Instance;

	VulkanDevice m_Device;
	VulkanSwapchain m_Swapchain;
	VulkanInstance m_Instance;

	bool m_Inited = false;
};
