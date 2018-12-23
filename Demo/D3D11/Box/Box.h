#pragma once

#include "D3DApplication.h"

class AppBox : public RApplication
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void InitScene() override;
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
	Geometry::Mesh m_BoxMesh;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader[eEffectCount];
	RBuffer m_CBufferVS;
	RShaderResourceView m_DiffuseTex;

	int32_t m_Effect = eNone;
};
