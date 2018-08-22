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

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

protected:
	VkEngine() = default;
	~VkEngine() = default;

	void InitLayerProperties();
private:
	static std::unique_ptr<VkEngine, std::function<void(VkEngine *)>> s_Instance;

	bool m_Inited = false;
};
