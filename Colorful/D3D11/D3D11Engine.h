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

	inline D3D11Context &GetIMContext()
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

	inline void SetRenderTargetView(const D3D11RenderTargetView &renderTarget, uint32_t slot = 0U)
	{
		m_IMContext.SetRenderTargetView(renderTarget, slot);
	}

	inline void SetDepthStencilView(const D3D11DepthStencilView &depthStencilView)
	{
		m_IMContext.SetDepthStencilView(depthStencilView);
	}

	inline void SetShaderResourceView(const D3D11ShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader)
	{
		m_IMContext.SetShaderResourceView(shaderResourceView, slot, targetShader);
	}

	inline void SetUnorderedAccessView(const D3D11UnorderedAccessView &unorderedAccessView, uint32_t slot, eRShaderType targetShader = eComputeShader)
	{
		m_IMContext.SetUnorderedAccessView(unorderedAccessView, slot, targetShader);
	}

	inline void SetVertexBuffer(const D3D11Buffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U) 
	{
		m_IMContext.SetVertexBuffer(vertexBuffer, stride, offset, slot);
	}

	inline void SetIndexBuffer(const D3D11Buffer &indexBuffer, uint32_t fmt, uint32_t offset)
	{
		m_IMContext.SetIndexBuffer(indexBuffer, fmt, offset);
	}

	inline void SetSamplerState(const D3D11SamplerState &samplerState, uint32_t slot, eRShaderType targetShader)
	{
		m_IMContext.SetSamplerState(samplerState, slot, targetShader);
	}

	inline void SetRasterizerState(const D3D11RasterizerState &rasterizerState)
	{
		m_IMContext.SetRasterizerState(rasterizerState);
	}

	inline void SetDepthStencilState(const D3D11DepthStencilState &depthStencilState, uint32_t stencilRef)
	{
		m_IMContext.SetDepthStencilState(depthStencilState, stencilRef);
	}

	inline void SetBlendState(const D3D11BlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF)
	{
		m_IMContext.SetBlendState(blendState, blendFactor, mask);
	}

	inline void SetInputLayout(const D3D11InputLayout &inputLayout)
	{
		m_IMContext.SetInputLayout(inputLayout);
	}

	inline void SetVertexShader(const D3D11VertexShader &vertexShader)
	{
		m_IMContext.SetVertexShader(vertexShader);
	}

	inline void SetHullShader(const D3D11HullShader &hullShader)
	{
		m_IMContext.SetHullShader(hullShader);
	}

	inline void SetDomainShader(const D3D11DomainShader &domainShader)
	{
		m_IMContext.SetDomainShader(domainShader);
	}

	inline void SetPixelShader(const D3D11PixelShader &pixelShader)
	{
		m_IMContext.SetPixelShader(pixelShader);
	}

	inline void SetGeometryShader(const D3D11GeometryShader &geometryShader)
	{
		m_IMContext.SetGeometryShader(geometryShader);
	}

	inline void SetComputeShader(const D3D11ComputeShader &computeShader)
	{
		m_IMContext.SetComputeShader(computeShader);
	}

	inline void SetUniformBuffer(const D3D11Buffer &constantBuffer, uint32_t slot, eRShaderType targetShader)
	{
		m_IMContext.SetUniformBuffer(constantBuffer, slot, targetShader);
	}

	inline void SetViewport(const D3D11Viewport &viewport, uint32_t slot = 0U)
	{
		m_IMContext.SetViewport(viewport, slot);
	}

	inline void SetScissorRect(const D3D11Rect &rect, uint32_t slot = 0U)
	{
		m_IMContext.SetScissorRect(rect, slot);
	}

	inline void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive)
	{
		m_IMContext.Draw(vertexCount, startVertex, primitive);
	}

	inline void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive)
	{
		m_IMContext.DrawIndexed(indexCount, startIndex, offset, primitive);
	}

	inline void ClearRenderTargetView(D3D11RenderTargetView &renderTarget, const Vec4 &color = Color::DarkBlue)
	{
		m_IMContext.ClearRenderTargetView(renderTarget, color);
	}

	inline void ClearDepthStencilView(D3D11DepthStencilView &depthStencil, uint32_t clearFlag = eClearDepthStencil, float depth = 1.0f, uint8_t stencil = 0U)
	{
		m_IMContext.ClearDepthStencilView(depthStencil, clearFlag, depth, stencil);
	}

	inline void ResetDefaultRenderSurfaces(const Vec4 &clearClr = Color::DarkBlue, D3D11Context *pContext = nullptr, bool bClear = true)
	{
		D3D11Context *pCtx = nullptr;
		if (pContext && pContext->IsValid())
		{
			pCtx = pContext;
		}
		else
		{
			pCtx = &m_IMContext;
		}

		pCtx->SetRenderTargetView(m_RenderTargetView, 0U);
		if (bClear)
		{
			pCtx->ClearRenderTargetView(m_RenderTargetView, clearClr);
		}

		pCtx->SetDepthStencilView(m_DepthStencilView);
		if (bClear)
		{
			pCtx->ClearDepthStencilView(m_DepthStencilView, eClearDepthStencil, 1.0f, 0U);
		}
	}

	inline void ClearDefaultRenderSurface(const Vec4 &clearClr = Color::DarkBlue)
	{
		m_IMContext.ClearRenderTargetView(m_RenderTargetView, clearClr);
		m_IMContext.ClearDepthStencilView(m_DepthStencilView, eClearDepthStencil, 1.0f, 0U);
	}

	inline void ResetDefaultRenderTargetView(const Vec4 &clearClr = Color::DarkBlue, bool bClear = true)
	{
		m_IMContext.SetRenderTargetView(m_RenderTargetView, 0U);
		if (bClear)
		{
			m_IMContext.ClearRenderTargetView(m_RenderTargetView, clearClr);
		}
	}

	inline void ResetDefaultDepthStencilView(bool bClear = true)
	{
		m_IMContext.SetDepthStencilView(m_DepthStencilView);
		if (bClear)
		{
			m_IMContext.ClearDepthStencilView(m_DepthStencilView, eClearDepthStencil, 1.0f, 0U);
		}
	}

	inline void FinishCommandList(bool bRestore, D3D11CommandList &commandList)
	{
		m_IMContext.FinishCommandList(bRestore, commandList);
	}

	inline void ExecuteCommandList(bool bRestore, D3D11CommandList &commandList)
	{
		m_IMContext.ExecuteCommandList(bRestore, commandList);
	}

	void Initialize(::HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen);

	void Resize(uint32_t width, uint32_t height);

	///void DrawInstanced();
	///void DrawAuto();

	///void DrawTextInPos(const char *pTextContent, uint32_t left = 10U, uint32_t top = 10U, uint32_t fontSize = 1U);
	inline void Finalize() {}
protected:
	D3D11Engine() = default;
	~D3D11Engine() = default;

	void RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height);
private:
	static std::unique_ptr<D3D11Engine, std::function<void(D3D11Engine *)>> s_Instance;

	D3D11Device m_Device;
	D3D11Context m_IMContext;
	D3D11SwapChain m_SwapChain;
	D3D11RenderTargetView m_RenderTargetView;
	D3D11DepthStencilView m_DepthStencilView;

	bool m_Inited = false;
};


#include "D3D11Device.h"

class d3d11Engine : public rEngine, public gear::singleton<d3d11Engine>
{
	singletonDeclare(d3d11Engine);
public:
	void initialize(::HWND, const gear::appConfig &config) override final;
	void finalize() override final {}

	void logError(uint32_t result) const override final;

	inline rDevicePtr getDevice() const override final
	{
		assert(m_Device);
		return m_Device;
	}

	inline rContextPtr getIMContext() const override final
	{
		assert(m_IMContext);
		return m_IMContext;
	}
protected:
private:
	d3d11DevicePtr m_Device = nullptr;
	d3d11ContextPtr m_IMContext = nullptr;
};