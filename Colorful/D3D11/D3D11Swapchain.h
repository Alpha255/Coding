#pragma once

#include "Colorful/D3D11/D3D11Device.h"

class d3d11Swapchain : public d3dObject<IDXGISwapChain1>
{
public:
	void create(uint64_t windowHandle,
		uint32_t width,
		uint32_t height,
		bool8_t vSync,
		bool8_t fullscreen,
		bool8_t tripleBuffer,
		const dxgiFactory7Ptr &dxgiFactoryPtr,
		const d3d11DevicePtr &devicePtr);

	void resize(uint32_t width, uint32_t height);
protected:
private:
	bool8_t m_bVSync = false;
	bool8_t m_bFullScreen = false;
	bool8_t m_bTripleBuffer = false;
};
typedef std::shared_ptr<d3d11Swapchain> d3d11SwapchainPtr;
