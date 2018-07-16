#pragma once

#include "D3DView.h"
#include "D3DMath.h"
#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DState.h"
#include "D3DContextState.h"
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

	inline void ClearRenderTargetView(D3DRenderTargetView &renderTarget, const float *pColor = nullptr) const
	{
		assert(renderTarget.IsValid());

		pColor = (nullptr == pColor ? reinterpret_cast<const float*>(&Color::DarkBlue) : pColor);

		m_IMContext->ClearRenderTargetView(renderTarget.Get(), pColor);
	}

	inline void ClearDepthStencilView(D3DDepthStencilView &depthStencil, uint32_t clearFlag, float depth, uint8_t stencil) const
	{
		assert(depthStencil.IsValid());

		m_IMContext->ClearDepthStencilView(depthStencil.Get(), clearFlag, depth, stencil);
	}

	inline void ResetDefaultRenderSurfaces()
	{
		SetRenderTargetView(m_RenderTargetView, 0U);
		ClearRenderTargetView(m_RenderTargetView);

		SetDepthStencilView(m_DepthStencilView);
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
		m_IMContextState.CommitState(m_IMContext);
	}

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height);

	void RegisterRenderThread(std::thread::id workerThreadID);

	void SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot = 0U);
	void SetDepthStencilView(const D3DDepthStencilView &depthStencilView);
	void SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader);
	void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader = D3DShader::eComputeShader);

	void SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U);
	void SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset);

	void SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader);
	void SetRasterizerState(const D3DRasterizerState &rasterizerState);
	void SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef);
	void SetBlendState(const D3DBlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF);

	void SetInputLayout(const D3DInputLayout &inputLayout);
	void SetVertexShader(const D3DVertexShader &vertexShader);
	void SetHullShader(const D3DHullShader &hullShader);
	void SetDomainShader(const D3DDomainShader &domainShader);
	void SetPixelShader(const D3DPixelShader &pixelShader);
	void SetGeometryShader(const D3DGeometryShader &geometryShader);
	void SetComputeShader(const D3DComputeShader &computeShader);

	void SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader);

	void SetViewport(const D3DViewport &viewport, uint32_t slot = 0U);
	void SetScissorRect(const D3DRect &rect, uint32_t slot = 0U);

	void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive);
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
	D3DContext m_IMContext;
	D3DSwapChain m_SwapChain;
	D3DRenderTargetView m_RenderTargetView;
	D3DDepthStencilView m_DepthStencilView;

	std::unordered_map<std::thread::id, D3DContext> m_ContextsMap;

	D3DContextState m_IMContextState;

	bool m_Inited = false;
};
