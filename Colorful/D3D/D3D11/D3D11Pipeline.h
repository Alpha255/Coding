#pragma once

#include "Colorful/D3D/D3D11/D3D11State.h"

class d3d11GraphicsPipeline : public rGraphicsPipeline
{
public:
	void setInputLayout(const rInputLayout *inputLayout) override final {}

	void setShader(const rShader *shader) override final {}

	void setRasterizerState(
		eRPolygonMode polygonMode,
		eRCullMode cullMode,
		eRFrontFace frontFace,
		bool8_t enableDepthBias = false,
		float32_t depthBias = 0.0f,
		float32_t depthBiasClamp = 0.0f,
		float32_t depthBiasSlope = 0.0f) override final
	{
		m_RasterizerState.create(
			polygonMode,
			cullMode,
			frontFace,
			enableDepthBias,
			depthBias,
			depthBiasClamp,
			depthBiasSlope);
	}

	void setBlendState(
		bool8_t enableLogicOp,
		eLogicOp logicOp,
		uint32_t renderTargetCount,
		const rColorBlendState * const clrBlendState) override final
	{
		assert(renderTargetCount < eMaxRenderTargets);

		m_BlendState.create(
			enableLogicOp,
			logicOp,
			renderTargetCount,
			clrBlendState);
	}

	void setDepthStencilState(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOp &front,
		const rStencilOp &back) override final
	{
		m_DepthStencilState.create(
			enableDepth,
			enableDepthWrite,
			depthCompareOp,
			enableStencil,
			stencilReadMask,
			stencilWriteMask,
			front,
			back);
	}

	void setViewport(const rViewport *viewport) override final {}

	void setScissor(const rScissor *scissor) override final {}

	void build(const rRenderpass &renderpass) override final {}
protected:
private:
	d3d11RasterizerState m_RasterizerState;
	d3d11BlendState m_BlendState;
	d3d11DepthStencilState m_DepthStencilState;
};

class d3d11Context final : public d3dObject<ID3D11DeviceContext>
{
public:
	void setGraphicsPipeline(const d3d11GraphicsPipeline &pipeline);
};
