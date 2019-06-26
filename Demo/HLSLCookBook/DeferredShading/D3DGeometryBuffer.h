#pragma once

#include "D3DTexture.h"
#include "D3DView.h"
#include "D3DBuffer.h"
#include "D3DState.h"
#include "D3DShader.h"
#include "D3DMath.h"

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

	void Init(uint32_t width, uint32_t height, bool bReinit = false);

	void Resize(uint32_t width, uint32_t height);

	void Bind();

	void UnBind();

	void VisulizeGBuffer(bool bVisulize, const Vec4 &camPerspective);

	inline D3DShaderResourceView GetShaderResourceView(eBufferType type) const
	{
		assert(type < eBufferTypeCount);
		return m_ShaderResourceViews[type];
	}
protected:
	struct ConstantBufferPS
	{
		Vec4 Perspective;
	};
private:
	std::array<D3DShaderResourceView, eBufferTypeCount> m_ShaderResourceViews;

	D3DRenderTargetView m_SurfaceColorSpecIntensity;
	D3DRenderTargetView m_SurfaceNormal;
	D3DRenderTargetView m_SurfaceSpecPower;

	D3DDepthStencilView m_SurfaceDepthStencil;
	D3DDepthStencilView m_SurfaceDepthStencilReadonly;

	D3DDepthStencilState m_DepthStencilState;
	D3DBuffer m_CBufferPS;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PSm_PixelShader;
};
