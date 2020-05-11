#pragma once

#include "D3D11Device.h"
#include "D3D11Swapchain.h"

class d3d11Engine : public GfxEngine, public Singleton<d3d11Engine>
{
	singletonDeclare(d3d11Engine);
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

	inline const d3d11Device &getDevice() const
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
	class enumTranslator 
	{
	public:
		static D3D11_FILL_MODE toPolygonMode(eRPolygonMode mode);
		static D3D11_CULL_MODE toCullMode(eRCullMode mode);
		static D3D11_BLEND_OP toBlendOp(eRBlendOp op);
		static D3D11_BLEND toBlendFactor(eRBlendFactor factor);
		static uint8_t toColorComponentFlags(uint32_t colorMask);
		static D3D11_COMPARISON_FUNC toCompareOp(eRCompareOp op);
		static D3D11_STENCIL_OP toStencilOp(eRStencilOp op);
		static DXGI_FORMAT toFormat(eRFormat format);
	};
protected:
private:
	d3d11Device m_Device;
	d3d11Context m_IMContext;
	d3d11Swapchain m_Swapchain;
};