#pragma once

#include "Public/IEngine.h"
#include "D3D11Context.h"
#include "D3D11Swapchain.h"
#include "D3D11Profile.h"

class D3D11Engine : public IEngine
{
public:
	static D3D11Engine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<D3D11Engine, std::function<void(D3D11Engine *)>>
				(new D3D11Engine(), [](D3D11Engine *pEngine) { SafeDelete(pEngine) });
		}
		return *s_Instance;
	}

	inline const D3D11Device &GetDevice() const
	{
		assert(m_Device.IsValid());
		return m_Device;
	}

	inline const D3D11Context &GetIMContext() const
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

	inline void SetRenderTargetView(const D3D11RenderTargetView &renderTarget, uint32_t slot = 0U)
	{
		GetThreadContext().SetRenderTargetView(renderTarget, slot);
	}

	inline void SetDepthStencilView(const D3D11DepthStencilView &depthStencilView)
	{
		GetThreadContext().SetDepthStencilView(depthStencilView);
	}

	inline void SetShaderResourceView(const D3D11ShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
		GetThreadContext().SetShaderResourceView(shaderResourceView, slot, targetShader);
	}

	inline void SetUnorderedAccessView(const D3D11UnorderedAccessView &unorderedAccessView, uint32_t slot, eRShaderType targetShader = eComputeShader)
	{
		GetThreadContext().SetUnorderedAccessView(unorderedAccessView, slot, targetShader);
	}

	inline void SetVertexBuffer(const D3D11Buffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U) 
	{
		GetThreadContext().SetVertexBuffer(vertexBuffer, stride, offset, slot);
	}

	inline void SetIndexBuffer(const D3D11Buffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
		GetThreadContext().SetIndexBuffer(indexBuffer, fmt, offset);
	}

	inline void SetSamplerState(const D3D11SamplerState &samplerState, uint32_t slot, eRShaderType targetShader)
	{
		GetThreadContext().SetSamplerState(samplerState, slot, targetShader);
	}

	inline void SetRasterizerState(const D3D11RasterizerState &rasterizerState)
	{
		GetThreadContext().SetRasterizerState(rasterizerState);
	}

	inline void SetDepthStencilState(const D3D11DepthStencilState &depthStencilState, uint32_t stencilRef)
	{
		GetThreadContext().SetDepthStencilState(depthStencilState, stencilRef);
	}

	inline void SetBlendState(const D3D11BlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF)
	{
		GetThreadContext().SetBlendState(blendState, blendFactor, mask);
	}

	inline void SetInputLayout(const D3D11InputLayout &inputLayout)
	{
		GetThreadContext().SetInputLayout(inputLayout);
	}

	inline void SetVertexShader(const D3D11VertexShader &vertexShader)
	{
		GetThreadContext().SetVertexShader(vertexShader);
	}

	inline void SetHullShader(const D3D11HullShader &hullShader)
	{
		GetThreadContext().SetHullShader(hullShader);
	}

	inline void SetDomainShader(const D3D11DomainShader &domainShader)
	{
		GetThreadContext().SetDomainShader(domainShader);
	}

	inline void SetPixelShader(const D3D11PixelShader &pixelShader)
	{
		GetThreadContext().SetPixelShader(pixelShader);
	}

	inline void SetGeometryShader(const D3D11GeometryShader &geometryShader)
	{
		GetThreadContext().SetGeometryShader(geometryShader);
	}

	inline void SetComputeShader(const D3D11ComputeShader &computeShader)
	{
		GetThreadContext().SetComputeShader(computeShader);
	}

	inline void SetUniformBuffer(const D3D11Buffer &constantBuffer, uint32_t slot, eRShaderType targetShader)
	{
		GetThreadContext().SetConstantBuffer(constantBuffer, slot, targetShader);
	}

	inline void SetViewport(const D3D11Viewport &viewport, uint32_t slot = 0U)
	{
		GetThreadContext().SetViewport(viewport, slot);
	}

	inline void SetScissorRect(const D3D11Rect &rect, uint32_t slot = 0U)
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

	inline void ClearRenderTargetView(D3D11RenderTargetView &renderTarget, const Vec4 &color = Color::DarkBlue)
	{
		GetThreadContext().ClearRenderTargetView(renderTarget, color);
	}

	inline void ClearDepthStencilView(D3D11DepthStencilView &depthStencil, uint32_t clearFlag = eClearDepthStencil, float depth = 1.0f, uint8_t stencil = 0U)
	{
		GetThreadContext().ClearDepthStencilView(depthStencil, clearFlag, depth, stencil);
	}

	inline void ResetDefaultRenderSurfaces(const Vec4 &clearClr = Color::DarkBlue)
	{
		D3D11Context &ctx = GetThreadContext();

		ctx.SetRenderTargetView(m_RenderTargetView, 0U);
		ctx.ClearRenderTargetView(m_RenderTargetView, clearClr);

		ctx.SetDepthStencilView(m_DepthStencilView);
		ctx.ClearDepthStencilView(m_DepthStencilView, eClearDepthStencil, 1.0f, 0U);
	}

	inline void ResetDefaultRenderTargetView(const Vec4 &clearClr = Color::DarkBlue)
	{
		D3D11Context &ctx = GetThreadContext();

		ctx.SetRenderTargetView(m_RenderTargetView, 0U);
		ctx.ClearRenderTargetView(m_RenderTargetView, clearClr);
	}

	inline void ResetDefaultDepthStencilView()
	{
		D3D11Context &ctx = GetThreadContext();

		ctx.SetDepthStencilView(m_DepthStencilView);
		ctx.ClearDepthStencilView(m_DepthStencilView, eClearDepthStencil, 1.0f, 0U);
	}

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen);

	void Resize(uint32_t width, uint32_t height);

	void RegisterRenderThread(std::thread::id workerThreadID);

	///void DrawInstanced();
	///void DrawAuto();

	///void DrawTextInPos(const char *pTextContent, uint32_t left = 10U, uint32_t top = 10U, uint32_t fontSize = 1U);
	inline void Finalize() {}
protected:
	D3D11Engine() = default;
	~D3D11Engine() = default;

	void RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height);

	inline D3D11Context &GetThreadContext()
	{
		return m_IMContext;
		///return m_ContextsMap[std::this_thread::get_id()];
	}
private:
	static std::unique_ptr<D3D11Engine, std::function<void(D3D11Engine *)>> s_Instance;

	D3D11Device m_Device;
	D3D11Context m_IMContext;
	D3D11SwapChain m_SwapChain;
	D3D11RenderTargetView m_RenderTargetView;
	D3D11DepthStencilView m_DepthStencilView;

	std::unordered_map<std::thread::id, D3D11Context> m_ContextsMap;

	D3D11ContextState m_IMContextState;

	bool m_Inited = false;
};
