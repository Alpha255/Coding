#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppBox : public D3DApp
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	enum eSpecialEffect
	{
		eNone,
		eInversion,
		eGrayscale,
		eSharpen,
		eBlur,
		eEdgeDetection,
		eEffectCount
	};
private:
#if 0 
	Geometry::Mesh m_BoxMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader[eEffectCount];
	D3DBuffer m_CBufferVS;
	D3DShaderResourceView m_DiffuseTex;

	uint32_t m_Effect = eNone;
#endif
	Geometry::Mesh m_Mesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_DiffuseTexMiddle;
	D3DShaderResourceView m_DiffuseTexPoor;

	int32_t m_TexIndex = 0U;
};
