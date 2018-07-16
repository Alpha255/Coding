#pragma once

#include "D3DMath.h"
#include "D3DView.h"
#include "D3DState.h"
#include "D3DBuffer.h"
#include "D3DShader.h"
#include "D3DContextState.h"

class D3DContext : public D3DObject<ID3D11DeviceContext>
{
public:
	void CreateAsDeferredContext();

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

	inline void ClearRenderTargetView(D3DRenderTargetView &renderTarget, const float *pColor = nullptr) const
	{
		assert(renderTarget.IsValid() && IsValid());

		pColor = (nullptr == pColor ? reinterpret_cast<const float*>(&Color::DarkBlue) : pColor);

		m_Object->ClearRenderTargetView(renderTarget.Get(), pColor);
	}

	inline void ClearDepthStencilView(D3DDepthStencilView &depthStencil, uint32_t clearFlag, float depth, uint8_t stencil) const
	{
		assert(depthStencil.IsValid() && IsValid());

		m_Object->ClearDepthStencilView(depthStencil.Get(), clearFlag, depth, stencil);
	}

	inline void ForceCommitState()
	{
		assert(IsValid());

		m_State.CommitState(*this);
	}
protected:
private:
	D3DContextState m_State;
};
