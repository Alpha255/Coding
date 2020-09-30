#pragma once

#include "Colorful/Public/GfxRenderer.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(SoftwareRenderer)
class SoftwareRenderer final : public GfxRenderer
{
public:
	void createDevice() override;
	void createSwapchain(uint64_t instance, uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) override;

	void toggleFullScreen(bool8_t fullscreen) override;
	void toggleVSync(bool8_t vSync) override;

	void finalize() override;
protected:
private:
};

NAMESPACE_END(Gfx)