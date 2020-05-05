#pragma once

#include "Colorful/Public/RInterface.h"

class d3d11Swapchain : public D3DObject<IDXGISwapChain1>
{
public:
	void create(uint64_t windowHandle,
		uint32_t width,
		uint32_t height,
		bool8_t vSync,
		bool8_t fullscreen,
		bool8_t tripleBuffer,
		const class dxgiFactory7 &inDxgiFactory,
		class d3d11Device &device);

	void resize(uint32_t width, uint32_t height);
protected:
private:
	bool8_t m_bVSync = false;
	bool8_t m_bFullScreen = false;
	bool8_t m_bTripleBuffer = false;
};
