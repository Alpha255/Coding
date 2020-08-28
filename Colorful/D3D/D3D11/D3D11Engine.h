#pragma once

#include "Colorful/D3D/D3D11/D3D11Buffer.h"
#include "Colorful/D3D/D3D11/D3D11Swapchain.h"
#include "Colorful/D3D/D3D11/D3D11Enum.h"
#include "Colorful/D3D/D3D11/D3D11Shader.h"
#include "Colorful/D3D/D3D11/D3D11View.h"

class D3D11Engine : public GfxEngine
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
		m_Swapchain.present();
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
		auto texture = std::make_shared<D3D11ShaderResourceView>(m_Device, name);
		return static_cast<GfxTexturePtr>(texture);
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
		auto texture = std::make_shared<D3D11ShaderResourceView>(
			m_Device,
			type,
			format,
			width,
			height,
			depth,
			mipLevels,
			arrayLayers,
			data,
			dataSize);
		return static_cast<GfxTexturePtr>(texture);
	}

	GfxSamplerPtr createSampler(const GfxSamplerDesc& desc) override final
	{
		auto sampler = std::make_shared<D3D11SamplerState>(m_Device, desc);
		return std::static_pointer_cast<GfxSampler>(sampler);
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

	void bindGfxPipelineState(const GfxPipelineState* state) override final
	{
		assert(state);
		m_CurGfxPipeline = getOrCreateGfxPipeline(state);
	}

	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset) override final
	{
		m_IMContext.setGraphicsPipeline(m_CurGfxPipeline);

		if (instanceCount > 1u)
		{
			m_IMContext->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, 0u); /// ????? 
		}
		else if (instanceCount == 1u)
		{
			m_IMContext->DrawIndexed(indexCount, firstIndex, vertexOffset);
		}
		else
		{
			assert(0);
		}
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
	D3D11GraphicsPipelinePtr m_CurGfxPipeline = nullptr;
	D3D11GraphicsPipelinePtr getOrCreateGfxPipeline(const GfxPipelineState* state);
	void prepareForDraw();
private:
	D3D11Device m_Device;
	D3D11Context m_IMContext;
	D3D11Swapchain m_Swapchain;

	std::vector<std::pair<const GfxPipelineState*, D3D11GraphicsPipelinePtr>> m_GfxPipelines;
};