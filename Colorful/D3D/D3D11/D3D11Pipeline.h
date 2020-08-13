#pragma once

#include "Colorful/D3D/D3D11/D3D11State.h"
#include "Colorful/D3D/D3D11/D3D11PipelineState.h"
#include "Colorful/D3D/D3D11/D3D11Shader.h"

class D3D11GraphicsPipeline
{
public:
	D3D11GraphicsPipeline(const class D3D11Device& device, const GfxPipelineState* state);

	const GfxPipelineState* gfxPipelineState() const
	{
		return m_GfxState;
	}

	const D3D11RasterizerStatePtr& rasterizerState() const
	{
		return m_RasterizerState;
	}

	const D3D11BlendStatePtr& blendState() const
	{
		return m_BlendState;
	}

	const D3D11DepthStencilStatePtr& depthStencilState() const
	{
		return m_DepthStencilState;
	}

	const D3D11InputLayoutPtr& inputLayout() const
	{
		return m_InputLayout;
	}
protected:
private:
	D3D11RasterizerStatePtr m_RasterizerState = nullptr;
	D3D11BlendStatePtr m_BlendState = nullptr;
	D3D11DepthStencilStatePtr m_DepthStencilState = nullptr;
	D3D11InputLayoutPtr m_InputLayout;

	const GfxPipelineState* m_GfxState = nullptr;
};
using D3D11GraphicsPipelinePtr = std::shared_ptr<D3D11GraphicsPipeline>;

class D3D11Context final : public D3DObject<ID3D11DeviceContext>
{
public:
	void setGraphicsPipeline(const D3D11GraphicsPipelinePtr& pipeline);
protected:
	void setVertexIndexBuffers(const D3D11GraphicsPipelinePtr& pipeline);
	void setShaderResources(const D3D11GraphicsPipelinePtr& pipeline);
private:
	D3D11PipelineState m_State;
};
