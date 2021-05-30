#pragma once

#include "Colorful/Gfx/GfxRenderer.h"
#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

class D3D11Renderer final : public GfxRenderer
{
public:
	void initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) override final;

	void toggleFullScreen(bool8_t fullscreen) override final;
	void toggleVSync(bool8_t VSync) override final;

	GfxDevice* device() override final
	{
		return m_Device.get();
	}

	void finalize() override;
protected:
private:
	D3D11DevicePtr m_Device;
};
DECLARE_UNIQUE_PTR(D3D11Renderer)

NAMESPACE_END(Gfx)