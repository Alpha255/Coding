#pragma once

#include "D3DView.h"
#include "D3DMath.h"
#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DState.h"
#include "D3DContext.h"
#include "D3DGeometry.h"

#include <unordered_map>
#include <thread>

class D3DEngine : public NoneCopyable
{
public:
	static D3DEngine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>>
				(new D3DEngine(), [](D3DEngine *pEngine) { SafeDelete(pEngine) });
		}
		return *s_Instance;
	}

	inline const D3DDevice &GetDevice() const
	{
		assert(m_Device.IsValid());
		return m_Device;
	}

	inline const D3DContext &GetIMContext() const
	{
		assert(m_IMContext.IsValid());
		return m_IMContext;
	}

	inline void Flush()
	{
		m_SwapChain.Flush();
	}

	inline void SetVSync(bool bVSync)
	{
		m_SwapChain.SetVSync(bVSync);
	}

	inline void SetFullScreen(bool bFullScreen)
	{
		m_SwapChain.SetFullScreen(bFullScreen);
	}

	inline void ForceCommitState()
	{
		GetThreadContext().ForceCommitState();
	}

	inline void SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot = 0U)
	{
		GetThreadContext().SetRenderTargetView(renderTarget, slot);
	}

	inline void SetDepthStencilView(const D3DDepthStencilView &depthStencilView)
	{
		GetThreadContext().SetDepthStencilView(depthStencilView);
	}

	inline void SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader)
	{
		GetThreadContext().SetShaderResourceView(shaderResourceView, slot, targetShader);
	}

	inline void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader = D3DShader::eComputeShader)
	{
		GetThreadContext().SetUnorderedAccessView(unorderedAccessView, slot, targetShader);
	}

	inline void SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U) 
	{
		GetThreadContext().SetVertexBuffer(vertexBuffer, stride, offset, slot);
	}

	inline void SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
		GetThreadContext().SetIndexBuffer(indexBuffer, fmt, offset);
	}

	inline void SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader)
	{
		GetThreadContext().SetSamplerState(samplerState, slot, targetShader);
	}

	inline void SetRasterizerState(const D3DRasterizerState &rasterizerState)
	{
		GetThreadContext().SetRasterizerState(rasterizerState);
	}

	inline void SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef)
	{
		GetThreadContext().SetDepthStencilState(depthStencilState, stencilRef);
	}

	inline void SetBlendState(const D3DBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF)
	{
		GetThreadContext().SetBlendState(blendState, blendFactor, mask);
	}

	inline void SetInputLayout(const D3DInputLayout &inputLayout)
	{
		GetThreadContext().SetInputLayout(inputLayout);
	}

	inline void SetVertexShader(const D3DVertexShader &vertexShader)
	{
		GetThreadContext().SetVertexShader(vertexShader);
	}

	inline void SetHullShader(const D3DHullShader &hullShader)
	{
		GetThreadContext().SetHullShader(hullShader);
	}

	inline void SetDomainShader(const D3DDomainShader &domainShader)
	{
		GetThreadContext().SetDomainShader(domainShader);
	}

	inline void SetPixelShader(const D3DPixelShader &pixelShader)
	{
		GetThreadContext().SetPixelShader(pixelShader);
	}

	inline void SetGeometryShader(const D3DGeometryShader &geometryShader)
	{
		GetThreadContext().SetGeometryShader(geometryShader);
	}

	inline void SetComputeShader(const D3DComputeShader &computeShader)
	{
		GetThreadContext().SetComputeShader(computeShader);
	}

	inline void SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader)
	{
		GetThreadContext().SetConstantBuffer(constantBuffer, slot, targetShader);
	}

	inline void SetViewport(const D3DViewport &viewport, uint32_t slot = 0U)
	{
		GetThreadContext().SetViewport(viewport, slot);
	}

	inline void SetScissorRect(const D3DRect &rect, uint32_t slot = 0U)
	{
		GetThreadContext().SetScissorRect(rect, slot);
	}

	inline void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
	{
		GetThreadContext().Draw(vertexCount, startVertex, primitive);
	}

	inline void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
	{
		GetThreadContext().DrawIndexed(indexCount, startIndex, offset, primitive);
	}

	inline void ClearRenderTargetView(D3DRenderTargetView &renderTarget, const float *pColor = nullptr)
	{
		GetThreadContext().ClearRenderTargetView(renderTarget, pColor);
	}

	inline void ClearDepthStencilView(D3DDepthStencilView &depthStencil, uint32_t clearFlag = D3DDepthStencilView::eDepthStencil, float depth = 1.0f, uint8_t stencil = 0U)
	{
		GetThreadContext().ClearDepthStencilView(depthStencil, clearFlag, depth, stencil);
	}

	inline void ResetDefaultRenderSurfaces()
	{
		D3DContext &ctx = GetThreadContext();

		ctx.SetRenderTargetView(m_RenderTargetView, 0U);
		ctx.ClearRenderTargetView(m_RenderTargetView);

		ctx.SetDepthStencilView(m_DepthStencilView);
		ctx.ClearDepthStencilView(m_DepthStencilView, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
	}

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height);

	void RegisterRenderThread(std::thread::id workerThreadID);

	///void DrawInstanced();
	///void DrawAuto();

	///void DrawTextInPos(const char *pTextContent, uint32_t left = 10U, uint32_t top = 10U, uint32_t fontSize = 1U);

protected:
	D3DEngine() = default;
	~D3DEngine() = default;

	void RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height);

	inline D3DContext &GetThreadContext()
	{
		return m_ContextsMap[std::this_thread::get_id()];
	}
private:
	static std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>> s_Instance;

	D3DDevice m_Device;
	D3DContext m_IMContext;
	D3DSwapChain m_SwapChain;
	D3DRenderTargetView m_RenderTargetView;
	D3DDepthStencilView m_DepthStencilView;

	std::unordered_map<std::thread::id, D3DContext> m_ContextsMap;

	D3DContextState m_IMContextState;

	bool m_Inited = false;
};
