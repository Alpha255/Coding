#pragma once

#include "D3DTexture.h"
#include "D3DView.h"
#include "D3DBuffer.h"
#include "D3DState.h"

class D3DGeometryBuffer
{
public:
	enum eBufferType
	{
		eDepthStencil,
		eColorSpecIntensity,
		eNormal,
		eSpecPower,
		eBufferTypeCount
	};

	D3DGeometryBuffer() = default;
	~D3DGeometryBuffer() = default;

	void Init(uint32_t width, uint32_t height);
protected:
private:
	std::array<D3DShaderResourceView, eBufferTypeCount> m_ShaderResourceViews;

	D3DRenderTargetView m_SurfaceColorSpecIntensity;
	D3DRenderTargetView m_SurfaceNormal;
	D3DRenderTargetView m_SurfaceSpecPower;

	D3DDepthStencilView m_SurfaceDepthStencil;
	D3DDepthStencilView m_SurfaceDepthStencilReadonly;

	D3DDepthStencilState m_DepthStencilState;
	D3DBuffer m_ConstantBufUnpacked;
};
