#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3D11GraphicsPipeline::D3D11GraphicsPipeline(const D3D11Device& device, const GfxPipelineState* state)
	: m_GfxState(state)
{
	assert(state);

	m_RasterizerState = std::make_shared<D3D11RasterizerState>(device, state->RasterizerStateDesc);
	m_BlendState = std::make_shared<D3D11BlendState>(device, state->BlendStateDesc);
	m_DepthStencilState = std::make_shared<D3D11DepthStencilState>(device, state->DepthStencilStateDesc);
}

void D3D11Context::setGraphicsPipeline(const D3D11GraphicsPipelinePtr& pipeline)
{
	assert(pipeline);
}
