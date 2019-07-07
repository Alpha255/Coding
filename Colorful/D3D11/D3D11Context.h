#pragma once

#include "D3D11View.h"
#include "D3D11State.h"
#include "D3D11Buffer.h"
#include "D3D11Shader.h"
#include "D3D11Texture.h"
#include "D3D11ContextState.h"

class D3D11Context : public D3DObject<ID3D11DeviceContext>
{
public:
	void CreateAsDeferredContext();

	void SetRenderTargetView(const D3D11RenderTargetView &renderTarget, uint32_t slot = 0U);
	void SetDepthStencilView(const D3D11DepthStencilView &depthStencilView);
	void SetShaderResourceView(const D3D11ShaderResourceView &shaderResourceView, uint32_t slot, eRShaderType targetShader);
	void SetUnorderedAccessView(const D3D11UnorderedAccessView &unorderedAccessView, uint32_t slot, eRShaderType targetShader = eComputeShader);

	void SetVertexBuffer(const D3D11Buffer &vertexBuffer, uint32_t stride, uint32_t offset, uint32_t slot = 0U);
	void SetIndexBuffer(const D3D11Buffer &indexBuffer, uint32_t fmt, uint32_t offset);

	void SetSamplerState(const D3D11SamplerState &samplerState, uint32_t slot, eRShaderType targetShader);
	void SetRasterizerState(const D3D11RasterizerState &rasterizerState);
	void SetDepthStencilState(const D3D11DepthStencilState &depthStencilState, uint32_t stencilRef);
	void SetBlendState(const D3D11BlendState &blendState, Vec4 blendFactor = Vec4(0.0f, 0.0f, 0.0f, 0.0f), uint32_t mask = 0xFFFFFFFF);

	void SetInputLayout(const D3D11InputLayout &inputLayout);
	void SetVertexShader(const D3D11VertexShader &vertexShader);
	void SetHullShader(const D3D11HullShader &hullShader);
	void SetDomainShader(const D3D11DomainShader &domainShader);
	void SetPixelShader(const D3D11PixelShader &pixelShader);
	void SetGeometryShader(const D3D11GeometryShader &geometryShader);
	void SetComputeShader(const D3D11ComputeShader &computeShader);

	void SetUniformBuffer(const D3D11Buffer &constantBuffer, uint32_t slot, eRShaderType targetShader);

	void SetViewport(const D3D11Viewport &viewport, uint32_t slot = 0U);
	void SetScissorRect(const D3D11Rect &rect, uint32_t slot = 0U);

	void Draw(uint32_t vertexCount, uint32_t startVertex, uint32_t primitive);
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t offset, uint32_t primitive);

	inline void FinishCommandList(bool bRestore, D3D11CommandList &commandList)
	{
		if (m_bDeferred)
		{
			auto pCommandList = commandList.Get();
			m_Object->FinishCommandList(bRestore, &pCommandList);
			assert(pCommandList);
			commandList.Reset(pCommandList);
		}
		else
		{
			assert(0);
		}
	}

	inline void ExecuteCommandList(bool bRestore, D3D11CommandList &commandList)
	{
		assert(!m_bDeferred);

		m_Object->ExecuteCommandList(commandList.Get(), bRestore);
	}

	inline void ClearRenderTargetView(D3D11RenderTargetView &renderTarget, const Vec4 &color) const
	{
		assert(renderTarget.IsValid() && IsValid());

		m_Object->ClearRenderTargetView(renderTarget.Get(), (float *)&color);
	}

	inline void ClearDepthStencilView(D3D11DepthStencilView &depthStencil, uint32_t clearFlag, float depth, uint8_t stencil) const
	{
		assert(depthStencil.IsValid() && IsValid());

		m_Object->ClearDepthStencilView(depthStencil.Get(), clearFlag, depth, stencil);
	}

	inline void ForceCommitState()
	{
		assert(IsValid());

		m_State.CommitState(*this);
	}

	inline bool IsDeferred() const
	{
		return m_bDeferred;
	}
protected:
private:
	D3D11ContextState m_State;
	bool m_bDeferred = false;
};
