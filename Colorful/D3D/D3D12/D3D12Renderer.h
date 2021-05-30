#pragma once

#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(Gfx)

class D3D12Renderer final : public GfxRenderer
{
public:
	void initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) override;

	void toggleFullScreen(bool8_t fullscreen) override;
	void toggleVSync(bool8_t VSync) override;

	GfxDevice* device() override final
	{
		return m_Device.get();
	}

	void finalize() override;
protected:
private:
	D3D12DebugPtr m_Debug;
	D3D12DevicePtr m_Device;
};
DECLARE_UNIQUE_PTR(D3D12Renderer)

NAMESPACE_END(Gfx)