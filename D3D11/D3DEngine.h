#pragma once

#include "D3DView.h"
#include "D3DMath.h"
#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DState.h"

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

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height);

	void SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot);
	void SetDepthStencilView(const D3DDepthStencilView &depthStencilView);
	void SetShaderResource(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader);

	void SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t index);
	void SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset);

	void SetSamplerStates(const D3DSamplerState &samplerStates, uint32_t startSlot, D3DShader::eShaderType targetShader);
	void SetRasterizerState(const D3DRasterizerState &rasterizerState);
	void SetDepthStencilState(const D3DDepthStencilState &depthStencilState, uint32_t stencilRef);
	void SetBlendState(const D3DBlendState &blendState, const float *pFactor = nullptr, uint32_t mask = 0xFFFFFFFF);

	void SetInputLayout(const D3DInputLayout &inputLayout);
	void SetVertexShader(const D3DVertexShader &vertexShader);
	void SetHullShader(const D3DHullShader &hullShader);
	void SetDomainShader(const D3DDomainShader &domainShader);
	void SetPixelShader(const D3DPixelShader &pixelShader);
	void SetGeometryShader(const D3DGeometryShader &geometryShader);
	void SetComputeShader(const D3DComputeShader &computeShader);

	void SetConstantBuffer(const D3DBuffer &constantsBuf, uint32_t slot, D3DShader::eShaderType targetShader);

	void SetViewport(const D3DViewport &viewport, uint32_t index);
	void SetScissorRect(const D3DRect &rect, uint32_t index);
protected:
	enum eResourceLimits
	{
		eMaxViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eMaxScissorRect = eMaxViewports,
		eMaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
		eMaxConstantBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT,
		eMaxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		eMaxShaderResource = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
		eMaxUnorderedAccessView = D3D11_PS_CS_UAV_REGISTER_COUNT,
		eMaxVertexStream = 1  /// Custom
	};

	enum eDirtyFlag
	{
		eDInputLayout,
		eDVerteBuffer,
		eDIndexBuffer,
		eDConstantBuffer,
		
		eDVertexShader,
		eDHullShader,
		eDDomainShader,
		eDPixelShader,
		eDGeometryShader,
		eDComputeShader,
		
		eDRasterizerState,
		eDSamplerState,
		eDBlendState,
		eDDepthStencilState,
		
		eDRenderTargetView,
		eDDepthStencilView,
		eDShaderResourceView,
		eDUnorderedAccessView,
		
		eDRect,
		eDViewport,

		eDPrimitiveTopolgy,

		eDirtyFlagCount
	};

	struct D3DPipeline
	{
		D3DInputLayout           InputLayout;
		D3DVertexBuffer          VertexBuffers[eMaxVertexStream];
		D3DIndexBuffer           IndexBuffer;
		D3DBuffer                ConstantBuffers[eMaxConstantBuffers];

		D3DVertexShader          VertexShader;
		D3DHullShader            HullShader;
		D3DDomainShader          DomainShader;
		D3DPixelShader           PixelShader;
		D3DGeometryShader        GeometryShader;
		D3DComputeShader         ComputeShader;

		D3DRasterizerState       RasterizerState;
		D3DSamplerState          SamplerState[eMaxSamplers];
		D3DBlendState            BlendState;
		D3DDepthStencilState     DepthStencilState;

		D3DRenderTargetView      RenderTargetView[eMaxRenderTargets];
		D3DDepthStencilView      DepthStencilView;
		D3DShaderResourceView    ShaderResourceView[eMaxShaderResource];
		D3DUnorderedAccessView   UnorderedAccessView[eMaxUnorderedAccessView];

		D3DViewport              Viewports[eMaxViewports];
		D3DRect                  ScissorRects[eMaxScissorRect];

		uint32_t                 PrimitiveTopology;
	};

	D3DEngine() = default;
	~D3DEngine() = default;

	void RecreateRenderTargetDepthStencil(uint32_t width, uint32_t height);
private:
	static std::unique_ptr<D3DEngine, std::function<void(D3DEngine *)>> s_Instance;

	D3DDevice m_Device;
	D3DContext m_IMContext;
	D3DSwapChain m_SwapChain;
	D3DRenderTargetView m_RenderTarget;
	D3DDepthStencilView m_DepthStencil;

	D3DPipeline m_Pipeline;
	bool m_DirtyFlags[eDirtyFlagCount] = {};
};
