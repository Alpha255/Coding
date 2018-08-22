#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

///#define ColorMask

class AppBox : public RenderApp
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
#if !defined (ColorMask)
	Geometry::Mesh m_BoxMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader[eEffectCount];
	D3DBuffer m_CBufferVS;
	D3DShaderResourceView m_DiffuseTex;

	int32_t m_Effect = eNone;
#else
	Geometry::Mesh m_Mesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_DiffuseTexMiddle;
	D3DShaderResourceView m_DiffuseTexPoor;

	int32_t m_TexIndex = 0U;
#endif
};
