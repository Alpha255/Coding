#pragma once

#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(D3D12Renderer)
class D3D12Renderer final : public GfxRenderer
{
public:
	void createDevice() override;
	void createSwapchain(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t vSync) override;

	void toggleFullScreen(bool8_t fullscreen) override;
	void toggleVSync(bool8_t vSync) override;

	void finalize() override;
protected:
private:
	D3D12DebugPtr m_Debug;
	D3D12DevicePtr m_Device;
};

NAMESPACE_END(Gfx)