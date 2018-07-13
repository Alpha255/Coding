#pragma once

#include "D3DView.h"
#include "D3DMath.h"
#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DState.h"
#include "D3DContext.h"
#include "D3DGeometry.h"

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
		assert(m_ContextsInUse[0].IsValid());
		return m_ContextsInUse[0];
	}

	inline void ClearRenderTargetView(D3DRenderTargetView &renderTarget, const float *pColor = nullptr, uint32_t ctxIndex = 0U) const
	{
		assert(renderTarget.IsValid() && m_ContextsInUse[ctxIndex].IsValid());

		pColor = (nullptr == pColor ? reinterpret_cast<const float*>(&Color::DarkBlue) : pColor);

		m_ContextsInUse[ctxIndex]->ClearRenderTargetView(renderTarget.Get(), pColor);
	}

	inline void ClearDepthStencilView(D3DDepthStencilView &depthStencil, uint32_t clearFlag, float depth, uint8_t stencil, uint32_t ctxIndex = 0U) const
	{
		assert(depthStencil.IsValid() && m_ContextsInUse[ctxIndex].IsValid());

		m_ContextsInUse[ctxIndex]->ClearDepthStencilView(depthStencil.Get(), clearFlag, depth, stencil);
	}

	inline void ResetRenderSurfaces()
	{
		SetRenderTargetView(m_RenderTargetView, 0U);
		SetDepthStencilView(m_DepthStencilView);
	}

	inline void ClearRenderSurfaces()
	{
		ClearRenderTargetView(m_RenderTargetView);

		ClearDepthStencilView(m_DepthStencilView, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);
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
		///m_Pipeline.CommitState(m_IMContext);
	}

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height);

	void SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot = 0U, uint32_t ctxIndex = 0U);
	void SetDepthStencilView(const D3DDepthStencilView &depthStencilView, uint32_t ctxIndex = 0U);
	void SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader, uint32_t ctxIndex = 0U);
	void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, uint32_t ctxIndex = 0U);

	void SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U, uint32_t ctxIndex = 0U);
	void SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset, uint32_t ctxIndex = 0U);

	void SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader, uint32_t ctxIndex = 0U);
	void SetRasterizerState(const D3DRasterizerState &rasterizerState, uint32_t ctxIndex = 0U);
	void SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef, uint32_t ctxIndex = 0U);
	void SetBlendState(const D3DBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF, uint32_t ctxIndex = 0U);

	void SetInputLayout(const D3DInputLayout &inputLayout, uint32_t ctxIndex = 0U);
	void SetVertexShader(const D3DVertexShader &vertexShader, uint32_t ctxIndex = 0U);
	void SetHullShader(const D3DHullShader &hullShader, uint32_t ctxIndex = 0U);
	void SetDomainShader(const D3DDomainShader &domainShader, uint32_t ctxIndex = 0U);
	void SetPixelShader(const D3DPixelShader &pixelShader, uint32_t ctxIndex = 0U);
	void SetGeometryShader(const D3DGeometryShader &geometryShader, uint32_t ctxIndex = 0U);
	void SetComputeShader(const D3DComputeShader &computeShader, uint32_t ctxIndex = 0U);

	void SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader, uint32_t ctxIndex = 0U);

	void SetViewport(const D3DViewport &viewport, uint32_t slot = 0U, uint32_t ctxIndex = 0U);
	void SetScissorRect(const D3DRect &rect, uint32_t slot = 0U, uint32_t ctxIndex = 0U);

	void CreateDeferredContexts(uint32_t count);

	void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive, bool bClearState = true, uint32_t ctxIndex = 0U);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive, bool bClearState = true, uint32_t ctxIndex = 0U);
	void DrawInstanced();
	void DrawAuto();

	void DrawTextInPos(const char *pTextContent, uint32_t left = 10U, uint32_t top = 10U, uint32_t fontSize = 1U);

protected:
	D3DEngine() = default;
	~D3DEngine() = default;

	void RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height);
private:
	static std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>> s_Instance;

	D3DDevice m_Device;
	D3DSwapChain m_SwapChain;
	D3DRenderTargetView m_RenderTargetView;
	D3DDepthStencilView m_DepthStencilView;

	std::vector<D3DContext> m_ContextsInUse;

	bool m_Inited = false;
};
