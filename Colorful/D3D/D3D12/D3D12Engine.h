#pragma once

#include "D3D12Device.h"
#include "D3D12Swapchain.h"

class D3D12Engine : public GfxEngine
{
	singletonDeclare(D3D12Engine);
public:
	void initialize(uint64_t windowHandle, const Configurations &config) override final;
	void finalize() override final;

	static void logError(uint32_t result);

	void handleWindowResize(uint32_t, uint32_t) override final
	{
	}

	void present() override final
	{
	}

	inline void handleWindowResize(uint32_t width, uint32_t height, const Configurations &)
	{
		if (m_Swapchain.isValid())
		{
			m_Swapchain.resize(width, height);
		}
	}

	GfxShaderPtr createVertexShader(const std::string& ) override final
	{
		return nullptr;
	}
	GfxShaderPtr createFragmentShader(const std::string& ) override final
	{
		return nullptr;
	}

	GfxGpuBufferPtr createIndexBuffer(eRBufferUsage, size_t, const void*) override final
	{
		return nullptr;
	}
	GfxGpuBufferPtr createVertexBuffer(eRBufferUsage, size_t, const void*) override final
	{
		return nullptr;
	}
	GfxGpuBufferPtr createUniformBuffer(size_t, const void*) override final
	{
		return nullptr;
	}

	GfxTexturePtr createTexture(const std::string&) override final
	{
		return nullptr;
	}
	GfxTexturePtr createTexture(
		eRTextureType,
		eRFormat,
		uint32_t,
		uint32_t,
		uint32_t,
		uint32_t,
		uint32_t,
		const void*,
		size_t) override final
	{
		return nullptr;
	}

	GfxSamplerPtr createSampler(const GfxSamplerDesc&) override final
	{
		return nullptr;
	}

	GfxRenderSurfacePtr createDepthStencil(uint32_t, uint32_t, eRFormat) override final
	{
		return nullptr;
	}
	GfxRenderSurfacePtr createRenderTarget(uint32_t, uint32_t, eRFormat) override final
	{
		return nullptr;
	}

	GfxRenderPassPtr createRenderPass(GfxFrameBufferDesc&) override final
	{
		return nullptr;
	}

	GfxFrameBufferPtr backBuffer() override final
	{
		return nullptr;
	}

	void bindGfxPipelineState(const GfxPipelineState*) override final
	{
	}

	void drawIndexed(uint32_t, uint32_t, uint32_t, int32_t) override final
	{
	}

	void draw(uint32_t, uint32_t, uint32_t) override final
	{
	}

	void beginDebugMarker(const char8_t*, Vec4) override final
	{
	}

	void insertDebugMarker(const char8_t*, Vec4) override final
	{
	}

	void endDebugMarker() override final
	{
	}
protected:
private:
	D3D12Device m_Device;
	d3d12Swapchain m_Swapchain;
};