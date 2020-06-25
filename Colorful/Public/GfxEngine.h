#pragma once

#include "Colorful/Public/ImGui/ImGui.h"
#include "Colorful/Public/GfxModel.h"
#include "Colorful/Public/GfxFactory.h"
#include "Colorful/Public/GfxMaterial.h"

namespace Gear
{
	struct Configurations;
}

struct GfxAdapter
{
	std::string DeviceName;
	uint32_t VendorID = 0u;
	uint32_t DeviceID = 0u;                      

	static std::string vendorName(uint32_t verdorID)
	{
		std::string name("Unknown");
		switch (verdorID)
		{
		case 0x1002: 
			name = std::string("AMD");
			break;
		case 0x1010: 
			name = std::string("ImgTec");
			break;
		case 0x10DE: 
			name = std::string("NVIDIA");
			break;
		case 0x13B5: 
			name = std::string("ARM");
			break;
		case 0x5143: 
			name = std::string("Qualcomm");
			break;
		case 0x8086: 
			name = std::string("Intel");
			break;
		}

		return name;
	}
};

class GfxDevice
{
protected:
	GfxAdapter m_Adapter;
};

class GfxGeometryBuffer
{
};

class GfxGpuMarker
{
};

class GfxGpuTimer
{
};

class GfxEngine
{
public:
	virtual ~GfxEngine() = default;

	using RenderFrameCallback = std::function<void(void)>;

	void registerRenderFrameCallback(RenderFrameCallback callback)
	{
		assert(!m_RenderFrameCallback);
		m_RenderFrameCallback = callback;
	}

	void renderFrame()
	{
		acquireNextFrame();

		m_ImGuiRenderer->begin();

		if (m_RenderFrameCallback)
		{
			m_RenderFrameCallback();
		}

		m_ImGuiRenderer->end();

		present();
	}

	void processMessage(const struct WindowMessage& message, uint32_t width, uint32_t height)
	{
		m_ImGuiRenderer->processMessage(message, width, height);
	}

	virtual void initialize(uint64_t windowHandle, const Gear::Configurations& config) = 0;
	virtual void finalize() = 0;
	virtual void present() = 0;
	virtual void acquireNextFrame() {};
	virtual void handleWindowResize(uint32_t width, uint32_t height) = 0;

	virtual bool isFocusOnUI() const 
	{
		return m_ImGuiRenderer->isFocus();
	}

	virtual GfxShaderPtr createVertexShader(const std::string& shaderName) = 0;
	virtual GfxShaderPtr createFragmentShader(const std::string& shaderName) = 0;

	virtual GfxGpuBufferPtr createIndexBuffer(eRBufferUsage usage, size_t size, const void* data) = 0;
	virtual GfxGpuBufferPtr createVertexBuffer(eRBufferUsage usage, size_t size, const void* data) = 0;
	virtual GfxGpuBufferPtr createUniformBuffer(size_t size, const void* data) = 0;

	virtual GfxTexturePtr createTexture(const std::string& fileName) = 0;
	virtual GfxTexturePtr createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void* data,
		size_t dataSize) = 0;

	virtual GfxSamplerPtr createSampler(const GfxSamplerDesc& desc) = 0;

	virtual GfxRenderSurfacePtr createDepthStencil(uint32_t width, uint32_t height, eRFormat format) = 0;
	virtual GfxRenderSurfacePtr createRenderTarget() = 0;

	virtual GfxRenderPassPtr createRenderPass(GfxFrameBufferDesc& desc) = 0;

	virtual GfxFrameBufferPtr backBuffer() = 0;
	virtual void bindGfxPipelineState(const GfxPipelineState* state) = 0;

	virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset) = 0;
	virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex) = 0;

	virtual void beginDebugMarker(const char8_t* name, Vec4 color) = 0;
	virtual void insertDebugMarker(const char8_t* name, Vec4 color) = 0;
	virtual void endDebugMarker() = 0;
protected:
	RenderFrameCallback m_RenderFrameCallback;
	ImGuiRendererPtr m_ImGuiRenderer = nullptr;
private:
};
using GfxEnginePtr = std::unique_ptr<GfxEngine>;
extern GfxEnginePtr g_GfxEngine;

class GfxDebugMarker
{
public:
	GfxDebugMarker(const char8_t* name, Vec4 color)
	{
		begin(name, color);
	}

	~GfxDebugMarker()
	{
		if (!m_End)
		{
			end();
		}
	}

	void begin(const char8_t* name, Vec4 color)
	{
		g_GfxEngine->beginDebugMarker(name, color);
	}

	void end()
	{
		g_GfxEngine->endDebugMarker();
		m_End = true;
	}

	static void insert(const char8_t* name, Vec4 color)
	{
		g_GfxEngine->insertDebugMarker(name, color);
	}
protected:
private:
	bool m_End = false;
};

#define GfxScopeGpuMarker(Name, Color) GfxDebugMarker DebugMarker_##Name(#Name, Color)
#define GfxInsertScopeGpuMarker(Name, Color) GfxDebugMarker::insert(#Name, Color)
