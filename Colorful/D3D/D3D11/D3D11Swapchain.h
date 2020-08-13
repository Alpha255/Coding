#pragma once

#include "Colorful/Public/GfxDefinitions.h"

class D3D11Swapchain : public D3DObject<IDXGISwapChain1>
{
public:
	void create(uint64_t windowHandle,
		uint32_t width,
		uint32_t height,
		bool8_t vSync,
		bool8_t fullscreen,
		bool8_t tripleBuffer,
		const class DXGIFactory7 &inDxgiFactory,
		class D3D11Device &device);

	void resize(uint32_t width, uint32_t height);

	void present();
protected:
private:
	bool8_t m_bVSync = false;
	bool8_t m_bFullScreen = false;
	bool8_t m_bTripleBuffer = false;
};
