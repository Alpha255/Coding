#pragma once

#include "Colorful/D3D/D3D11/D3D11Device.h"
#include "Colorful/D3D/D3D11/D3D11Swapchain.h"
#include "Colorful/D3D/D3D11/D3D11Enum.h"

class D3D11Engine : public GfxEngine, public Singleton<D3D11Engine>
{
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

	inline const D3D11Device &getDevice() const
	{
		assert(m_Device.isValid());
		return m_Device;
	}

public:
	GfxShaderPtr createVertexShader(const std::string &) override final
	{
		return nullptr;
	}
	GfxShaderPtr createFragmentShader(const std::string &) override final
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
	GfxRenderSurfacePtr createRenderTarget() override final
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

	void bindGfxPipelineState(GfxPipelineState*) override final
	{
	}

	void drawIndexed(uint32_t, uint32_t, int32_t) override final
	{
	}
public:
protected:
private:
	D3D11Device m_Device;
	D3D11Context m_IMContext;
	d3d11Swapchain m_Swapchain;
};