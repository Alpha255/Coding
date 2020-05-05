#pragma once

#include "Colorful/D3D/D3D11/D3D11State.h"

class d3d11GraphicsPipeline
{
public:
protected:
private:
	d3d11RasterizerState m_RasterizerState;
	d3d11BlendState m_BlendState;
	d3d11DepthStencilState m_DepthStencilState;
};

class d3d11Context final : public D3DObject<ID3D11DeviceContext>
{
public:
	void setGraphicsPipeline(const d3d11GraphicsPipeline &pipeline);
};
