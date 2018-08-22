#pragma once

#include "VulkanCommon.h"

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

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

protected:
	VulkanEngine() = default;
	~VulkanEngine() = default;

	void InitLayerProperties();
private:
	static std::unique_ptr<VulkanEngine, std::function<void(VulkanEngine *)>> s_Instance;

	bool m_Inited = false;
};
