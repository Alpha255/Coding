#pragma once

#include "D3DView.h"
#include "D3DMath.h"
#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DState.h"
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

	inline void BindMesh(const Geometry::Mesh &mesh)
	{
		assert(m_Inited && mesh.IsCreated());

		SetInputLayout(mesh.VertexLayout);
		SetVertexBuffer(mesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
		SetIndexBuffer(mesh.IndexBuffer, eR32_UInt, 0U);
	}

	void DrawSDKMesh(const Geometry::SDKMesh &sdkMesh, uint32_t meshIndex = 0U);

	void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);

	void Resize(uint32_t width, uint32_t height);

	void SetRenderTargetView(const D3DRenderTargetView &renderTarget, uint32_t slot = 0U);
	void SetDepthStencilView(const D3DDepthStencilView &depthStencilView);
	void SetShaderResourceView(const D3DShaderResourceView &shaderResourceView, uint32_t slot, D3DShader::eShaderType targetShader);
	void SetUnorderedAccessView(const D3DUnorderedAccessView &unorderedAccessView, uint32_t slot, D3DShader::eShaderType targetShader = D3DShader::eComputeShader);

	void SetVertexBuffer(const D3DBuffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U);
	void SetIndexBuffer(const D3DBuffer &indexBuffer, uint32_t fmt, uint32_t offset);

	void SetSamplerState(const D3DSamplerState &samplerState, uint32_t slot, D3DShader::eShaderType targetShader);
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

	void SetConstantBuffer(const D3DBuffer &constantBuffer, uint32_t slot, D3DShader::eShaderType targetShader);

	void SetViewport(const D3DViewport &viewport, uint32_t slot = 0U);
	void SetScissorRect(const D3DRect &rect, uint32_t slot = 0U);

	void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive);
	void DrawInstanced();
	void DrawAuto();
protected:
	enum eResourceLimits
	{
		eMaxViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eMaxScissorRects = eMaxViewports,
		eMaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
		eMaxConstantBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT,
		eMaxRenderTargetViews = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		eMaxShaderResourceView = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
		eMaxUnorderedAccessViews = D3D11_PS_CS_UAV_REGISTER_COUNT,  /// D3D11_1_UAV_SLOT_COUNT
		eMaxVertexStream = 1  /// Custom
	};

	enum eDirtyFlag
	{
		eDInputLayout,
		eDVertexBuffer,
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
		
		eDScissorRect,
		eDViewport,

		eDPrimitiveTopology,

		eDirtyFlagCount
	};

	struct D3DPipeline
	{
	public:
		D3DInputLayout           InputLayout;
		D3DVertexBuffer          VertexBuffers[eMaxVertexStream];
		D3DIndexBuffer           IndexBuffer;
		D3DBuffer                ConstantBuffers[D3DShader::eShaderTypeCount][eMaxConstantBuffers];

		D3DVertexShader          VertexShader;
		D3DHullShader            HullShader;
		D3DDomainShader          DomainShader;
		D3DPixelShader           PixelShader;
		D3DGeometryShader        GeometryShader;
		D3DComputeShader         ComputeShader;

		D3DRasterizerState       RasterizerState;
		D3DSamplerState          SamplerStates[D3DShader::eShaderTypeCount][eMaxSamplers];
		D3DBlendState            BlendState;
		D3DDepthStencilState     DepthStencilState;

		D3DRenderTargetView      RenderTargetViews[eMaxRenderTargetViews];
		D3DDepthStencilView      DepthStencilView;
		D3DShaderResourceView    ShaderResourceViews[D3DShader::eShaderTypeCount][eMaxShaderResourceView];
		D3DUnorderedAccessView   UnorderedAccessViews[eMaxUnorderedAccessViews];

		D3DViewport              Viewports[eMaxViewports];
		D3DRect                  ScissorRects[eMaxScissorRects];

		uint32_t                 PrimitiveTopology = UINT32_MAX;
		uint32_t                 StencilRef = 0U;
		uint32_t                 BlendMask = 0U;
		float                    BlendFactor[4] = {};
		bool                     DirtyFlags[eDirtyFlagCount] = {};

		std::vector<ID3D11Buffer *> VertexBufferCache;
		std::vector<uint32_t> VertexStrideCache;
		std::vector<uint32_t> VertexOffsetCache;

		std::vector<ID3D11RenderTargetView *> RenderTargetViewCache;
		std::vector<ID3D11UnorderedAccessView *> UnorderedAccessViewCache;
		std::array<std::vector<ID3D11ShaderResourceView *>, D3DShader::eShaderTypeCount> ShaderResourceViewCache;

		std::array<std::vector<ID3D11Buffer *>, D3DShader::eShaderTypeCount> ConstantBufferCache;
		std::array<std::vector<ID3D11SamplerState *>, D3DShader::eShaderTypeCount> SamplerStateCache;

		std::vector<D3DViewport> ViewportCache;
		std::vector<D3DRect> ScissorRectCache;

		void CommitState(const D3DContext &IMContext);
	protected:
		void BindConstantBuffers(const D3DContext &IMContext);
		void BindSamplerStates(const D3DContext &IMContext);
		void BindShaderResourceViews(const D3DContext &IMContext);
		void BindUnorderedAccessViews(const D3DContext &IMContext);
	};

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

	D3DPipeline m_Pipeline;

	bool m_Inited = false;
};
