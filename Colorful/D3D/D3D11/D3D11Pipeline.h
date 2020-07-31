#pragma once

#include "Colorful/D3D/D3D11/D3D11State.h"

class D3D11GraphicsPipeline
{
public:
	D3D11GraphicsPipeline(const class D3D11Device& device, const GfxPipelineState* state);
protected:
private:
	D3D11RasterizerStatePtr m_RasterizerState = nullptr;
	D3D11BlendStatePtr m_BlendState = nullptr;
	D3D11DepthStencilStatePtr m_DepthStencilState = nullptr;
	const GfxPipelineState* m_GfxState = nullptr;
};
using D3D11GraphicsPipelinePtr = std::shared_ptr<D3D11GraphicsPipeline>;

class D3D11Context final : public D3DObject<ID3D11DeviceContext>
{
public:
	void setGraphicsPipeline(const D3D11GraphicsPipeline &pipeline);
};
