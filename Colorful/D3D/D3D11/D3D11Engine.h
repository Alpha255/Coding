#pragma once

#include "Colorful/D3D/D3D11/D3D11Buffer.h"
#include "Colorful/D3D/D3D11/D3D11Swapchain.h"
#include "Colorful/D3D/D3D11/D3D11Enum.h"
#include "Colorful/D3D/D3D11/D3D11Shader.h"
#include "Colorful/D3D/D3D11/D3D11Texture.h"

class D3D11Engine : public GfxEngine, public Singleton<D3D11Engine>
{
public:
	void initialize(uint64_t windowHandle, const Configurations& config) override final;
	void finalize() override final;

	static void logError(uint32_t result);

	void handleWindowResize(uint32_t width, uint32_t height) override final
	{
		if (m_Swapchain.isValid())
		{
			m_Swapchain.resize(width, height);
		}
	}

	void present() override final
	{
	}

public:
	GfxShaderPtr createVertexShader(const std::string& shaderName) override final
	{
		auto shader = std::make_shared<D3D11Shader>(m_Device, eVertexShader, shaderName);
		return std::static_pointer_cast<GfxShader>(shader);
	}
	GfxShaderPtr createFragmentShader(const std::string& shaderName) override final
	{
		auto shader = std::make_shared<D3D11Shader>(m_Device, eFragmentShader, shaderName);
		return std::static_pointer_cast<GfxShader>(shader);
	}

	GfxGpuBufferPtr createIndexBuffer(eRBufferUsage usage, size_t size, const void* data) override final
	{
		auto buffer = D3D11BufferPool::instance()->allocIndexBuffer(usage, size, data);
		return static_cast<GfxGpuBufferPtr>(buffer);
	}
	GfxGpuBufferPtr createVertexBuffer(eRBufferUsage usage, size_t size, const void* data) override final
	{
		auto buffer = D3D11BufferPool::instance()->allocVertexBuffer(usage, size, data);
		return static_cast<GfxGpuBufferPtr>(buffer);
	}
	GfxGpuBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		auto buffer = D3D11BufferPool::instance()->allocUniformBuffer(size, data);
		return static_cast<GfxGpuBufferPtr>(buffer);
	}

	GfxTexturePtr createTexture(const std::string& name) override final
	{
		auto texture = std::make_shared<D3DTexture>(name, m_Device);
		return static_cast<GfxTexturePtr>(texture);
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
public:
protected:
private:
	D3D11Device m_Device;
	D3D11Context m_IMContext;
	D3D11Swapchain m_Swapchain;
};