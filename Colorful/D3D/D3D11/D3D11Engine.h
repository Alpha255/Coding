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

	GfxGpuBufferPtr createIndexBuffer(eRBufferUsage usage, size_t size, const void* data) override final
	{
		return nullptr;
	}
	GfxGpuBufferPtr createVertexBuffer(eRBufferUsage usage, size_t size, const void* data) override final
	{
		return nullptr;
	}
	GfxGpuBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		return nullptr;
	}

	GfxTexturePtr createTexture(const std::string& fileName) override final
	{
		return nullptr;
	}
	GfxTexturePtr createTexture(
		eRTextureType type,
		eRFormat format,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const void* data,
		size_t dataSize) override final
	{
		return nullptr;
	}

	GfxSamplerPtr createSampler(const GfxSamplerDesc& desc) override final
	{
		return nullptr;
	}

	GfxRenderSurfacePtr createDepthStencilView(uint32_t width, uint32_t height, eRFormat format) override final
	{
		return nullptr;
	}
	GfxRenderSurfacePtr createRenderTargetView() override final
	{
		return nullptr;
	}

	GfxRenderPassPtr createRenderPass(GfxFrameBufferDesc& desc) override final
	{
		return nullptr;
	}
public:
protected:
private:
	D3D11Device m_Device;
	D3D11Context m_IMContext;
	d3d11Swapchain m_Swapchain;
};