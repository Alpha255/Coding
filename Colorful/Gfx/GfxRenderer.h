#pragma once

#include "Colorful/Gfx/GfxDevice.h"

#define CREATE_RENDERER_FUNC_NAME "createRenderer"

NAMESPACE_START(Gfx)

enum class ERenderer : uint8_t
{
	Null,
	D3D11,
	D3D12,
	Vulkan,
	Software
};

class GfxRenderer : public Gear::NoneCopyable
{
public:
	GfxRenderer() = default;
	virtual ~GfxRenderer() = default;

	virtual void initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) = 0;

	virtual void toggleFullScreen(bool8_t fullscreen) = 0;
	virtual void toggleVSync(bool8_t VSync) = 0;

	virtual GfxDevice* device() = 0;

	virtual void finalize() {}
protected:
private:
};

typedef void(*CreateRendererFunc)(GfxRendererPtr&);
extern GfxRendererPtr GRenderer;

NAMESPACE_END(Gfx)